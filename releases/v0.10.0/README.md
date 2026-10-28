# Emerald YM 0.10.0 (beta)

Generative YM2612 + PSG firmware for M-VAVE FM-1, by **Caspius Labs**.

[Download firmware](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v0.10.0/emerald-ym-0.10.0.fwsc) · [SHA256SUMS](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v0.10.0/SHA256SUMS) · [Matching source](https://github.com/CaspiusLabs/Emerald-YM/tree/v0.10.0) · [Hardware evidence](../../docs/verification/usb-midi-out-0.10.0/hardware/README.md)

This is the exact installed and tested package, copied without rebuilding:

- File: `emerald-ym-0.10.0.fwsc` — 609,677 bytes.
- SHA-256: `195cd2d1a504d5936e79cde227d64d2d8d1bd407945e8156dac795f682d59401`.
- Identity: `FM-1_68001000`.
- Source/tag `v0.10.0`: `e906fbc5eb3c91e45c95101c46bd0d674eef533d`.
- Documentation version 0.10.1 records publication and physical results; it is not replacement firmware. PR #11 remains open for owner review.

## MIDI IN/OUT

EDIT dim selects note/effect input over USB or the minijack. EDIT bright selects USB note output from the physical keyboard and generative sequencer; the header shows USB MIDI OUT. Local sound continues. Incoming note/effect messages are ignored in OUT to prevent echo; USB Clock/transport, status and stereo USB audio stay available. The mode resets to IN at boot and is not saved.

Output channels: keyboard 1; FM bass/lead 2–3; FM chords 4–6; PSG arp/noise 7–8; drums 10 (36/38/42/46). The minijack remains input: physical output requires routing FM-1 USB MIDI through a computer/compatible USB host to an external MIDI interface. No Bluetooth output or outgoing Clock/transport is included. GLO / USB ONLY continues to mute physical audio while retaining USB capture.

## Validation

Exact 0.10.0 passed a 60-second 180 BPM run with simultaneous MIDI OUT capture, USB audio and status: sampled CPU average 45.98%, maximum 72%, longest block 4,212 us; XRUN/overload/failed boots 0/0/0. All nine assigned MIDI channels produced notes, Stop cleared captured notes, and separate tests established keyboard chords, EDIT-held-note cleanup and input reception before/after switching. The 81.41 s stereo PCM16/44.1 kHz recording had no clipping in its analyzed body. The owner confirmed smooth backing and EDIT LED/header behavior.

Desktop build, deterministic audio/generic emulator comparison, MIDI, native UI/LED, persistence and update simulations passed separately. RAMTEXT is 24,488/24,576 B (88 B spare); the original memory and 85% protective limits remain unchanged. Short physical results do not establish long-session reliability, target-synth rendering or new installation/recovery paths. TRS input was not retested on exact 0.10.0. [Installation instructions](../../docs/INSTALLING.md) and [detailed controls](../../docs/MANUAL.md).

![Native 0.10.0 UI render in USB MIDI OUT mode; not a device photograph](ui-preview.png)
