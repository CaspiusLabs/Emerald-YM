# Emerald YM 0.1.1 — historical RAM optimization summary

English editorial summary; original Polish note remains in Git history.0.1.0 booted, but the user reported CPU MAX 92%, XRUN 0 and OVERLOAD AUDIO STOPPED: protective 85% stop, not a proven stable run.

0.1.1 moved YM2612/sequencer code, tables and short samples from XIP to RAM without changing FM calculations, NTSC clock, audible voices or guard. Desktop output remained bit-identical: WAV SHA-256 488849e65ddbc8178f98311bc487abba968b1e7846cfa4c75e24990153866605.

Identity FM-1_68000101; app 96696 B; hot code/tables 19500 B; RAMTEXT 22388/24576 B including flash code; data+BSS 28656/98304 B; pool 69120/344064 B. .audio_hot startup copy precedes emulator/IRQ initialization. Executable guard covers contiguous flash/audio RAM code, not app data/stacks. Platform indirect calls bridge XIP/RAM safely. Flash-off .ram_text remains call-checked; audio is stopped during programming.

Image/memory/no-soft-double, deterministic streaming across block sizes, seed/pause/extremes/no allocations, no limiter hits and different-image 0.1.0→0.1.1 OTA/loader simulation passed. The note requested another 20–30 s physical reading; subsequent user feedback said the overload behavior was unchanged. A RAM move alone was insufficient.

Boot paused; PLAY toggles, REC reseeds, K1 tempo/K2 density/K3 mutation/K4 transpose. Eight over 85% blocks latch a stop until restart. Old vendor-sdk and fixed-version build instructions are superseded by current BUILDING.md. Build/tests/export do not open MIDI; installation is separate. License terms unchanged: combined GPL-3.0-only, ymfm BSD-3-Clause, original code MIT, SDK Apache-2.0, font notices.
