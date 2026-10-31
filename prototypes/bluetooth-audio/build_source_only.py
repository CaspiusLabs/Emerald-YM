#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Source-only SDK optimization/link audit. No packaging or hardware access."""
import json
import argparse
import re
import hashlib
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from build_probe import ROOT, PIN, sections


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference-rf", action="store_true",
                        help="Use the pinned SDK demo board RF values for an OFFLINE link audit, never FM-1")
    args = parser.parse_args()
    sdk = ROOT / ".runtime/ac79-bt-sdk"
    tc = ROOT / ".runtime/toolchain/pi32"
    out = ROOT / "build/bt-prototype/source-only"
    if args.reference_rf:
        out /= "reference-rf"
    out.mkdir(parents=True, exist_ok=True)
    if subprocess.check_output(["git", "-C", str(sdk), "rev-parse", "HEAD"], text=True).strip() != PIN:
        raise RuntimeError("Wrong SDK revision")

    def run(name, cmd, check=True):
        result = subprocess.run([str(x) for x in cmd], cwd=ROOT, capture_output=True)
        (out / (name + ".txt")).write_bytes(result.stdout + result.stderr)
        if check and result.returncode:
            raise RuntimeError("Failed: " + name)
        return result

    profile = ROOT / "prototypes/bluetooth-audio/source-only"
    incs = [profile, sdk / "apps/common/include", sdk / "apps/common/config/include"]
    incs += [sdk / "include_lib" / p for p in ["", "system", "utils", "driver",
        "driver/cpu/wl82", "driver/device", "system/generic", "system/os/FreeRTOS/pi32v2",
        "btctrler", "btctrler/port/wl82", "btstack", "media", "utils/syscfg", "utils/event",
        "net", "net/lwip_2_1_3/lwip/src/include", "net/lwip_2_1_3/lwip/port", "update"]]
    incs += [tc / "include/libc"]
    flags = ["-target", "pi32v2", "-mcpu=r3", "-mfprev1", "-Oz", "-flto",
             "-mllvm", "-pi32v2-large-program=true",
             "-ffunction-sections", "-fdata-sections", "-fno-common",
             "-DCONFIG_FREE_RTOS_ENABLE=1", "-DCONFIG_THREAD_ENABLE=1",
             "-DCONFIG_CPU_WL82=1", "-DCONFIG_NO_SDRAM_ENABLE=1",
             "-DUSE_SDFILE_NEW=1", "-D__GCC_PI32V2__=1",
             *["-I" + str(p) for p in incs]]
    sdk_units = ["apps/common/config/bt_profile_config.c",
        "apps/common/config/log_config/lib_btstack_config.c",
        "apps/common/config/log_config/lib_btctrler_config.c",
        "apps/common/config/log_config/lib_system_config.c",
        "apps/common/config/log_config/lib_driver_config.c",
        "apps/common/audio_music/audio_config.c", "apps/common/system/init.c",
        "apps/common/config/user_cfg.c", "apps/common/net/wifi_conf.c",
        "apps/common/system/system_reset_reason.c", "cpu/wl82/key/adc_api.c",
        "apps/common/update/update.c",
        "cpu/wl82/debug.c", "cpu/wl82/setup.c"]
    objects = []
    for unit in sdk_units:
        obj = out / (Path(unit).stem + ".o")
        source = sdk / unit
        if obj.stem == "lib_btctrler_config":
            text = source.read_text(encoding="utf-8")
            for setting in ["CONFIG_TEST_DUT_CODE", "CONFIG_TEST_FCC_CODE", "CONFIG_LMP_MASTER_ESCO_ENABLE"]:
                text, count = re.subn(r"(const int\s+" + setting + r"\s*=\s*)1(\s*;)", r"\g<1>0\2", text)
                if count != 1:
                    raise RuntimeError("Unexpected SDK definition: " + setting)
            source = out / "lib_btctrler_config.generated.c"
            source.write_text(text, encoding="utf-8")
        if obj.stem == "bt_profile_config":
            text = source.read_text(encoding="utf-8")
            # Disable SDK private app/HFP extras explicitly. The legacy ADV
            # selector can compare equal to undefined 0 in a BLE-off profile.
            for setting in ["pbg_support_enable", "adt_profile_support", "more_hfp_cmd_support"]:
                text, count = re.subn(r"(const u8\s+" + setting + r"\s*=\s*)1(\s*;)", r"\g<1>0\2", text)
                if count != 1:
                    raise RuntimeError("Unexpected SDK profile definition: " + setting)
            source = out / "bt_profile_config.generated.c"
            source.write_text(text, encoding="utf-8")
        run(obj.stem, [tc / "bin/clang.exe", *flags, "-c", source, "-o", obj])
        objects.append(obj)
    obj = out / "platform_policy.o"
    run("platform_policy", [tc / "bin/clang.exe", *flags, "-c", profile / "platform_policy.c", "-o", obj])
    objects.append(obj)
    obj = out / "profile_contract.o"
    run("profile_contract", [tc / "bin/clang.exe", *flags, "-Werror", "-c", profile / "profile_contract.c", "-o", obj])
    objects.append(obj)
    if args.reference_rf:
        # Extract the actual reference declaration. Never use this silently as
        # FM-1 calibration, never compile the demo board GPIO/power/startup.
        board = sdk / "apps/demo/demo_edr/board/wl82/board.c"
        matches = re.findall(r"const struct wifi_calibration_param wifi_calibration_param\s*=\s*\{.*?\};",
                             board.read_text(encoding="utf-8"), re.S)
        if len(matches) != 1:
            raise RuntimeError("Unexpected SDK reference RF declaration")
        source = out / "reference_rf.generated.c"
        source.write_text('#include "generic/typedef.h"\n#include "wifi/wifi_connect.h"\n' + matches[0] + '\n', encoding="utf-8")
        obj = out / "reference_rf.o"
        run("reference_rf", [tc / "bin/clang.exe", *flags, "-c", source, "-o", obj])
        objects.append(obj)
    for name in ["pcm_queue", "sbc_queue", "sdk_encoder"]:
        obj = out / (name + ".o")
        run(name, [tc / "bin/clang.exe", *flags, "-Wall", "-Werror", "-DEYM_BT_PI32=1", "-c",
                  ROOT / "prototypes/bluetooth-audio" / (name + ".c"), "-o", obj])
        objects.append(obj)
    lib_names = ["audio_server.a", "btstack.a", "btctrler.a", "system.a", "cpu.a",
                 "common_lib.a", "fs.a", "event.a", "cfg_tool.a", "update.a",
                 "crypto_toolbox_Osize.a", "wl_rf_common.a"]
    libs = [sdk / "cpu/wl82/liba" / x for x in lib_names]
    libs += [sdk / "cpu/wl82/liba/lib_sbc_eng.a"]
    libs += [sdk / "include_lib/newlib/pi32v2-lib" / x for x in
             ["libc.a", "libcompiler_rt.a", "libm.a"]]
    roots = ["audio_server_info", "subdev_sbc_enc", "sbc_package_ops", "vir_dev_sub",
             "btstack_init", "a2dp_src_sdp_record_item", "a2dp_source_sbc_codec",
             "eym_bt_encoder_open", "eym_bt_encoder_push", "eym_bt_encoder_close",
             "a2dp_sbc_encoder_get_data"]
    link = [tc / "bin/pi32v2-ld.exe", "-plugin", tc / "bin/LLVMgold.dll",
            "--plugin-opt=mcpu=r3", "--plugin-opt=-mattr=+fprev1",
            "--plugin-opt=-function-sections", "--plugin-opt=-data-sections",
            "--plugin-opt=-pi32v2-large-program=true",
            *[a for name in roots for a in ["-u", name]], *objects,
            "--start-group", *libs, "--end-group"]
    combined = out / "source-only.o"
    run("partial-link", [*link, "-r", "--gc-sections", "-o", combined])
    result = run("undefined", [tc / "bin/llvm-nm.exe", "-u", combined])
    names = sorted(x.split()[-1] for x in result.stdout.decode().splitlines() if x.strip())
    sec = sections(combined)
    # The archive packs SBC and JLA descriptors in one input section. Selecting
    # the whole table would expose both codecs even with JLA encoding absent.
    # Bound the public table to the actual SBC symbol's ELF metadata, without
    # inventing a private descriptor struct or changing the SDK archive.
    sizes = run("symbol-sizes", [tc / "bin/llvm-nm.exe", "-S", combined]).stdout.decode()
    match = re.search(r"^[0-9a-fA-F]+\s+([0-9a-fA-F]+)\s+\w\s+a2dp_source_sbc_codec$", sizes, re.M)
    if not match or int(match[1], 16) != 24:
        raise RuntimeError("Pinned SDK SBC descriptor layout changed")
    table_source = sdk / "include_lib/btstack/btstack_lib_text.ld"
    table = table_source.read_text(encoding="utf-8")
    old = re.compile(r"a2dp_source_media_codec_begin = \.;\s*KEEP\(\*\(\.a2dp_source_media_codec\)\)\s*a2dp_source_media_codec_end = \.;")
    table, count = old.subn("KEEP(*(.a2dp_source_media_codec))\n"
        "a2dp_source_media_codec_begin = a2dp_source_sbc_codec;\n"
        "a2dp_source_media_codec_end = a2dp_source_sbc_codec + 24;", table)
    if count != 1:
        raise RuntimeError("Unexpected SDK source-codec linker table")
    (out / "btstack_source_only.ld").write_text(table, encoding="utf-8")
    report = {"sdk_commit": PIN, "reference_rf": args.reference_rf,
              "hardware_accessed": False, "installable_package_created": False,
              "fm1_port_validated": False, "unresolved_symbols": names,
              "profile_contract_compiled": True, "advertised_source_codec": "SBC only",
              "sbc_descriptor_bytes": 24, "negotiation_adapter_implemented": False,
              "audit_completed_not_firmware_success": True,
              "task_table_stack_queue_budget_bytes": 29440,
              "unresolved_symbol_count": len(names), "sections": sec,
              "writable_bytes": sum(x["bytes"] for x in sec if x["writable"]),
              "executable_bytes": sum(x["bytes"] for x in sec if x["executable"])}
    script = out / "audit.ld"
    run("link-script", [tc / "bin/clang.exe", *flags, "-I" + str(out), "-E", "-P", "-x", "c",
                        "-include", profile / "app_config.h", profile / "audit.ld.S", "-o", script])
    strict = run("strict-link", [*link, "--gc-sections", "--no-undefined", "-T", script,
                                "-e", "eym_bt_encoder_open", "-o", out / "not-firmware.elf"], check=False)
    report["strict_link_passed"] = strict.returncode == 0
    diagnostics = (strict.stdout + strict.stderr).decode(errors="replace")
    report["strict_undefined_symbols"] = sorted(set(re.findall(r"undefined reference to [`']([^']+)'", diagnostics)))
    report["library_sha256"] = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in libs}
    report["sdk_source_sha256"] = {unit: hashlib.sha256((sdk / unit).read_bytes()).hexdigest() for unit in sdk_units}
    report["sdk_source_sha256"]["include_lib/btstack/btstack_lib_text.ld"] = hashlib.sha256(table_source.read_bytes()).hexdigest()
    if args.reference_rf:
        report["sdk_source_sha256"]["apps/demo/demo_edr/board/wl82/board.c"] = hashlib.sha256(board.read_bytes()).hexdigest()
    if strict.returncode == 0:
        elf = out / "not-firmware.elf"
        report["linked_sections"] = sections(elf, include_addresses=True)
        symbols = run("defined", [tc / "bin/llvm-nm.exe", "--defined-only", elf]).stdout.decode()
        report["linked_root_presence"] = {name: bool(re.search(r"\b" + name + r"$", symbols, re.M)) for name in roots}
        report["final_undefined"] = run("final-undefined", [tc / "bin/llvm-nm.exe", "-u", elf]).stdout.decode()
        if not all(report["linked_root_presence"].values()) or report["final_undefined"].strip():
            raise RuntimeError("Incomplete linked audit: missing roots or remaining undefined symbols")
        # A weak SDK fallback can be inlined away by LTO. Its absence in nm is
        # NOT evidence of connected negotiation; the adapter is still unwired.
        table_symbols = run("final-symbol-sizes", [tc / "bin/llvm-nm.exe", "-S", elf]).stdout.decode()
        values = {m[3]: int(m[1], 16) for m in re.finditer(r"^([0-9a-fA-F]+)\s+(?:[0-9a-fA-F]+\s+)?(\w)\s+(\S+)$", table_symbols, re.M)}
        if values.get("a2dp_source_media_codec_begin") != values.get("a2dp_source_sbc_codec") or values.get("a2dp_source_media_codec_end", 0) - values.get("a2dp_source_media_codec_begin", 0) != 24:
            raise RuntimeError("Source codec selection table must contain SBC only")
        report["source_table_verified"] = True
        report["unselected_jla_descriptor_retained"] = "a2dp_source_jla_codec" in values
        report["soft_double_symbols"] = sorted({x.split()[-1] for x in symbols.splitlines()
            if x.strip() and re.match(r"__(?:add|sub|mul|div|cmp|eq|ne|gt|ge|lt|le|unord|fix|float|extend|trunc).*df", x.split()[-1])})
        regions = {"XIP": (0x02000120, 0x8DFBC), "RAMTEXT": (0x01C00000, 0x6000),
                   "RAM": (0x01C08000, 98304), "POOL": (0x01C20000, 344064)}
        report["region_section_bytes"] = {name: sum(s["bytes"] for s in report["linked_sections"]
            if start <= s["address"] < start + size) for name, (start, size) in regions.items()}
        # Comparison is a conservative coexistence budget, not a combined link:
        # SDK replaces/duplicates some bare-metal services and startup code.
        baseline = {"RAMTEXT": 24492, "RAM": 39120, "POOL": 101888}
        report["baseline_plus_sdk_region_bytes"] = {name: value + report["region_section_bytes"][name]
                                                    for name, value in baseline.items()}
        report["region_capacity_bytes"] = {name: pair[1] for name, pair in regions.items()}
    (out / "not-firmware.elf").unlink(missing_ok=True)
    (out / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("reference_rf", "strict_link_passed", "unresolved_symbol_count",
          "strict_undefined_symbols", "writable_bytes", "executable_bytes", "region_section_bytes",
          "baseline_plus_sdk_region_bytes", "source_table_verified") if k in report}, indent=2))


if __name__ == "__main__":
    main()
