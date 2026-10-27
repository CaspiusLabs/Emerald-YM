# USB audio 0.5.0 — desktop evidence, 2026-10-27

During the initial desktop-only stage, no device was flashed and no physical MIDI port was opened. The subsequent authorized physical test is recorded separately below. Validated firmware/release 0.4.3 is unchanged. Candidate package: `build/emerald-ym-0.5.0.fwsc`, identity `FM-1_68000500`; exact digest in [package verification](package-verification.json).

Build and `test-ym-windows.ps1` passed, including no-soft-double and flash-off checks, deterministic renderer/PSG/FX, persistence, UI/LED, OTA/loader simulations and mock installer tests.

| Region | 0.5.0 used | Available |
|---|---:|---:|
| Application image | 107060 B | checked by build |
| RAM data + BSS | 39120 B | 98304 B |
| Pool | 101888 B | 344064 B |
| Audio hot code/tables | 21604 B | inside RAMTEXT |
| Total RAMTEXT | 24492 B | 24576 B |

Relative to accepted 0.4.3: application +1700 B, data/BSS +8704 B; pool and RAMTEXT unchanged. RAMTEXT retains only 84 B of margin; memory regions and the 85% protection threshold were not enlarged.

The UAC test compiles production DSP and descriptor sections, projected by tools/prepare-uac-desktop-test.py without register-level HAL. It checks coexistence of MIDI IN/OUT and stereo 16-bit/44.1 kHz audio-streaming interfaces, including presence of the composite descriptor in the actual built application image, and runs the existing 40-second drift simulation at 44100, 44145 and 44300 input Hz. Output frequency was 999.975 Hz, residual below -82 dB, zero short packets after 10 seconds, low ring fill 197–199 against target 200. Simulation includes double-precision signal analysis on the desktop; firmware build checks separately exclude soft-double routines.

The projection does not execute USB control transfers, ISR concurrency or hardware enumeration. At that initial stage, Windows recognition, actual USB recording, simultaneous MIDI/status under recording and hardware CPU headroom were unverified; the following physical run supplies limited evidence for exact 0.5.0. USB packet-level glitches and long-session reliability remain unverified. No CPU percentage from 0.4.3 is transferred to this candidate. Status v1 still reports renderer CPU/XRUNs and does not report separate UAC packet counters. Later hardware testing requires a new authorized task.

[Raw desktop test log](desktop-tests.log). The inherited optional JavaScript installer comparison was skipped (missing node/build fixture); it is not counted as passed. No hardware-test tool was executed.

## Subsequent physical 0.5.0 test — 2026-10-27

The owner authorized installation and recording with MIDI and diagnostics. Exact package SHA-256: `622295cc5c9cccc440de5744ee972011d0bbeee783c24ea0b2bca95e92c5d1a6`, source commit `e216026b61aace77960fb2ebaf72566afa367e99`; running identity `FM-1_68000500`.

Windows exposed `Line (Felucca)`, associated with `USB\VID_1209&PID_0001&MI_00`; MIDI remained `Emerald YM FM-1`. Only this verified audio input and Emerald MIDI pair were opened. No microphones or Roland UM-ONE were opened. The archive contains the [raw MIDI/status report](hardware/midi-status.json), [capture analysis](hardware/capture-report.json), [summary](hardware/summary.json), [status log](hardware/midi-status.log) and [executed scratch recorder](hardware/executed-recorder.py.txt). The recorder uses WinMM PCM capture plus the documented diagnostic tool; the archived script is evidence, not a portable recovery/install tool.

A 32.33-second stereo PCM16/44100 recording overlapped 30 seconds of continuous backing, alternating two/three standard MIDI notes and status polling each second. Pass: average sampled CPU 46.93%, maximum 72%, longest block 4200 us, XRUNS 0, overload 0, failed boots 0. Test cleanup stopped playback. No effects CCs or persistent edits were sent.

After excluding the first 4 and last 3 seconds, peak was 3959, RMS 365.24, clipped samples 0, longest identical-frame run 0.272 ms and longest zero run 0.204 ms. These limited checks do not prove absence of single-packet glitches. Status v1 does not expose internal UAC counters. The local WAV is `build/usb-audio-hardware-0.5.0/usb-mix-live-midi.wav` (SHA-256 `19d32ec62c434ba74856daae8c6cbf7a502bd072c7d8b1c4690ec6479b4aac07`); it is not committed.

These physical results apply only to exact 0.5.0. The 0.5.1 audio-name follow-up is separately desktop-tested and has not been flashed; do not transfer the 0.5.0 CPU/recording result to that image.
