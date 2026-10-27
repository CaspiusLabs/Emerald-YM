# Emerald YM 0.4.0 — beta

Persistent instrument settings and a new startup screen for M-VAVE FM-1.

- All twelve instrument levels and 36 DECAY/COLOR/ECHO amounts can survive power-off.
- Changes save after three seconds without edits, backing playback or held live keys. **SAVE** pauses backing and requests a quiet save; wait for **SAVED** before switching off. Pending/saving/error states and the SAVE LED show progress.
- Version number below the 16-BIT badge, taken from VERSION.
- Original console-inspired boot screen: circular cartridge deck, vents and red power LED, drawn using the existing tile canvas.

Controls remain SELECT = instrument, PRESETS = level, FX = edited effect, ALGORITHM = amount. Tempo/seed/transpose and other performance settings are not saved. The audio DSP and 85% guard are unchanged.

## Installation and data

Package: `emerald-ym-0.4.0.fwsc`, with the retained Felucca update loader. Identity `FM-1_68000400`; MIDI port `Emerald YM FM-1`. Check SHA256SUMS before installing. Read the [manual](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.4.0/docs/MANUAL.md) and [installation guide](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.4.0/docs/INSTALLING.md).

Saving uses the existing project sectors at FC000/FD000 and may replace another firmware's data there. Emerald does not import or back up other firmware's projects/presets/samples. Back up before switching. Recent edits may be lost if power is removed before SAVED. There is no guaranteed automatic rollback.

## Verification

Desktop tests cover the production storage codec and A/B flash implementation, reboot restoration of every value, interrupted payload/header writes, CRC fallback, invalid/foreign/oversized records, debounce/error retry and timer wrap; audio, UI/LED and update simulations also passed. Physical audio measurements and the separate power-cycle retention check are recorded in the [verification report](https://github.com/CaspiusLabs/Emerald-YM/blob/v0.4.0/docs/VERIFICATION.md). Do not treat simulated power loss as a completed physical power-cycle or cross-firmware update test.

The binary was built locally with the documented Windows toolchain. No GitHub build attestation is claimed. Repository remains private by owner request; no Firmware Hub submission has been sent. Remaining listing checks are in docs/FIRMWARE-HUB.md.

Combined firmware GPL-3.0-only, with component notices in LICENSING.md. Original music and graphics; independent of M-VAVE, Sega and Yamaha.
