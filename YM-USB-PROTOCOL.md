# Emerald YM USB status protocol v1 (firmware 0.1.2 and later)

This is the protocol implemented by `firmware/src/ym/ym_loop.c`, not a guessed command for stock M-VAVE or DUB-1. Compatible identities are `FM-1_68000102` (0.1.2), `FM-1_68000103` (0.1.3), and `FM-1_68000104` (0.1.4). The UI update in 0.1.4 does not change the protocol. It never exposes flash erase/write, preset or firmware operations. The existing Felucca updater is separate and unchanged.

Send status query bytes: `F0 7D 45 4D 59 01 F7`. Manufacturer byte `7D` is the non-commercial/experimental namespace; `45 4D 59` is ASCII `EMY`.

Reply: `F0 7D 45 4D 59 41`, then eleven uint32 values, then `F7`. Each value is five little-endian 7-bit chunks (low 7 bits first; upper chunk uses at most 4 bits). Reply length including F0/F7 is 62 bytes. Field order:

0. Playing (0/1)
1. Smoothed audio CPU percentage
2. Worst audio block percentage since boot
3. XRUN count
4. Blocks exceeding the 85% threshold after warm-up (8 stops rendering)
5. Musical bar (1-based)
6. Current initial seed
7. Milliseconds since boot
8. Audio DMA halves handled
9. Worst block duration in microseconds
10. Failed boots recorded by boot guard

The status query takes a brief, consistent snapshot and replies; it changes no synth setting or persistent data. Unsupported commands are ignored by this handler. There is no command to bypass overload protection or reset its counters.

Standard single-byte MIDI System Real-Time messages are also supported: `FA` Start and `FB` Continue request playback; `FC` Stop requests a fade to silence. For this continuous generator Start and Continue both resume the existing position; they do not reseed. Start/Continue are ignored when the overload protection has latched. MIDI Clock F8 and incoming note messages remain unused. Physical PLAY works as before.

Query only the explicitly selected Emerald YM FM-1 input/output pair. Do not probe other connected MIDI gear. Suggested test: verify identity via the unchanged documented Felucca identity handshake; read baseline; send Start; collect status once per second for 30 seconds; send Stop; read status. Stop immediately on overload or increasing XRUNs.
