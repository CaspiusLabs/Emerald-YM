# USB audio 0.5.0 — desktop evidence, 2026-10-27

No device was flashed and no physical MIDI port was opened. Validated firmware/release 0.4.3 is unchanged. Candidate package: `build/emerald-ym-0.5.0.fwsc`, identity `FM-1_68000500`; exact digest in [package verification](package-verification.json).

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

The projection does not execute USB control transfers, ISR concurrency or hardware enumeration. Windows recognition, actual USB recording, simultaneous MIDI/status under recording, USB glitches and real-time CPU headroom remain unverified. No CPU percentage from 0.4.3 is transferred to this candidate. Status v1 still reports renderer CPU/XRUNs and does not report separate UAC packet counters. Later hardware testing requires a new authorized task.

[Raw desktop test log](desktop-tests.log). The inherited optional JavaScript installer comparison was skipped (missing node/build fixture); it is not counted as passed. No hardware-test tool was executed.
