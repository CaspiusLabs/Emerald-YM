#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Read JieLi objdump -t symbols and report guarded audio RAM occupancy.

Never alters the ELF, linker script or firmware. The table lists *named*
symbols; section sizes are calculated from linker boundary symbols, not from
summing function sizes (which can overlap or omit padding).
"""
import argparse
import json
import re
from pathlib import Path

RAM_BASE = 0x01C00000
RAM_END = 0x01C06000
REFERENCE_USED = 24544  # exact physically tested v1.0.12, not a new build
REQUIRED = ("_rt_start", "_rt_end", "_audio_start", "_audio_end", "_fx_start", "_fx_end")
HEXDIGITS = re.compile(r"^[0-9a-fA-F]+$")


def read_symbols(text):
    addresses, functions = {}, []
    for line in text.splitlines():
        fields = line.strip().split()
        if len(fields) < 3 or not HEXDIGITS.fullmatch(fields[0]):
            continue
        address = int(fields[0], 16)
        name = fields[-1]
        if name in REQUIRED:
            addresses[name] = address
        for index in range(1, len(fields) - 2):
            if fields[index] not in (".ram_text", ".audio_hot"):
                continue
            if not HEXDIGITS.fullmatch(fields[index + 1]):
                continue
            size = int(fields[index + 1], 16)
            if size and RAM_BASE <= address < RAM_END and address + size <= RAM_END:
                functions.append({"name": name, "section": fields[index],
                                  "address": address, "size": size})
            break
    missing = set(REQUIRED) - set(addresses)
    if missing:
        raise ValueError("missing linker symbols: " + ", ".join(sorted(missing)))
    return addresses, functions


def summarize(text, baseline=REFERENCE_USED, goal=1024, top=25):
    a, functions = read_symbols(text)
    if not (RAM_BASE <= a["_rt_start"] <= a["_rt_end"] <=
            a["_audio_start"] <= a["_fx_start"] <= a["_fx_end"] <=
            a["_audio_end"] <= RAM_END):
        raise ValueError("invalid RAMTEXT/FX section order or guarded region")
    used = a["_audio_end"] - RAM_BASE
    free = RAM_END - a["_audio_end"]
    if not 0 <= baseline <= RAM_END - RAM_BASE:
        raise ValueError("baseline outside RAMTEXT region")
    if goal < 0 or top < 1:
        raise ValueError("goal must be nonnegative; top must be positive")
    functions = sorted(functions, key=lambda x: (-x["size"], x["name"]))
    return {
        "baseline_used_bytes": baseline,
        "ramtext_capacity_bytes": RAM_END - RAM_BASE,
        "ramtext_used_bytes": used,
        "ramtext_free_bytes": free,
        "delta_vs_stable_bytes": used - baseline,
        "reclaimed_vs_stable_bytes": baseline - used,
        "target_reclaim_bytes": goal,
        "target_met": baseline - used >= goal,
        "flash_driver_bytes": a["_rt_end"] - a["_rt_start"],
        "flash_to_audio_padding_bytes": a["_audio_start"] - a["_rt_end"],
        "audio_hot_bytes": a["_audio_end"] - a["_audio_start"],
        "fx_code_bytes": a["_fx_end"] - a["_fx_start"],
        "top_named_symbols": functions[:top],
        "named_symbol_count": len(functions),
        "symbol_size_notice": "individual symbol sizes are informative, not additive",
    }


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--symbols", type=Path, default=Path("build/amb.syms"))
    ap.add_argument("--json", type=Path, default=Path("build/ramtext-report.json"))
    ap.add_argument("--baseline", type=int, default=REFERENCE_USED)
    ap.add_argument("--goal", type=int, default=1024)
    ap.add_argument("--top", type=int, default=25)
    ap.add_argument("--require-goal", action="store_true",
                    help="fail unless the required number of bytes was reclaimed")
    args = ap.parse_args()
    try:
        report = summarize(args.symbols.read_text(encoding="utf-8"),
                           args.baseline, args.goal, args.top)
    except (OSError, ValueError) as e:
        ap.error(str(e))
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print("RAMTEXT used {ramtext_used_bytes}/{ramtext_capacity_bytes} B; "
          "free {ramtext_free_bytes} B; delta {delta_vs_stable_bytes:+d} B".format(**report))
    print("flash driver {flash_driver_bytes} B; audio code {audio_hot_bytes} B; "
          "FX subsection {fx_code_bytes} B".format(**report))
    print("Goal: reclaim {} B vs stable 1.0.12; {}".format(
        args.goal, "MET" if report["target_met"] else "NOT MET"))
    for symbol in report["top_named_symbols"]:
        print("  {:>5d} B  {:<11s} {}".format(
            symbol["size"], symbol["section"], symbol["name"]))
    if args.require_goal and not report["target_met"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
