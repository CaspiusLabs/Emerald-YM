#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Offline pi32v2 compile + unresolved SDK dependency/resource probe. No device IO.

Exit success means the OBJECT probe completed, never a deployable A2DP firmware.
The report explicitly records unresolved symbols and whether the strict link fails.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]
PIN = "d179b4484759423312073f5fbb232501aa491047"


def sections(path, include_addresses=False):
    data = path.read_bytes()
    if data[:6] != b"\x7fELF\x01\x01":
        raise RuntimeError("Expected ELF32 little endian")
    offset = struct.unpack_from("<I", data, 32)[0]
    stride, count, names_idx = struct.unpack_from("<HHH", data, 46)
    headers = [struct.unpack_from("<10I", data, offset + i * stride)
               for i in range(count)]
    name_header = headers[names_idx]
    names = data[name_header[4]:name_header[4] + name_header[5]]
    return [dict(name=names[h[0]:].split(b"\0")[0].decode(),
                 **({"address": h[3]} if include_addresses else {}),
                 bytes=h[5], writable=bool(h[2] & 1),
                 executable=bool(h[2] & 4), nobits=h[1] == 8)
            for h in headers if h[2] & 2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, default=ROOT / ".runtime/ac79-bt-sdk")
    parser.add_argument("--toolchain", type=Path, default=ROOT / ".runtime/toolchain/pi32")
    parser.add_argument("--with-runtime", action="store_true",
                        help="Also audit SDK OS/CPU/filesystem/crypto/RF dependencies")
    args = parser.parse_args()
    sdk, tc = args.sdk.resolve(), args.toolchain.resolve()
    out = ROOT / "build/bt-prototype"
    if args.with_runtime:
        out /= "runtime"
    out.mkdir(parents=True, exist_ok=True)
    head = subprocess.check_output(["git", "-C", str(sdk), "rev-parse", "HEAD"], text=True).strip()
    if head != PIN:
        raise SystemExit(f"SDK must be pinned to {PIN}; found {head}")

    def run(name, command, require=True):
        result = subprocess.run([str(x) for x in command], cwd=ROOT,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out / (name + ".txt")).write_bytes(result.stdout)
        if require and result.returncode:
            raise RuntimeError(f"{name} failed; see {out / (name + '.txt')}")
        return result

    flags = ["-target", "pi32v2", "-mcpu=r3", "-mfprev1", "-ffp-contract=off",
             "-Os", "-fno-builtin", "-ffunction-sections", "-fdata-sections", "-Wall"]
    includes = [sdk / "include_lib" / p for p in
                ["", "system", "utils", "driver", "driver/cpu/wl82",
                 "system/generic", "system/os/FreeRTOS/pi32v2"]]
    includes += [tc / "include/libc"]
    include_flags = ["-I" + str(p) for p in includes]
    sources = ["pcm_queue", "sbc_queue", "sdk_encoder"]
    objects = []
    for name in sources:
        obj = out / (name + ".o")
        run(name + "-compile", [tc / "bin/clang.exe", *flags, *include_flags,
            "-DEYM_BT_PI32=1", "-DCONFIG_FREE_RTOS_ENABLE=1", "-Werror", "-c",
            ROOT / "prototypes/bluetooth-audio" / (name + ".c"), "-o", obj])
        objects.append(obj)
    undefined_pcm = run("pcm-undefined", [tc / "bin/llvm-nm.exe", "-u", objects[0]]).stdout.decode()
    if undefined_pcm.strip():
        raise RuntimeError("PCM queue must have no external/atomic/allocator calls: " + undefined_pcm)
    run("pcm-disassembly", [tc / "bin/llvm-objdump.exe", "-d", "-r", objects[0]])
    lib_names = ["audio_server.a", "btstack.a", "btctrler.a", "lib_sbc_eng.a"]
    if args.with_runtime:
        lib_names += ["system.a", "cpu.a", "common_lib.a", "fs.a",
                      "crypto_toolbox_Osize.a", "wl_rf_common.a"]
    libs = [sdk / "cpu/wl82/liba" / name for name in lib_names]
    # Keep the actual codec registration, not merely server_open's string lookup.
    # No config stubs, fake OS, changed placement or ignored-symbol executable.
    link = [tc / "bin/pi32v2-ld.exe", "-plugin", tc / "bin/LLVMgold.dll",
            "-u", "audio_server_info", "-u", "subdev_sbc_enc", "-u", "btstack_init",
            *objects, "--start-group", *libs, "--end-group"]
    combined = out / "sdk-probe.o"
    run("sdk-partial-link", [*link, "-r", "-o", combined])
    unresolved = run("sdk-undefined", [tc / "bin/llvm-nm.exe", "-u", combined]).stdout.decode()
    symbols = sorted(line.split()[-1] for line in unresolved.splitlines() if line.strip())
    strict = run("sdk-strict-link", [*link, "-e", "eym_bt_encoder_open",
                                      "--no-undefined", "-o", out / "not-firmware.elf"], require=False)
    # Do not leave even a mistakenly successful output available as a candidate.
    (out / "not-firmware.elf").unlink(missing_ok=True)
    sec = sections(combined)
    report = {
        "scope": "isolated target objects and unconfigured SDK dependency closure; NOT firmware",
        "sdk_commit": head, "target_compile_passed": True,
        "with_sdk_runtime": args.with_runtime,
        "partial_link_passed": True, "strict_link_passed": strict.returncode == 0,
        "hardware_accessed": False, "installable_package_created": False,
        "pcm_external_symbols": [],
        "objects": {p.stem: sections(p) for p in objects},
        "sdk_library_sha256": {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in libs},
        "unresolved_symbol_count": len(symbols), "unresolved_symbols": symbols,
        "partial_link_sections": sec,
        "partial_link_allocated_bytes": sum(s["bytes"] for s in sec),
        "partial_link_writable_bytes": sum(s["bytes"] for s in sec if s["writable"]),
        "partial_link_executable_bytes": sum(s["bytes"] for s in sec if s["executable"]),
        "warning": "No final placement/GC/profile config, task stacks, heap or RF buffers. These sums do not prove final fit or minimum A2DP cost.",
    }
    (out / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: v for k, v in report.items() if k not in
                      ("objects", "partial_link_sections", "unresolved_symbols")}, indent=2))


if __name__ == "__main__":
    main()
