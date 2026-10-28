# Emerald YM USB status protocol v1 (firmware 0.1.2 and later)

This is the protocol implemented by `firmware/src/ym/ym_midi.c` (previously in `ym_loop.c`), not a guessed command for stock M-VAVE or DUB-1. Compatible identities are `FM-1_68000102` (0.1.2), `FM-1_68000103` (0.1.3), `FM-1_68000104` (0.1.4), `FM-1_68000200` (0.2.0), `FM-1_68000201` (0.2.1), `FM-1_68000202` (0.2.2), `FM-1_68000203` (0.2.3), `FM-1_68000204` (0.2.4), `FM-1_68000300` (0.3.0), `FM-1_68000301` (0.3.1), `FM-1_68000400` (0.4.0), `FM-1_68000402` (0.4.2), `FM-1_68000403` (0.4.3), and `FM-1_68000500` (0.5.0) `FM-1_68000501` (0.5.1), `FM-1_68000600` (0.6.0), `FM-1_68000700` (0.7.0), and development `FM-1_68000900` (0.9.0). The UI update in 0.1.4 does not change the protocol. It never exposes flash erase/write, preset or firmware operations. The existing Felucca updater is separate and unchanged.

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

Before 0.9.0, standard single-byte MIDI System Real-Time messages are supported as follows: `FA` Start and `FB` Continue request playback; `FC` Stop requests a fade to silence. For this continuous generator Start and Continue both resume the existing position; they do not reseed. Start/Continue are ignored when the overload protection has latched. MIDI Clock F8 remains unused. In 0.2.0, Note On/Off (all MIDI channels) in the F3..G5 range (53..79) adds held keys to the physical keyboard mask. Velocity zero is Note Off; nonzero velocity is a gate (no velocity scaling). CC 120/123 clears USB-held notes. These are standard MIDI messages, not private SysEx. Earlier versions ignore note messages. Physical PLAY works as before.

Query only the explicitly selected Emerald YM FM-1 input/output pair. Do not probe other connected MIDI gear. Suggested test: verify identity via the unchanged documented Felucca identity handshake; read baseline; send Start; collect status once per second for 30 seconds; send Stop; read status. Stop immediately on overload or increasing XRUNs.

## 0.2.0 live PSG test

ARP toggles all PSG voices, including live keys. PLAY pauses only the backing sequencer: held live keys continue. The live allocator selects the three most recently pressed keys (highest key wins simultaneous mask changes), returning to previously held keys when a voice is released. Physical and USB-held keys are combined. With fewer than three live keys the third PSG voice is available to the generative arp. `ym_hardware_test.py --live-stress` alternates two and three held notes through documented standard Note On/Off, always cleaning them up on exit. No protocol field or private command was added.

In 0.2.2, ALGORITHM controls PSG mix 0..400% (default 300%). This is local UI only; no new SysEx command was added. The bottom hint displays the percentage. PSG off/on retains that mix until reboot; there is no automatic persistent write.

In 0.2.3, SELECT controls the five FM melodic channels (0..100%), and PRESETS controls DAC percussion and PSG noise (0..100%). ALGORITHM retains PSG mix 0..400%. CPU/XRUN counters are hidden on the panel but continue to update through this unchanged status protocol. The 85% guard is unchanged. No new private command was added.

In 0.2.4, SELECT chooses one of twelve sources and ALGORITHM adjusts only that source (0..100% of the accepted default balance). PRESETS remains the global percussion bus. No remote mix-control message was added; status layout remains unchanged.

## 0.3.0 FX controls

FX button cycles the edited parameter DECAY/COLOR/ECHO of the selected source; it never disables the other parameters. PRESETS edits its value 0..100%. Independent 12x3 volatile settings, all zero at boot. PRESETS no longer controls the global drum bus. Standard MIDI CC20/21/22 map to DECAY/COLOR/ECHO; MIDI channels 1..12 map to source indexes 0..11 from the mixer. Values 0..127 map to rounded 0..100%. These are documented application mappings of standard CC messages, not private SysEx. FX does not change the v1 status format or expose storage writes.

`--effects-stress` is restricted to identity FM-1_68000300, sets DECAY25/COLOR100/ECHO100 on all 12 sources, and restores zero in finally. Combined with `--live-stress`, it also exercises two/three held notes. The raw report records effect settings separately from audio counters.

In 0.3.1 the physical rotary roles are swapped: PRESETS is the selected source level, ALGORITHM is the selected FX amount. FX button still selects DECAY/COLOR/ECHO. Audio processing and MIDI CC mappings are unchanged.

## 0.4.0 persistent instrument settings

Status v1 and its read-only query are unchanged. Local instrument levels and MIDI CC20–22 effect edits now mark settings dirty. The application saves them through the existing A/B project store only after three seconds without edits, backing playback or held live keys. SAVE pauses backing and requests a quiet save. This changes the persistence of CC edits: do not send test values unless their eventual storage is intended. The hardware-test tool rejects --effects-stress for 0.4.x because the current protocol has no settings snapshot/restore command. Note-only live stress does not change persistent levels/FX. No new SysEx, storage-control message or overload bypass has been added.

## 0.4.2 performance settings

The same read-only status v1 is retained. Schema 2 adds K1–K4 (tempo, density, mutation, signed transpose) to the persistent snapshot; the old schema 1 mixer/FX record is accepted without a boot-time write. Local knob and SEQ changes mark the snapshot dirty and use the same paused/quiet save policy. No remote performance-setting or storage command is introduced. Effect-stress remains disabled for persistent firmware versions.

## Development 0.5.0 USB audio

UAC1 capture (stereo 16-bit / 44.1 kHz) uses isochronous EP4 IN; USB MIDI remains on EP1 IN/OUT. The status v1 request/reply and standard MIDI controls are unchanged. The hardware-test tool accepts the new identity and passed the documented 30-second recording/live-MIDI/status run on exact 0.5.0. Version 0.5.1 keeps status v1 unchanged and passed its separate recording/live-MIDI/status run. Source/tooling 0.5.2 does not introduce a new device identity or status message. Audio XRUN/status counters do not expose the separate internal UAC underrun/overrun/missed-packet counters; a later physical recording check must inspect the recording too. No UAC diagnostic SysEx extension is introduced.

## Development 0.6.0 MASTER display

The new volume slider displays the existing filtered physical MASTER ADC position. It does not add a MIDI or SysEx control and does not change master gain, audio rendering, persistence or status v1. The hardware-test tool accepts the explicit new identity but has not been executed on a device for this version.

## Development 0.7.0 USB ONLY

GLO toggles session-only physical-output mute. USB capture stays before that mute; MIDI and status v1 are unchanged. There is no remote routing command or route-status field. Confirm GLO/physical silence with the owner; the read-only query cannot establish the switch position. The existing persistent-effects stress guard remains in place.

## Development 0.9.0 USB Clock and TRS input

USB FA selects external clock and resets the arrangement with the current seed; FB selects external clock and preserves pulse/step position; FC pauses. F8 is 24 PPQ, with the first pulse after FA triggering step 1 and every six pulses advancing a sixteenth. F8 alone updates the tempo estimate but does not take over internal playback. External steps count pulses rather than free-running at estimated BPM. More than 500 ms without a clock stops external playback; returning F8 alone cannot restart it. Start/Continue remain blocked by overload protection. K1 restores internal tempo. No Song Position Pointer, outgoing Clock or TRS transport/Clock support is added.

The unchanged Playing status field reflects the requested transport: 1 can mean armed and waiting for the first external pulse. It is not proof that the arrangement is advancing; use bar progress and evidence of an actual clock source for that assessment.

The minijack input accepts live notes and existing effect CCs. Its internal queue source is tagged separately from USB, with per-port held masks; CC120/123 clears only the originating port. This source tag is not a new USB cable or wire protocol. TRS SysEx is ignored, including updater requests. Input loss clears both MIDI held masks and stops external playback. Physical keys remain independent. External mode/BPM are session-only and not exposed in status v1; the private query/reply bytes above are unchanged. See [manual and physical-test limits](docs/MANUAL.md#usb-midi-clock-and-trs-input--development-090).

## Development 0.10.0 note direction

EDIT is a local session-only mode switch. In IN (dim), existing USB/TRS notes and CCs are accepted. In OUT (bright), those messages are ignored and their held masks cleared; only physical keyboard and generated notes are exported over USB. USB real-time Clock/transport and the status query remain accepted. No new SysEx, wire cable number, Clock output, Thru or remote mode setter is added. Status v1 cannot report EDIT state.

Output is standard Note On/Off at velocity 90, cable 0: keyboard ch1; FM bass/lead/chords ch2–6; PSG arp ch7; noise ch8 note76; DAC drums ch10 notes36/38/42/46. After a USB suspend/reset following output use, cleanup sends CC123 to channels1–8 and10. Notes are block-quantized (~5.8ms); long backpressure may coalesce intermediate events while retaining release retries. See the [manual](docs/MANUAL.md#edit--usb-midi-out--development-0100) for routing and cleanup limitations. The physical minijack remains input; no undocumented TX register/pin changes are made.
