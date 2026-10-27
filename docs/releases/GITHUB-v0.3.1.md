# Emerald YM 0.3.1 — beta

Generative 16-bit music for M-VAVE FM-1: five YM2612 FM voices, DAC percussion and Sega-style PSG, with up to three live keyboard notes over an evolving backing.

Each of twelve instruments has its own level and independent DECAY, COLOR and ECHO amounts. **SELECT** chooses the instrument, **PRESETS** adjusts its volume, **FX** chooses the effect and **ALGORITHM** sets its amount. All three effects can run together. Settings reset at power-off.

## Install

Download `emerald-ym-0.3.1.fwsc` and verify it against `SHA256SUMS`. The package includes the Felucca update loader. Identity: `FM-1_68000301`; USB MIDI: `Emerald YM FM-1`.

Read [installation and data compatibility](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.3.1/docs/INSTALLING.md) and the [manual](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.3.1/docs/MANUAL.md). Emerald does not import other firmware's projects/presets/samples or provide their backup/restore. Back up before switching. Audio is through the device output, not USB audio.

## Verification and remaining checks

- Desktop audio/FX/UI/LED checks and update/loader/installer simulations passed.
- Physical 0.3.1: 30 seconds continuous backing plus live notes, neutral FX, CPU MAX 65%, XRUNS 0, overload blocks 0.
- The identical DSP in 0.3.0 passed an all-source FX scene at 68%, XRUNS 0, overload 0. A separate paused sustain run retains its failed continuity result.
- Installation from official V15 and another custom firmware, leaving Emerald for another firmware, interrupted-write recovery, all settings and multi-hour operation are **not yet verified for this release**. It is not yet ready for a Firmware Hub listing. See the [checklist](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.3.1/docs/FIRMWARE-HUB.md).

This is the original tested binary from 2026-10-26; the publication commit adds English documentation without changing firmware. It was built locally with the documented Windows toolchain, not by GitHub Actions; no build attestation is claimed. Full source is browsable at this tag; upstream history and third-party notices are preserved.

Combined firmware: GPL-3.0-only. Component licenses and attribution: [LICENSING.md](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.3.1/LICENSING.md). Original music; no Sonic game assets. Independent of M-VAVE, Sega and Yamaha.
