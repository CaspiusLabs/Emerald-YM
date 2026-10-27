# Verification scope and evidence

Desktop tests, flash/update simulations, user feedback and physical FM-1 measurements are separate evidence. The reports in releases/ retain their original values; an offline report intentionally says flashed=false even when a separate hardware report records installation.

## Physical installation round trips — accepted 0.4.3

PRs #1–#3 were verified merged into main, commit b5848cd40b4d8dda4612cb9b8c3dc9687e29abc6. On 2026-10-27 the exact accepted 0.4.3 package (SHA-256 6669787e1d7a3b56f37bd0494c992fbdba5cb59e5dc13d5f2380d7c4fbd64925) passed Emerald → official V15 → Emerald → Dinghy 0.3 → Emerald through normal USB MIDI updates. All target identities matched. No manual power cycle, OCT shortcut or ROM rescue was required; Roland UM-ONE was never opened.

The owner confirmed V15 and Dinghy screens and audible keys. Each Emerald return passed 20 seconds of continuous playback/status monitoring: CPU MAX 67%, XRUNS 0, overload 0, failed boots 0; longest blocks 3929 us after V15 and 3922 us after Dinghy. Firmware 0.4.3 is restored. [Manifest, raw reports, runner and scope](verification/install-roundtrips-2026-10-27/README.md).

Source/documentation version 0.4.4 records this work without building/flashing a new firmware binary. Earlier same-version review packages and old release 0.4.0 do not inherit these exact-package results. Interrupted-write recovery, browser Hub Connect, arbitrary firmware/data compatibility and long-session behavior remain separate checks.

## Accepted 0.4.3 — uppercase header follow-up

The user confirmed K1–K4 retention and prior mixer/FX settings on 0.4.2, but requested the earlier large font in uppercase and a second badge stripe. The 0.4.3 renderer preview has been inspected: EMERALD YM fits beside the 48×21 badge, with two stripes and a version gap. Build and full desktop tests passed. Storage/audio DSP are unchanged. Prior 0.4.2 hardware measurements are referenced as prior evidence, not relabeled as 0.4.3 results.

## Earlier 0.4.2 — header and K1–K4 persistence

Desktop build, streaming/FX/settings/UI/LED tests and update/loader/installer simulations passed. Settings tests restore all four knobs and the complete mixer, reject invalid tempo/density/mutation/transpose, exercise valid extrema including signed transpose, migrate schema 1 without a boot-time write and commit the migrated snapshot on the next edit. The actual UI renderer confirms the full title fits before the smaller badge; its preview is in releases/v0.4.2/ui-preview.png.

The DSP and guarded RAM regions are unchanged. This was a PR candidate at the time of the original evidence; PRs #1–#3 have since been merged by the owner. New physical audio results and power-cycle feedback must be recorded independently; prior 0.4.0 confirmation does not prove K1–K4 retention in 0.4.2.


Physical FM-1_68000402: 30 seconds continuous backing with alternating two/three live notes, passed=true; CPU MAX 67%, longest block 3945 us, XRUN 0, overload 0, failed boots 0. The test preserved loaded settings and sent no effect CCs, so it is not labeled a neutral-FX test. The user subsequently confirmed K1–K4 retention and preservation of previous mixer/FX settings after the requested complete power-cycle test. They requested a larger uppercase title and second badge stripe as a follow-up (0.4.3). The original summary preserves the pending state at the first candidate commit; dated feedback is in releases/v0.4.2/user-confirmation.json.

## Accepted 0.4.0 — persistent settings and startup screen

Installed as FM-1_68000400. A 30-second continuous physical backing + two/three live-note test with neutral FX passed: CPU MAX 65%, longest block 3827 us, XRUN 0, overload 0, failed boots 0. It did not alter persistent levels/FX. DSP is unchanged from 0.3.1; the 85% guard remains.

Desktop tests with production storage.c passed complete restoration of all twelve levels/36 FX values, interrupted payload/header saves, newest-copy CRC fallback, schema/range/length/foreign-record rejection, no repeated writes when clean, explicit failure retry, three-second inactivity/debounce, and timer wrap. Every simulated write stayed within FC000/FD000. These are simulated flash/reboot tests, **not** a physical power-cycle confirmation.

On 2026-10-27 the user replied “działa” (works) to the requested SAVE / SAVED / complete power-off / reboot / reselect check, confirming retained level/effect values, the version label and startup screen. This is a representative physical user check, not an exhaustive physical retention test of all 48 values. The dated evidence is releases/v0.4.0/user-confirmation.json. The original verification-summary.json and release tag retain the pending state recorded at release creation. No V15/other-firmware transition or interrupted physical write was tested. Package, offline logs, desktop UI renders and physical audio evidence are separate in releases/v0.4.0.

## Earlier 0.3.1 and FX 0.3.0

0.3.1 (FM-1_68000301) swaps the two encoders after the user confirmed FX in 0.3.0: PRESETS = individual level; ALGORITHM = edited effect amount. DSP is identical. Its 30-second continuous physical backing-plus-two/three-live-note test, neutral FX, passed: CPU MAX 65%, longest block3820 us, XRUN 0, overload 0. The owner subsequently reported the version working before requesting persistent settings in 0.4.0; source/UI tests also verify the encoder mapping.

0.3.0: a separate continuous 30-second run with all twelve sources at DECAY25/COLOR100/ECHO100 and live notes passed: CPU MAX 68%, longest block3975 us, XRUN 0, overload 0, failed boots 0. A subsequent DECAY0/COLOR100/ECHO100 sustain run included PLAY pauses; its passed=false continuity result remains. Counters stayed at zero and the lifetime maximum reached3984 us. Do not describe that run as continuous.

Desktop tests cover independent sources/effects, deterministic blocks256/63, echo delay/stereo/feedback/tail/reset, no render allocation and signal bounds. Optimized and generic paths produce dry hash7ce44fbe and FX hashf9fae4ef. UI/LED tests, linker/no-soft-double checks and OTA/loader/installer simulations passed. The inherited JavaScript comparison was skipped, not passed. See the release notes and raw evidence under releases/v0.3.0 and releases/v0.3.1.

## Earlier development

| Version | Physical evidence | Important qualification |
|---|---|---|
| 0.2.4 | 30s backing + live, CPU85%, max4985us, XRUN 0, overload 0 | Very small margin; prototype report also85%/4980us, not evidence of a timing improvement |
| 0.2.3 | 30s continuous + live, CPU84%, max4921us, XRUN 0, overload 0 | Individual FM/drum buses; user later requested per-instrument selection |
| 0.2.2 | 30s continuous + live, CPU84%, max4918us, XRUN 0, overload 0 | Default PSG300%; 400% tested on desktop only; user accepted mix/control |
| 0.2.1 | 60s backing-only continuous pass, XRUN 0, overload 0 | Lifetime84%/4901us includes an earlier interactive live run; interactive report remains failed |
| 0.2.0 | About60.7s interactive, 47/60 window samples playing, CPU84%, max4910us, XRUN 0 | Raw passed=true used an older pause-tolerant rule, not a continuous-minute pass; separate timeout run incomplete |

## Accepted baseline 0.1.4

Installed identity FM-1_68000104. The user confirmed music, no flicker, LEDs and knob indicators. Preserved package SHA-256:

```text
5fcae705878056c52588b4af9a98902a556c26488926a98d0cbcd7e6e79bd52a
```

The interactive trace contains62 samples across about60.6s, 48 playing, CPU MAX80%, XRUN 0, overload 0, failed boots 0, longest block4653us. It retains passed=false: REC reset the bar to1, failing the old end-bar criterion. This is not a continuous-minute pass or an observed audio error. The earlier 0.1.3 algorithm passed over a minute continuously (80%,4665us, XRUN 0); trace in docs/history/hardware-0.1.3-continuous.json.

The standalone project rebuild reproduced the preserved package byte for byte. New runs go to ignored build/, not over old release evidence. Existing baseline logs must not be edited to manufacture success.

## Remaining public-release checks

Still not established: all parameter combinations, multi-hour reliability, physical interrupted-write recovery and the Hub browser Connect/Install route. The ordinary V15/Dinghy transitions are now physically verified for the pinned 0.4.3 package, not for every release. The repository is private and the tested 0.4.3 bytes still need a corresponding GitHub release/tag for the preferred Hub workflow. See docs/FIRMWARE-HUB.md.
