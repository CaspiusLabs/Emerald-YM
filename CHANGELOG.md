# Emerald YM changelog

## Repository preparation — 2026-10-27 (firmware remains 0.3.1)

- Adopt CaspiusLabs/Emerald-YM as the canonical GitHub repository; retain full upstream history and immutable v0.1.4.
- English README, current user/install/build instructions, developer notes, release summaries and historical summaries.
- Move the inherited DUB-1 manual into the historical docs; do not present legacy web tools as Emerald features.
- Add fmhub.json, a 240×240 UI render link and an honest Firmware Hub readiness checklist. Repository remains private by owner request; no submission sent.
- Preserve the installed/tested 0.3.1 binary unchanged. No device operations during this documentation task.

## 0.3.1 — 2026-10-26 — encoder swap

- After user FX acceptance: PRESETS controls individual volume, ALGORITHM the edited FX amount. SELECT and FX unchanged.
- PRE / ALG tile labels updated. DSP/MIDI unchanged. Physical neutral-FX backing/live run:65%, XRUN 0, overload 0.

## 0.3.0 — 2026-10-26 — three simultaneous effects

- Independent DECAY/COLOR/ECHO for 12 instruments, initially 0; FX chooses the edited parameter without disabling others.
- Per-source sends to shared stereo echo (~186 ms), bounded feedback; FX LED/tile, standard CC20–22 on channels 1–12.
- EMERALD_STATIC_OPN skips unused SSG-EG/AM/PM. Generic comparison matches dry/FX hashes.
- 32 KiB pool buffers; critical FX code remains in the original guarded RAM range, without raising memory limits or the 85% threshold.
- 30 s continuous all-source FX25/100/100 + live:68%, XRUN 0, overload 0. Sustain run contains pauses, retains passed=false.
- User confirmed effects/independent values and requested the encoder swap in 0.3.1.

## 0.2.4 — 2026-10-26 — individual levels

- SELECT chooses one of 12 sources; ALGORITHM sets its independent 0–100% level. PRESETS still controls the percussion bus in this version.
- New instrument tile, isolated-source/live-mute tests; default audio matches 0.2.2 hash 7ce44fbe.
- PSG tuning table moves to setup-time XIP, hot oscillator state stays in RAM.
- Physical 30 s continuous + live:85%, max 4985 us, XRUN 0, overload 0. Very little margin. User confirmed independent controls.

## 0.2.3 — 2026-10-26 — FM/PSG/drum mixer

- SELECT = melodic FM0–100%; PRESETS = DAC drums/PSG noise 0–100%; ALGORITHM retains PSG mix.
- Three percentage/bar tiles replace CPU/XRUN and the hint. USB counters and 85% guard remain.
- FM scaling before DAC preserves unity audio; main-loop leaf controls move to XIP .ym_control, without enlarging RAM regions.
- Physical 30 s continuous + live:84%, XRUN 0, overload 0. User requested individual-source control next.

## 0.2.2 — 2026-10-26 — adjustable PSG mix

- ALGORITHM sets PSG 0–400%, default 300% (+9.54dB over 0.2.1); applies to arp/live/noise, not FM/DAC.
- On-screen value, changed-tile updates and volatile settings. Desktop 400% /three-live-note test without limiter hits.
- Physical 30 s at default 300% + live:84%, XRUN 0, overload 0. User accepted level and control.

## 0.2.1 — 2026-10-26 — musical arpeggio

- PSG arp +6 dB, one octave lower, longer gates, repeating chord tones 0–1–2–1.
- Interactive stress run retains passed=false after PLAY/REC. Separate 60 s backing-only pass, XRUN 0/overload 0; lifetime 84% includes earlier live notes.
- Test tool preserves partial transport-error reports and can require continuity. User found it more musical but still too quiet.

## 0.2.0 — 2026-10-26 — PSG and live keys

- Original Sega-style PSG: NTSC 3579545 Hz, three 10-bit tone dividers,2 dB attenuation,16-bit LFSR, white noise clock/512.
- Generative arp/noise and up to three live F3–G5 notes from panel/USB, including pause. Latest-note priority/return. ARP toggles all PSG.
- Dim backing/bright live LEDs. Five FM+DAC,256-frame blocks and 85% guard retained.
- Final interactive run:84%, XRUN 0,47/60 samples playing. Separate continuous run ended in USB timeout, not passed. Prototype 85% is separate evidence.

## 0.1.4 — 2026-10-26 — accepted baseline

- Real YM2612 emulation, five FM voices + DAC drums and generative sequencer.
- RAM/operator/output-channel/repeated-DAC-write optimizations retain NTSC clock and 85% guard.
- Changed-tile UI, four parameter cards, step strip, original Emerald mark/16-BIT badge, PLAY/REC/SEQ LEDs.
- USB diagnostics and MIDI Start/Continue/Stop. User accepted sound, no flicker, LEDs and controls.
- Interactive CPU 80%, XRUN 0, overload 0; raw continuity result remains false due to REC. Earlier 0.1.3 continuous test also 80% /XRUN 0.
- Standalone runtime/build/tests/docs, preserved package and real tag added without changing the firmware.

## Earlier experiments — 2026-10-26

- 0.1.0: first hardware boot; reported 92% /XRUN 0, protective stop.
- 0.1.1: code/tables moved to RAM; user reported that this alone did not solve overload.
- 0.1.2: five-FM+DAC optimization and USB status;87% /XRUN 0, protective stop.
- 0.1.3: inline hot clock functions; successful continuous test and listening confirmation.

These early stages predate the separate Emerald repository and are not fabricated as separate commits. Historical summaries and available traces remain in docs/history. The first actual Emerald commit imports the complete working 0.1.4 on the preserved DUB-1 history.
