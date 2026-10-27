# Emerald YM 0.1.0 — historical experiment summary

This English editorial summary replaces the earlier Polish working note. The original wording remains in Git history; raw test evidence is not rewritten. At the time of that note the package had been built and tested on a PC, **not yet installed**. Later hardware results appear in CHANGELOG.md.

The first port used Aaron Giles's ymfm at commit 81aec25ccbb98f4873a255f7551ac4dadac59b4a, five FM voices and emulated channel-six 8-bit DAC percussion, no PSG. The seeded E-major sequencer produced original motifs/harmony/bass/chords/arrangement with synthetic drum samples, without Sonic game data. DUB-1/Felucca base commit e6fe7b8f8f3114019bb36b5b536ad458eacd2c2d supplied hardware/USB/updating/watchdog/safe-mode code.

Streaming events, short samples and small buffers replaced minute-long desktop note/audio arrays. A narrow freestanding C++ adapter used an arena initialized before interrupts, with no render allocations. Save states/debug WAV were disabled on target; FM calculations preserved.

Package identity FM-1_68000100, expected USB Emerald YM FM-1 (Windows may cache aliases), inherited VID/PID 1209:0001 and unchanged loader. Package was 609666 B, app 96636 B, data+BSS 28656/98304 B, graphics pool 69120/344064 B; stacks/reserves were separate, so this is not total RAM usage. A60s host WAV was a desktop render, not hardware audio.

Historical controls: boot paused, low MASTER then PLAY; K1 tempo 80–180, K2 density 0–100, K3 mutation 0–100, K4 transpose−12..+12; REC reseed/restart, SEQ mutation 0/35. Volatile settings; keyboard/MIDI notes unconnected. Display showed tempo/bar/CPU MAX/XRUN.256-frame budget about 5.8ms; eight blocks over 85% after warm-up latched OVERLOAD until restart while USB/panel remained active.

PC checks: actual pi32v2 r3/mfprev 1 compile using JieLi Windows 2.5.3 clang 4.0.1; image/memory/HAL ranges, no soft-double, no forbidden flash-off XIP calls;180+s rendering across blocks 256/63, seeds/pause/extremes/no allocations/no limiter hits. OTA handshake/staging/CRC/commit/disconnect/no-commit cleanup and different-image loader flash-window/header/key tests passed. Mock installer passed; inherited JS comparison skipped without Node. Three SDK inputs matched upstream hashes.

No target timing/stack/IRQ/audio/panel or physical power-loss behavior had been established. The first planned hardware test was boot, PLAY, CPU/XRUN, then seeds/tempo/transpose. Later 0.1.0 reached 92% and protection stopped audio. Watchdog/safe mode/update shortcut were not guarantees of recovery or automatic rollback.

Historical builds referenced a vendor-sdk layout and machine-specific tool paths. Use current BUILDING.md instead; do not run archived migration scripts. Installer needed mido/python-rtmidi and an explicitly verified FM-1 port, never Roland UM-ONE; no --force or automatic install. The inherited OCT pair held 5 s entered update mode.

Licenses: combined GPL-3.0-only; ymfm BSD-3-Clause; original engine/adapter MIT; SDK inputs Apache-2.0; fonts retain notices. Compiler not distributed. See current LICENSING.md.
