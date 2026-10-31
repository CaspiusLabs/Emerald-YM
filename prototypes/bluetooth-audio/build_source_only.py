#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Source-only SDK optimization/link audit. No packaging or hardware access."""
import json
import argparse
import re
import hashlib
import subprocess
import sys
import shutil
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from build_probe import ROOT, PIN, sections
from elf_namespace import rename_defined_symbol


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference-rf", action="store_true",
                        help="Use the pinned SDK demo board RF values for an OFFLINE link audit, never FM-1")
    parser.add_argument("--lean-virtual", action="store_true",
                        help="Offline virtual-only archive profile and 1024-frame/4-slot queues")
    parser.add_argument("--combined-preflight", action="store_true",
                        help="Attempt strict link with current Emerald objects, never create firmware")
    args = parser.parse_args()
    if args.combined_preflight and not (args.lean_virtual and args.reference_rf):
        parser.error("Combined preflight requires --lean-virtual --reference-rf")
    sdk = ROOT / ".runtime/ac79-bt-sdk"
    tc = ROOT / ".runtime/toolchain/pi32"
    out = ROOT / "build/bt-prototype/source-only"
    if args.lean_virtual:
        out /= "lean-virtual"
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
    if args.lean_virtual:
        flags += ["-DEYM_BT_PCM_FRAMES=1024", "-DEYM_BT_SBC_SLOTS=4", "-DEYM_BT_USE_POOL=1"]
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
    original_libs = list(libs)
    if args.lean_virtual:
        # Remove only the unused physical audio output registration member from
        # an ignored PRIVATE COPY, never the provisioned SDK archive. Any real
        # dependency on it must fail the strict link, not resolve to a stub.
        source_cpu = sdk / "cpu/wl82/liba/cpu.a"
        private_cpu = out / "virtual-only-cpu.a"
        members = run("cpu-members", [tc / "bin/llvm-ar.exe", "t", source_cpu]).stdout.decode().splitlines()
        if members.count("subdevice_dac.c.o") != 1:
            raise RuntimeError("Pinned physical-output archive member changed")
        shutil.copyfile(source_cpu, private_cpu)
        run("remove-physical-output", [tc / "bin/llvm-ar.exe", "d", private_cpu, "subdevice_dac.c.o"])
        libs = [private_cpu if p == source_cpu else p for p in libs]
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
              "lean_virtual": args.lean_virtual,
              "pcm_frames": 1024 if args.lean_virtual else 2048,
              "sbc_slots": 4 if args.lean_virtual else 8,
              "queue_pool_storage": args.lean_virtual,
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
                                "-Map=" + str(out / "sdk.map"),
                                "-e", "eym_bt_encoder_open", "-o", out / "not-firmware.elf"], check=False)
    report["strict_link_passed"] = strict.returncode == 0
    diagnostics = (strict.stdout + strict.stderr).decode(errors="replace")
    report["strict_undefined_symbols"] = sorted(set(re.findall(r"undefined reference to [`']([^']+)'", diagnostics)))
    report["library_sha256"] = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in libs}
    report["original_library_sha256"] = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in original_libs}
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
        if args.lean_virtual:
            if any(n in values for n in ("subdev_dac", "subdev_iis_dec", "sub_dac_init")):
                raise RuntimeError("Virtual-only build retains physical audio output driver")
            report["physical_output_driver_absent"] = True
            if not all(0x01C20000 <= values.get(n, 0) < 0x01C74000 for n in ("pcm_queue", "queue")):
                raise RuntimeError("Lean queues must occupy existing non-executable POOL")
            report["queue_pool_addresses_verified"] = True
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
    if args.combined_preflight:
        # A real strict-link attempt against unchanged production objects, not
        # merely added sizes. No ignored symbols, duplicate-definition override,
        # success shims, expanded regions or exported installable output.
        app = ROOT / "build/bt-prototype/current-main"
        app_report = json.loads((app / "report.json").read_text(encoding="utf-8"))
        if app_report["production_version"] != "1.0.4" or not app_report["production_sources_match_main"]:
            raise RuntimeError("Build the current production app audit first")
        app_objects = [app / (n + ".o") for n in ("crt0", "fm1_vec", "fm1_isr", "main")]
        app_objects += sorted((app / "o2").glob("*.o"))
        if not all(p.is_file() for p in app_objects) or len(app_objects) != 8:
            raise RuntimeError("Incomplete production object set")
        # The SDK startup member also contains cpu1_start, required by its RTOS
        # port. Keep its real code but namespace its competing boot entry. This
        # resolves a symbol collision only; it does not port dual-core startup.
        combined_cpu = out / "emerald-cpu.a"
        shutil.copyfile(private_cpu, combined_cpu)
        startup = out / "startup.S.o"
        startup.write_bytes(run("extract-startup", [tc / "bin/llvm-ar.exe", "p", combined_cpu, "startup.S.o"]).stdout)
        startup_names = run("startup-symbols", [tc / "bin/llvm-nm.exe", startup]).stdout.decode()
        if len(re.findall(r"\bT\s+_start$", startup_names, re.M)) != 1:
            raise RuntimeError("Pinned SDK startup entry changed")
        startup.write_bytes(rename_defined_symbol(startup.read_bytes(), "_start", "eym_sdk_boot_start"))
        new_names = run("namespaced-startup-symbols", [tc / "bin/llvm-nm.exe", startup]).stdout.decode()
        if re.search(r'\bT\s+_start$', new_names, re.M) or not re.search(r'\bT\s+eym_sdk_boot_start$', new_names, re.M):
            raise RuntimeError("SDK boot entry namespace failed")
        if sorted(new_names.replace('eym_sdk_boot_start', '_start').splitlines()) != sorted(startup_names.splitlines()):
            raise RuntimeError("Unexpected change to other SDK startup symbols")
        run("replace-sdk-entry", [tc / "bin/llvm-ar.exe", "r", combined_cpu, startup])
        # Keep the original regions, protected stacks, noinit and audio layout.
        # Add real SDK registry/data declarations to a private generated script.
        app_ld = (ROOT / "firmware/app.ld").read_text(encoding="utf-8")
        app_ld = app_ld.replace('.text : {', '.text : {\ntext_rodata_begin = .;\n'
            '#include "btstack_source_only.ld"\n#include "driver/cpu/wl82/system.ld"\n'
            '#include "driver/cpu/wl82/system_data.ld"\n#include "server/media.ld"\n'
            '*(.classic*_code .classic*_const .hci*_code .hci*_const .link*_code .link*_const .lmp*_code .lmp*_const .bt_rf_code .bt_rf_const .sbc_eng_code .sbc_eng_const)\n', 1)
        app_ld = app_ld.replace('.data : { . = ALIGN(4); _data_start = .;', '.sdk_ram_code : { ram_text_rodata_begin = .; '
            '*(.vm .spi_code .flushinv_icache .volatile_ram_code .os_str .uECC_const .crypto_const) ram_text_rodata_end = .; } > RAMTEXT AT > XIP\n'
            '.data : { . = ALIGN(4); _data_start = .; *(.volatile_ram .non_volatile_ram .os_data .os_port_data .sram .boot_info)\n'
            '#include "btctrler/btctler_lib_data.ld"\n#include "btstack/btstack_lib_data.ld"\n', 1)
        app_ld = app_ld.replace('.bss (NOLOAD) : { _bss_start = .;', '.bss (NOLOAD) : { _bss_start = .; *(.mem_heap .os_bss .os_port_bss) '
            'KEEP(*(.cpu0_ustack .cpu1_ustack))\n#include "btctrler/btctler_lib_bss.ld"\n'
            '#include "btstack/btstack_lib_bss.ld"\n'
            'cpu0_sstack_begin = .; KEEP(*(.cpu0_sstack)) cpu0_sstack_end = .; '
            'cpu1_sstack_begin = .; KEEP(*(.cpu1_sstack)) cpu1_sstack_end = .;\n', 1)
        app_ld = app_ld.replace('/DISCARD/ :', '.cache_data (NOLOAD) : { cache_ram_data_vma = .; cache_ram_data_lma = .; } > RAM\n'
            'cache_ram_data_size = SIZEOF(.cache_data);\n'
            '.cache_bss (NOLOAD) : { cache_ram_bss_vma = .; } > RAM\n'
            'cache_ram_bss_size = SIZEOF(.cache_bss);\n/DISCARD/ :', 1)
        # Compatibility aliases are actual linker bounds, never fake services.
        app_ld += '\nHEAP_BEGIN = ALIGN(_pool_end,32); HEAP_END = ORIGIN(POOL)+LENGTH(POOL)-8192;\n'
        app_ld += 'RAM0_SIZE=LENGTH(RAM); SDRAM_SIZE=0; CACHE_RAM_SIZE=0; FREE_DACHE_WAY=0; FREE_IACHE_WAY=0;\n'
        app_ld += 'text_rodata_end=_etext; text_size=SIZEOF(.text); data_size=SIZEOF(.data); data_vma=ADDR(.data); data_lma=LOADADDR(.data); bss_size=SIZEOF(.bss); bss_begin=ADDR(.bss);\n'
        app_ld += 'ram0_data_size=SIZEOF(.data); ram0_bss_size=SIZEOF(.bss); _ram0_data_size=SIZEOF(.data); _ram0_data_vma=ADDR(.data); _ram0_data_lma=LOADADDR(.data); _ram0_bss_size=SIZEOF(.bss); _ram0_bss_vma=ADDR(.bss);\n'
        app_ld = re.sub(r'(?<![<>!])=(?!=)', ' = ', app_ld)
        src = out / "combined.generated.ld.S"
        src.write_text(app_ld, encoding="utf-8")
        ld = out / "combined.ld"
        run("combined-script", [tc / "bin/clang.exe", *flags, "-I" + str(out), "-E", "-P", "-x", "c", src, "-o", ld])
        i = link.index("--start-group")
        combined_link = [combined_cpu if p == private_cpu else p for p in [*link[:i], *app_objects, *link[i:]]]
        elf = out / "combined-not-firmware.elf"
        result = run("combined-link", [*combined_link, "--gc-sections", "--no-undefined", "-u", "_start", "-e", "_start",
            "-T", ld, "-Map=" + str(out / "combined.map"), "-o", elf], check=False)
        report["combined_preflight"] = {"attempted": True, "link_passed": result.returncode == 0,
            "startup_or_renderer_wired": False, "production_app_sha256": app_report["production_app_sha256"],
            "sdk_boot_entry_namespaced": True, "sdk_cpu1_start_retained_not_ported": True,
            "combined_cpu_archive_sha256": hashlib.sha256(combined_cpu.read_bytes()).hexdigest(),
            "production_object_sha256": {str(p.relative_to(app)): hashlib.sha256(p.read_bytes()).hexdigest() for p in app_objects},
            "diagnostics": (result.stdout + result.stderr).decode(errors="replace")}
        elf.unlink(missing_ok=True)
        if "syntax error" in report["combined_preflight"]["diagnostics"]:
            raise RuntimeError("Combined audit script is invalid; this is not an integration result")
    (out / "not-firmware.elf").unlink(missing_ok=True)
    (out / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("reference_rf", "strict_link_passed", "unresolved_symbol_count",
          "strict_undefined_symbols", "writable_bytes", "executable_bytes", "region_section_bytes",
          "baseline_plus_sdk_region_bytes", "source_table_verified") if k in report}, indent=2))


if __name__ == "__main__":
    main()
