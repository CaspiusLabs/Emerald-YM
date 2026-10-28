# Source-only audit evidence — 2026-10-28 — 0.8.1

Offline target compilation/linking only. No USB/MIDI/device/radio access, installable package, physical CPU measurement or firmware installation. Production 0.7.0 is unchanged.

| Check | Without RF definition | Explicit SDK reference RF |
|---|---|---|
| Project units `-Wall -Werror` | Pass | Pass |
| Compiled source-only configuration contract | Pass | Pass |
| Relocatable SDK closure | Pass, 189 unresolved names | Pass, 188 unresolved names |
| **Strict link with actual registration tables and GC** | **Fails: wifi_calibration_param** | **Pass, zero undefined symbols** |
| Required linked roots | Not a complete ELF | All 11 present |
| Source codec selection | No complete ELF | SBC only, range verified as 24 B |
| FM-1 hardware/port validation | None | None; SDK demo RF values are not verified for FM-1 |

The unresolved counts in the relocatable closure include functions later removed by final GC; they are **not** the final-link error count. No missing services were replaced with success stubs. Standard SDK services and compiler libraries solve the symbol dependencies; default mode keeps the calibration requirement explicit.

## Final reference memory

| Region | Isolated SDK section bytes | Existing 0.7.0 baseline | Conservative sum | Region capacity |
|---|---:|---:|---:|---:|
| XIP | 218,648 | Not directly additive as a packaged image | Not a combined firmware link | 581,564 |
| RAMTEXT | 10,734 | 24,492 | **35,226** | 24,576 |
| RAM | 68,647 | 39,120 | **107,767** | 98,304 |
| POOL static | 0 | 101,888 | 101,888 | 344,064 |

This budget exceeds RAMTEXT by **10,650 B** and RAM by **9,463 B**. The pool is used as the audit heap range, so its zero static section count does not mean the SDK requires no pool memory. No heap/stack high-water or peak allocation was measured. The task table alone describes 29,440 B of possible stack/queue allocations, excluding TCBs; the virtual encoder requests 2,048 B output storage, with further codec/server/crypto overhead. SDK user stacks are explicitly in RAM despite their input section flags.

This is not a minimum A2DP cost or a final combined layout: SDK startup/runtime services duplicate or conflict with Emerald's bare-metal platform. It is also not an executable-layout or flash-off/XIP guard pass for production. The reference still contains soft-double helpers; their names are recorded in the summary. Limits and guards were not relaxed. The full reference ELF was inspected and removed rather than packaged.

The current configured relocatable closure is 60,406 B writable / 555,480 B executable, compared with the older unconfigured 0.8.0 runtime closure's 74,779 B writable / 832,300 B executable. Those closure counts help track reduction, but the **final reference section table above** is the useful layout result. The different roots/runtime choices and pruning mean this is not a controlled measurement of a single optimization.

## Artifacts

- [Default summary](default-summary.json), [full default report, unedited gzip](default-report.json.gz), [default strict-link diagnostics, gzip](default-strict-link.txt.gz).
- [Reference summary](reference-summary.json), [full reference report, unedited gzip](reference-report.json.gz), [reference strict-link diagnostics, gzip](reference-strict-link.txt.gz), [retained reference symbols, gzip](reference-symbols.txt.gz).
- [Desktop queue test output](desktop-tests.txt): synthetic PCM/byte-frame tests pass; no actual SBC encoding/playback was executed.

Summaries are derived by omitting long section lists from the raw reports. Each includes the SHA-256 of its uncompressed full report. Reports identify the SDK commit, all library/source hashes, retained roots, SBC range verification, remaining weak-negotiation integration gap, and reference-only scope. Original 0.8.0 evidence remains unchanged under ../../evidence/.

Do not read strict SDK link success as working headphones: the negotiation/init/close adapter is still not connected. SDK weak callbacks can disappear into LTO; the report explicitly records `negotiation_adapter_implemented=false`. Unselected JLA descriptor bytes remain in a shared input section outside the SBC-only selection range; no JLA encoder library is linked.

The unchanged 0.7.0 package remains 609,666 B, SHA-256 `b575995cb7c829acc97a3672579212767346194811c18a66323621b95d7b64b6`. No source under firmware/, production build tools, release bytes or tags were changed in this follow-up.
