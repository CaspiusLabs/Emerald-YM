# Emerald YM — development instructions

## Project identity and authoritative status

- Creator: **Caspius Labs**. Canonical repository: https://github.com/CaspiusLabs/Emerald-YM (public). Speak Polish with the owner; write code comments, commits, PR descriptions and repository documentation in English.
- Target hardware: **M-VAVE FM-1**, JieLi AC79. This is original YM2612/PSG generative firmware built on the DUB-1/Felucca platform; not a general Mega Drive or VGM emulator.
- **Stable, released and documented as physically tested: v1.0.4**. Source/tag `7bb4efdd02499664296e96f134371c68403b22de`, exact `.fwsc` SHA-256 `f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`, hardware identity `FM-1_68010004`. Do not replace or modify published releases or tags.
- **main at the start of this task: VERSION 1.0.6**, which contains K1–K4 *documentation only*, not a different installed firmware. **Documentation VERSION 1.0.8 records the physically tested song-variation firmware 1.0.7**. VERSION may be ahead of the last installed and released binary. Verify all current refs instead of copying status statements from archived history.
- At the start of this task, PRs #1–#8 and #10–#20 were merged; **PR #9 Bluetooth Audio was closed without merge on 2026-11-03**. The unmerged 1.0.12 Bluetooth experiments are **not** stable firmware. Never infer current branch/PR status from a stale note.
- Source-of-truth reading order: `VERSION`, `README.md`, `CHANGELOG.md`, `docs/VERIFICATION.md`, `docs/ARCHITECTURE.md`, `BUILDING.md`, `docs/INSTALLING.md`, `docs/FIRMWARE-HUB.md`, then current GitHub releases/PRs. Historical reports under `docs/history/`, `docs/verification/` and `releases/` are version-specific.

## Required workflow

1. Use a separate topic branch for **every change**, including docs; update `VERSION` and append `CHANGELOG.md` (patch by default). Never silently re-label earlier firmware/package evidence.
2. Commit/push to `origin`, open a PR targeting `main`; **do not push straight to main, auto-merge, merge, publish a release, or delete branches without explicit owner instruction**. `upstream` is read-only reference material.
3. Preserve stable v1.0.4 and all previous firmware binaries, release assets, checksums and tags. A documentation VERSION bump does not change firmware; a source change is **not** a tested or installable release until it is built, checked, physically validated as appropriate, and explicitly approved.
4. No automatic USB/MIDI device access, firmware installation, hardware probing, power-cycling or Firmware Hub submission during ordinary code/build/test tasks. Physical tests and flash require explicit scoped owner authorization. Identify only the intended FM-1 USB pair; **do not select Roland UM-ONE** by substring. Never invent SysEx commands.
5. Report build/test/flash status honestly, including unavailable dependencies or unexecuted tests. Do not claim successful hardware testing based on desktop simulations.

## Hardware, build and safety invariants

- Supported local build: Windows / JieLi AC79 toolchain, provisioned under ignored `.runtime/`, with scripts `build-ym-windows.ps1` and `test-ym-windows.ps1`. Read `BUILDING.md` for SDK/tool hashes. An ordinary clone does not include compiler/SDK inputs, and GitHub-hosted attested builds are not verified.
- Real-time audio: **44,100 Hz, 256-frame blocks (~5804 us)**; preserve the **85% overload guard**. Do not remove it, alter the firmware memory map, invoke XIP from flash-off `.ram_text`, or change startup copy/PC guards to make a feature fit.
- Stable v1.0.4 RAMTEXT utilization: **24,508 / 24,576 bytes (only 68 bytes spare)**. Check complete link map and section sizes before attempting synthesis, Bluetooth, new ISR, or memory-heavy features. The closed/unmerged Bluetooth SDK prototype exceeded RAMTEXT by 9,455 bytes in its last combined link; never advertise it as working.
- Active app: `firmware/src/ym/`, selected by `EN_YM2612`. Do not confuse inherited DUB-1 `app/`, `dsp/`, `web/` or `host/` with a supported Emerald application, browser installer or player.
- Audio rendering: no heap allocation, full-song buffers, unbounded loops, blocking I/O, or new float/libm/double calls in sample loops. Control helpers in XIP `.ym_control` are reached through `FL_FAR` outside the renderer. Keep audio-critical code in guarded RAM.
- YM2612 profile uses **five FM voices and DAC on channel 6**, restricted `EMERALD_FIXED_DAC`/`EMERALD_STATIC_OPN`; PSG has three tone voices and white noise. Keep optimized/generic comparison and PCM determinism when changing DSP.
- Levels and DECAY/COLOR/ECHO persist in A/B sectors `0xFC000`/`0xFD000`, with K1–K4 schema-2 settings and CRC. Saved values do **not** change in song arrangement profiles. SAVE must pause appropriately, and ordinary flash writes must never occur in the renderer.
- Screen is **240×240**, built from changed-only DMA-safe tiles using an existing 240×144 workspace; no full-frame redraws or new framebuffer. Preserve LEDs: assigned available controls dim when inactive, bright when active, including GLO/USB ONLY.
- USB audio is stereo PCM16/44.1 kHz; USB MIDI supports IN/OUT and Clock depending on EDIT mode; the physical TRS minijack remains MIDI IN. No MIDI Thru or Bluetooth audio in the stable image.

## Song form and future variations

- Stable song: **88 bars**: INTRO 8 → VERSE 16 → CHORUS 16 → BREAK 8 → VERSE 16 → CHORUS 16 → OUTRO 8 → END. HOME bright is song mode; HOME dim uses deterministic endless generation. REC chooses a new seed. Preserve restart/END behavior and automatic USB MIDI Stop/note cleanup.
- `firmware/src/ym/ym_song.h` calculates a control-rate arrangement profile, and `firmware/src/ym/ym_prelude.c` publishes its stem mask/relative density/levels. The engine remains in `ym_engine.cpp`. Reuse this mechanism for phrasing; do not add per-sample conditional DSP or a song-long pattern buffer.
- Candidate **1.0.7** makes verse 1 gradually add lead/arp, verse 2 denser with one-bar lead responses, chorus 2 breathe through short dropouts, and the break build up. Keep source 10 (live keys) exempt from the form's mask/gain automation; save no transient variation values.
- Song decisions must be deterministic for the same seed and inputs, have bounded state/work, preserve 88-bar boundaries and the original endless path, and avoid unexpected external MIDI note leaks on masked stems. Add regression assertions in `tests/ym_song_test.cpp`, plus a full actual-engine MIDI/audio transport run before claiming completion.
- Tests must distinguish deterministic changes from true additional audio variety: layer masks/density are one modest first step, not a new harmony, patch library, or genre engine.

## Known gaps and publication

- **Open reliability question:** an early v1.0.4 trace showed an unexplained USB MIDI Stop after ~2.52 seconds. The subsequent 162.40-second full-song test and 321.78-second K1–K4 test did not reproduce it; the original failure is **not resolved**. Preserve the failed evidence under `docs/verification/stable-1.0.4/`.
- Stable v1.0.4 full-song physical report: 407/407 status replies, 47.59% average music CPU / 72% maximum, zero XRUN/overloads in that test. Separate K1–K4 report: 806/806 valid replies and zero unexpected Stop within its observed windows. These finite tests do not prove long-term reliability, USB packet integrity or external synth timing.
- Historical Emerald 0.4.3 ↔ official V15 / Dinghy 0.3 transitions passed, but those exact-package results are **not** proof of 1.0.4 recovery compatibility. Interrupted changed-sector flash, power loss, Firmware Hub browser install, live Hub identity uniqueness and independent GitHub build provenance remain unverified.
- Repository is public with stable release assets, but **Firmware Hub submission was not sent** according to `docs/FIRMWARE-HUB.md`. Do not submit or claim listing without authorization and current confirmation.
- Record every candidate's tests precisely: desktop build/tests, size guards, actual FM-1 flash and music/MIDI/USB measurements are separate. State which ones were executed and retain original failures and hashes.
