#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Offline production-app/resource audit; no loader, package, USB or radio IO."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(Path(__file__).resolve().parent))
import build
from build_probe import sections


def main():
    os.chdir(ROOT)
    os.environ['JIELI_TOOLCHAIN'] = str(ROOT / '.runtime/toolchain')
    os.environ['EN_FONT_NORAQM'] = '1'
    os.environ['X0X_FONTSET'] = 'terminus'
    out = ROOT / 'build/bt-prototype/current-main'
    out.mkdir(parents=True, exist_ok=True)
    # The production app retains the exact stable identity/version. This audit
    # never invokes build.main/build_loader or any packaging/installer routine.
    build.OUT = out
    build.VERSION = '1.0.4'
    build.PRODUCT = 'FM-1_68010004'
    build.generate()
    image, symbols, disassembly, ramtext = build.build_app()
    errors, notes = build.check(image, symbols, disassembly, ramtext)
    errors += build.mmio_check()
    if errors:
        raise RuntimeError('\n'.join(errors))
    sec = sections(out / 'amb.elf', include_addresses=True)
    byname = {s['name']: s for s in sec}
    def span(first, last):
        return byname[last]['address'] + byname[last]['bytes'] - byname[first]['address']
    # Region consumption includes linker alignment gaps, not only section sums.
    baseline = {'RAMTEXT': span('.ram_text', '.audio_hot'),
                'RAM': span('.data', '.bss'), 'POOL': byname['.pool']['bytes']}
    sdk = json.loads((ROOT / 'build/bt-prototype/source-only/reference-rf/report.json').read_text(encoding='utf-8'))
    assert sdk['strict_link_passed'] and not sdk['final_undefined'].strip()
    capacity = sdk['region_capacity_bytes']
    origins = {'XIP': 0x02000120, 'RAMTEXT': 0x01C00000,
               'RAM': 0x01C08000, 'POOL': 0x01C20000}
    sdk_spans = {k: max([s['address'] + s['bytes'] - origin
                        for s in sdk['linked_sections'] if s['bytes'] and
                        origin <= s['address'] < origin + capacity[k]] or [0])
                 for k, origin in origins.items()}
    combined = {k: v + sdk_spans[k] for k, v in baseline.items()}
    ref = subprocess.check_output(['git', 'rev-parse', 'origin/main'], text=True).strip()
    production_diff = subprocess.check_output(['git', 'diff', ref, '--', 'firmware', 'tools'], text=True)
    assert not production_diff, 'Production sources differ from the main baseline'
    package = ROOT / 'releases/v1.0.4/emerald-ym-1.0.4.fwsc'
    package_sha = hashlib.sha256(package.read_bytes()).hexdigest()
    assert package_sha == 'f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f'
    report = dict(main_commit=ref, production_version='1.0.4', development_version=(ROOT/'VERSION').read_text().strip(),
                  production_sources_match_main=True, hardware_accessed=False, installable_package_created=False,
                  combined_link_performed=False, production_checks=notes, production_sections=sec,
                  production_app_sha256=hashlib.sha256(image).hexdigest(), preserved_package_sha256=package_sha,
                  baseline_region_bytes=baseline, sdk_reference_region_bytes=sdk_spans,
                  sdk_reference_section_sum_bytes=sdk['region_section_bytes'],
                  conservative_sum_bytes=combined, capacity_bytes=capacity,
                  conservative_overflow_bytes={k: max(0, v-capacity[k]) for k,v in combined.items()},
                  sdk_task_stack_queue_budget_bytes=sdk['task_table_stack_queue_budget_bytes'],
                  sdk_soft_double_symbols=sdk['soft_double_symbols'],
                  sdk_report_sha256=hashlib.sha256((ROOT/'build/bt-prototype/source-only/reference-rf/report.json').read_bytes()).hexdigest())
    (out/'report.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k:report[k] for k in ['main_commit','baseline_region_bytes','sdk_reference_region_bytes','conservative_sum_bytes','conservative_overflow_bytes','production_app_sha256']},indent=2))


if __name__ == '__main__':
    main()
