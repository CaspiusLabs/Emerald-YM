# Emerald YM 0.6.0 — beta

Generative 16-bit music firmware for M-VAVE FM-1, by **Caspius Labs**.

- Console-style main screen with a red volume slider following the physical MASTER knob.
- YM2612 FM, Sega-style PSG, DAC percussion, live keyboard and generative backing.
- Stereo 16-bit / 44.1 kHz USB master-mix recording alongside MIDI and read-only diagnostics.
- Independent levels/decay/color/echo for twelve sources, with saved mixer/FX and K1-K4 settings.

Download **emerald-ym-0.6.0.fwsc**, not a source archive, and check SHA256SUMS. This is the **exact physically tested file**, published without rebuilding. Tag `v0.6.0` points to matching firmware source commit `c0530a6cd7d026554e21b957665edfd7fe7d51d9`; later documentation versions do not change it.

SHA-256: `ea7a5bff5fcf2671debad1ee32f13b50843d6f51d7fe879f14798097a56fbe89`

[Installation](https://github.com/CaspiusLabs/Emerald-YM/blob/main/docs/INSTALLING.md) · [Manual](https://github.com/CaspiusLabs/Emerald-YM/blob/main/docs/MANUAL.md) · [Exact-package test reports](https://github.com/CaspiusLabs/Emerald-YM/blob/main/docs/verification/master-ui-0.6.0/README.md)

Physical 0.6.0 check: 30 seconds continuous backing with live MIDI notes and diagnostics while recording 32.34 seconds of USB audio. Average sampled audio CPU 46.93%, maximum 72%, longest block 4196 us; XRUN 0 / overload 0 / failed boots 0. The owner confirmed correct screen and MASTER-slider behavior. Windows recording name: Emerald YM FM-1 (Emerald YM FM-1); an existing cached Felucca name may require the documented targeted Windows refresh.

This short test is not a multi-hour or packet-level guarantee. Direct V15/Dinghy round trips were verified for older exact 0.4.3, not 0.6.0; no interrupted-write/power-loss/Hub-browser result is inherited. The package includes the Felucca update loader; identity `FM-1_68000600`, MIDI name `Emerald YM FM-1`.

Emerald does not import or guarantee preservation of other firmware's presets/projects/samples. Back up with your existing firmware's tools before switching. Persistent settings reuse project sectors FC000/FD000. USB audio captures the mixed instrument output; no host playback or individual tracks. Combined license GPL-3.0-only, with component notices in source. Locally built; no GitHub build attestation. No game music, extracted game samples or Sega logos.
