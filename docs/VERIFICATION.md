# Verification scope and evidence

Desktop tests, flash/update simulations, user feedback and physical FM-1 measurements are separate evidence. The reports in releases/ retain their original values; an offline report intentionally says flashed=false even when a separate hardware report records installation.

## Current 0.3.1 and FX 0.3.0

0.3.1 (FM-1_68000301) swaps the two encoders after the user confirmed FX in 0.3.0: PRESETS = individual level; ALGORITHM = edited effect amount. DSP is identical. Its 30-second continuous physical backing-plus-two/three-live-note test, neutral FX, passed: CPU MAX65%, longest block3820 us, XRUN0, overload0. The user has not yet separately confirmed the swapped physical controls; source/UI tests verify their mapping.

0.3.0: a separate continuous 30-second run with all twelve sources at DECAY25/COLOR100/ECHO100 and live notes passed: CPU MAX68%, longest block3975 us, XRUN0, overload0, failed boots0. A subsequent DECAY0/COLOR100/ECHO100 sustain run included PLAY pauses; its passed=false continuity result remains. Counters stayed at zero and the lifetime maximum reached3984 us. Do not describe that run as continuous.

Desktop tests cover independent sources/effects, deterministic blocks256/63, echo delay/stereo/feedback/tail/reset, no render allocation and signal bounds. Optimized and generic paths produce dry hash7ce44fbe and FX hashf9fae4ef. UI/LED tests, linker/no-soft-double checks and OTA/loader/installer simulations passed. The inherited JavaScript comparison was skipped, not passed. See the release notes and raw evidence under releases/v0.3.0 and releases/v0.3.1.

## Earlier development

| Version | Physical evidence | Important qualification |
|---|---|---|
| 0.2.4 | 30s backing + live, CPU85%, max4985us, XRUN0, overload0 | Very small margin; prototype report also85%/4980us, not evidence of a timing improvement |
| 0.2.3 | 30s continuous + live, CPU84%, max4921us, XRUN0, overload0 | Individual FM/drum buses; user later requested per-instrument selection |
| 0.2.2 | 30s continuous + live, CPU84%, max4918us, XRUN0, overload0 | Default PSG300%; 400% tested on desktop only; user accepted mix/control |
| 0.2.1 | 60s backing-only continuous pass, XRUN0, overload0 | Lifetime84%/4901us includes an earlier interactive live run; interactive report remains failed |
| 0.2.0 | About60.7s interactive, 47/60 window samples playing, CPU84%, max4910us, XRUN0 | Raw passed=true used an older pause-tolerant rule, not a continuous-minute pass; separate timeout run incomplete |

## Accepted baseline 0.1.4

Installed identity FM-1_68000104. The user confirmed music, no flicker, LEDs and knob indicators. Preserved package SHA-256:

```text
5fcae705878056c52588b4af9a98902a556c26488926a98d0cbcd7e6e79bd52a
```

The interactive trace contains62 samples across about60.6s, 48 playing, CPU MAX80%, XRUN0, overload0, failed boots0, longest block4653us. It retains passed=false: REC reset the bar to1, failing the old end-bar criterion. This is not a continuous-minute pass or an observed audio error. The earlier 0.1.3 algorithm passed over a minute continuously (80%,4665us, XRUN0); trace in docs/history/hardware-0.1.3-continuous.json.

The standalone project rebuild reproduced the preserved package byte for byte. New runs go to ignored build/, not over old release evidence. Existing baseline logs must not be edited to manufacture success.

## Remaining public-release checks

Not established: all parameter combinations, multi-hour reliability, physical interrupted-write recovery, installation from official V15 and another custom firmware, and leaving the release for another firmware. Emerald-to-Emerald updates and mock flash tests do not prove these paths. The Firmware Hub checklist is in docs/FIRMWARE-HUB.md. This documentation task does not flash or run new physical tests.
