# Emerald YM 0.1.3 — historical working hardware prototype

English editorial summary; original Polish note remains in Git history. Firmware was installed and passed over a minute of automated playback; the user confirmed audible smooth music.

Identity FM-1_68000103, port Emerald YM FM-1.60 status reads at >=1 s intervals between MIDI Start/Stop reached bar 35, covering the arrangement and its return. CPU MAX 80%, longest block 4665 us of about 5804 us, XRUN 0, over 85% blocks 0, failed boots 0. Uptime/buffer count advanced without a recorded reboot. Settings:132 BPM,density 100,mutation 35,transpose 0,seed 20261026. Evidence: hardware-0.1.3-continuous.json. Not proof of every setting or multi-hour stability.

One ymfm YM2612 at 7670454 Hz, five FM voices and channel-six 8-bit DAC drums, no PSG. Original endless motifs/harmony/bass/chords, no Sonic game music/samples.

Optimizations: RAM code/tables with correct far calls and code guard; avoid repeated full-channel scanning for one channel output while preserving four operators/activity/DAC nonlinearity; skip duplicate DAC writes; permanently reserve channel six for DAC and omit unused FM phase/envelope work; inline hot clock functions on FM-1. Generic builds without EMERALD_FIXED_DAC retain six FM paths. This specialization must not play arbitrary registers/VGM that switch channel six. NTSC clock/voices/85% guard unchanged. Desktop WAV SHA-256 488849e65ddbc8178f98311bc487abba968b1e7846cfa4c75e24990153866605 unchanged.

Earlier 0.1.0 measured 92% /XRUN 0; RAM-only 0.1.1 was insufficient; USB 0.1.2 measured 87% /XRUN 0 with protection.0.1.3 reached 80% without protection.

Boot paused; PLAY fade/pause, REC reseed/restart; K1 tempo 80–180,K2 density,K3 mutation,K4 transpose±12; SEQ mutation 0/35, not a freeze of all randomness. MIDI Start/Continue/Stop supported; incoming notes and physical keyboard not yet connected. CPU/XRUN shown. USB audio disabled, listen through device output. Volatile settings, no protection-bypass command.

Package 609666 B; app 97040 B; hot 19372 B; RAMTEXT 22260/24576 B; data+BSS 28916/98304 B; pool 69120/344064 B. Build/image/ranges/no-soft-double/flash-off and simulated OTA/loader passed.180+s PC render tests checked block sizes/seeds/pause/extremes/no allocation/no limiter hits. Hardware measurement complements these tests.

Historical vendor-sdk/machine-specific build paths are obsolete; current BUILDING.md describes standalone tools. Source lineage: DUB-1 e6fe7b8f8f3114019bb36b5b536ad458eacd2c2d; ymfm 81aec25ccbb98f4873a255f7551ac4dadac59b4a with documented local changes. Preserved Felucca updater, watchdog, failed-boot safe mode and OCT pair 5 s shortcut, without guaranteed rollback/recovery. Combined GPL-3.0-only; ymfm BSD-3-Clause; original engine/adapter MIT; SDK inputs Apache-2.0; fonts with notices; compiler excluded.
