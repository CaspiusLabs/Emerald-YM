# Emerald YM 0.4.3 user manual

Emerald YM makes original, continuously generated music with five YM2612 FM voices, DAC percussion and a Sega-style PSG. It starts paused. Turn MASTER down, press PLAY, then raise the level gradually. In validated 0.4.3, audio comes from the FM-1 speaker/headphone output, not USB audio. Development version 0.5.0 also enables USB recording of the stereo master mix at 16-bit / 44.1 kHz while retaining MIDI and status diagnostics. Exact 0.5.0 passed a short physical recording/MIDI/status test (CPU MAX72%, XRUNS 0). Exact 0.5.1 also passed recording/MIDI/status but Windows still displayed Line (Felucca), despite the USB-reported name Emerald YM FM-1. See the [targeted Windows registration refresh](WINDOWS-AUDIO-NAME.md). In a recorder, select Emerald YM FM-1 as the recording input, stereo, 44.1 kHz; MASTER affects the captured mix. No computer-to-FM-1 audio or individual instrument tracks are provided.

## Playing and shaping the backing

| Control | Action |
|---|---|
| PLAY | Start/pause the backing, with a fade; resume the current sequence |
| REC | Choose a new seed and restart the sequence |
| SAVE | Pause backing and commit pending instrument settings once quiet |
| SEQ | Toggle mutation 0 / 35; this does not freeze every random decision |
| K1 | Tempo, 80–180 BPM |
| K2 | Melody density, 0–100% |
| K3 | Mutation, 0–100% |
| K4 | Transpose, −12 to +12 semitones, including live keys |
| MASTER | Overall output level |

A short boot screen uses an original console silhouette with a circular cartridge deck, vents and a red power LED. The version comes from VERSION and appears below the 16-BIT badge. The boot screen does not delay USB/MIDI/panel servicing; a button, live note or playback dismisses it.

The screen shows play state, bar/step position, four parameter cards and the selected instrument/effect. Only changed tiles are transmitted to the LCD. CPU/XRUN counters remain available over USB; they are not shown in normal operation.

## Instrument mixer and effects

1. Turn **SELECT** to choose an instrument.
2. Turn **PRESETS** to set its volume from 0 to 100%, in 5% steps.
3. Press **FX** to select DECAY, COLOR or ECHO.
4. Turn **ALGORITHM** to set that effect from 0 to 100%, in 5% steps.

| SELECT position | Instrument |
|---|---|
| 1 | FM BASS |
| 2 | FM LEAD |
| 3–5 | FM CHORD1, FM CHORD2, FM CHORD3 |
| 6 | KICK |
| 7 | SNARE |
| 8 | CLOSED HAT |
| 9 | OPEN HAT |
| 10 | PSG ARP |
| 11 | PSG LIVE (the live keyboard group, up to three voices) |
| 12 | PSG NOISE |

All sources start at 100% of the calibrated default balance. Each has three independent effect amounts, initially zero on the first boot without saved settings. Selecting another instrument or effect leaves previous values intact. All effects can run together. PRESETS is no longer a global drum-volume control.

- **DECAY:** increasing the amount shortens notes. FM gates/releases become shorter; drum/noise segments and arp gates shorten. With live PSG, a nonzero amount limits held-note duration, from roughly one second down to 20 ms. At zero, a held live note sustains normally.
- **COLOR:** source-specific timbre: FM modulation/feedback, drum bit reduction, PSG tone ring modulation, or one of three noise clock ranges. This is not a shared filter.
- **ECHO:** each source sends into one shared stereo delay, about 186 ms with fixed feedback. It is not tempo synchronized. Existing echoes may briefly remain after muting the source. REC resets the delay history.

The FX LED is dim when all effects for the selected instrument are zero, bright when any is active. `PRE` labels the instrument level; `ALG` labels the edited FX amount. Instrument levels and all 36 effect amounts are persistent. They load before audio starts. After an edit the transport tile shows **SAVE *** and SAVE lights brightly. Saving waits until there have been no edits, backing playback or held keys for three seconds. Thus no erase is attempted in the middle of music. Press SAVE to pause the backing and request saving; release live keys, then wait for **SAVED** before turning off. **SAVING** means do not remove power; **SAVE ERR** keeps changes in RAM and requires a new edit or SAVE to retry. There is no automatic erase/retry loop.

The first boot without compatible settings uses 100% levels, zero effects and knob defaults (132 BPM, density 100%, mutation 35%, transpose 0) without writing flash. Existing 0.4.0 schema 1 settings keep all levels/FX and use those knob defaults until you change/save settings. Schema 2 is committed only after an actual edit and a quiet save. Older 0.4.0 firmware cannot read schema 2 and will use defaults if downgraded. Reverting an edit to the saved value cancels the pending save. K1 tempo, K2 density, K3 mutation and K4 signed transpose are also stored in 0.4.2, including mutation changes through SEQ. These restore before audio starts. Seed, master, PSG enable, selected instrument and FX cursor are not stored. REC does not erase saved levels/FX. Inherited DUB-1 menus and project saving are not implemented.

## Live keyboard and LEDs

Keys F3–G5 play PSG tones, up to three at once, including when the backing is paused. The most recently pressed notes have priority; previously held notes return as voices become available. Simultaneous mask changes favor higher keys. With three live notes, only the PSG arpeggio gives up its voice; FM and DAC keep playing.

**ARP** toggles the entire PSG, including keyboard and noise. Dim keys indicate gated backing notes within the physical range; bright keys indicate assigned live notes. Release tails and notes outside the range are not shown. Transposed live notes light the physical key you pressed.

## USB MIDI

The port is `Emerald YM FM-1`. In released 0.7.0, standard Start/Continue resumes playback; Stop pauses, and MIDI Clock is unused. Notes 53–79 on any MIDI channel join the physical keyboard mask. Velocity is a gate, not volume scaling. CC120/123 releases USB-held notes. Development 0.9.0 adds the clock/TRS behavior below.

For remote effects, MIDI channels 1–12 select the sources in the table. CC20 = DECAY, CC21 = COLOR, CC22 = ECHO, mapped from 0–127 to 0–100%. Details and status-query bytes are in [YM-USB-PROTOCOL.md](../YM-USB-PROTOCOL.md). Do not send this private query to other firmware or unrelated devices.

## USB MIDI Clock and TRS input — development 0.9.0

This section records the original 0.9.0 desktop candidate. Later owner-authorized physical tests established USB Clock and note reception through USB and UM-ONE/TRS on installed 0.9.0. Those observations do not validate the new 0.10.0 output implementation; its scope is documented below.

In a DAW, choose `Emerald YM FM-1` as a MIDI destination and enable sending MIDI Clock and transport. Start from the beginning of the arrangement:

- **USB Start (FA):** reset to the beginning using the current seed, select external clock and wait for F8. The first Clock plays step 1, then each six pulses advances one sixteenth note (24 clocks per quarter note).
- **USB Continue (FB):** select external clock and resume the retained step/pulse position. **USB Stop (FC):** pause and retain that position. Live keys and release/echo tails remain available.
- **Clock alone (F8):** update the tempo estimate, but do not start playback or take over internal mode without Start/Continue. The display/gate-length estimate supports 30–300 BPM and settles over a quarter note; sequencer timing counts pulses directly, so integer BPM rounding does not accumulate timing drift.
- **Clock missing for more than 500 ms:** stop the externally clocked backing and show **USB LOST**. Returning Clock alone does not restart it; use Start, Continue or physical PLAY. Input queue loss also clears USB/TRS held notes and stops external playback, rather than continuing with a missing pulse.
- **Turn K1:** return to internal tempo (80–180 BPM). USB clock mode and incoming tempo are session-only: they do not overwrite the saved K1 value or dirty the settings. The tempo tile shows **K1 USB CLK**, **USB WAIT** or **USB LOST** as appropriate.

Clocked step changes are applied at audio-block boundaries (256 frames, about 5.8 ms), with at most one pending step consumed per block. USB/main-loop scheduling adds latency/jitter; this is not sample-accurate synchronization, and physical accuracy has not been measured. Song Position Pointer/DAW seeking, outgoing Clock, and TRS Clock/transport are not implemented. Start resets Emerald to its first bar regardless of a DAW cursor elsewhere; Continue does not seek to that cursor.

TRS **MIDI IN** on the minijack is enabled at 31,250 baud. Connect a controller's MIDI OUT using an FM-1-compatible TRS-MIDI adapter/cable. It accepts the same live-note range and effect CC mapping as USB. Both inputs may be used together: a USB Note Off does not cancel a still-held TRS note, and CC120/123 clears only the originating port's held mask. Channels share the live key mask within each port. Physical keyboard keys remain independent. TRS SysEx is ignored and cannot enter the updater; TRS real-time messages cannot take over the USB clock.

USB audio, USB ONLY on GLO and diagnostic status v1 are retained. Status v1 does not report external clock mode/BPM. The existing hardware-test helper's identity allowlist intentionally does not accept 0.9.0: its Start-only playback test supplies no clock. Physical verification needs an explicitly authorized clock-aware test and TRS controller; desktop tests are not that evidence. [Original desktop verification report](verification/usb-clock-trs-0.9.0/README.md).

## Data, installation and troubleshooting

Emerald does not load another firmware's projects, presets or samples. It saves its own instrument settings only, not complete songs/projects. Saving uses the existing 0xFC000/0xFD000 project-sector pair and may overwrite another firmware's project data there. Back up before switching; see [installation](INSTALLING.md).

If there is no sound, check MASTER, PLAY, ARP for PSG, and the selected source levels. Three live notes can temporarily replace the PSG arp. FX values of zero are neutral.

`OVERLOAD: AUDIO STOPPED` means the protective limit has latched after eight over-budget blocks. Restart to clear it. Do not raise/remove the 85% guard. Report the version, actions and USB status if possible; physical test results cover representative scenes, not every setting.

The inherited update route and OCT− + OCT+ held for five seconds remain. There is no guaranteed automatic rollback. Recovery and the remaining physical update tests are described in [installation](INSTALLING.md) and [verification](VERIFICATION.md).

## Development 0.6.0 main screen

The left VOLUME rail follows the existing filtered position of the physical MASTER knob. Turn clockwise to move the red handle upward toward 10, counter-clockwise toward 0. This 0-10 scale shows control position, not dB, a VU meter or an additional gain stage. The display cannot be dragged: the FM-1 knob remains the control. MASTER still affects the speaker/headphone and USB-recorded master mix and is not stored by SAVE.

Visual hysteresis suppresses small ADC fluctuations around a display-pixel boundary. It does not alter the audio gain law or knob filtering. Only the narrow slider tile redraws on movement; the static rail and other controls remain untouched. Transport, SAVE states, K1-K4, instrument level and selected effect controls keep their existing behavior. Exact 0.6.0 passed a subsequent 30-second physical recording/live-MIDI/status run (maximum audio CPU 72%, XRUNS 0); the owner confirmed correct screen/slider behavior. See the [version-specific evidence](verification/master-ui-0.6.0/README.md). Documentation version 0.6.1 does not change that firmware.

## Development 0.7.0: GLO / USB ONLY

Press GLO once to mute physical speaker/headphone audio while keeping USB recording, live keys, backing and MIDI/status active. The GLO LED is bright and the bottom-left tile says USB ONLY. Press again to restore physical audio; GLO turns off and the headphone symbol returns. MASTER and its slider still control the level sent to USB.

Development 0.11.2 changes only GLO illumination: it stays **dim** when physical audio is enabled and becomes **bright** in USB ONLY. Restoring physical audio returns it to dim, so the assigned function remains discoverable. This version is not installed; output routing and persistence behavior are unchanged.

The local output fades over one 256-frame block (~5.8 ms) to reduce switching clicks. The USB mix is captured before this local fade/mute. The amplifier is not powered off, so this feature does not promise removal of analog hiss or power savings. USB ONLY is session-only, not stored by SAVE; reboot restores normal physical output. It does not mute only the built-in speaker while retaining headphones.

## USB MIDI master clock — development 0.11.0

Version 0.11.0 adds clock/transport output over the existing USB MIDI port; the minijack remains input and external hardware still needs a compatible USB host or computer/interface route. Version 0.11.1 repairs the diagnostic dispatch race and is now installed; development 0.11.2 retains that behavior and is not installed. The public 0.10.0 release is unchanged. [Historical 0.11.0 physical evidence and limits](verification/usb-midi-sync-0.11.0/hardware/README.md) · [0.11.1 repair and original desktop tests](verification/usb-midi-sync-0.11.1/README.md).

- **EDIT OUT:** FM-1 is master, using K1's internal tempo (80–180 BPM). Incoming USB Timing Clock, Start, Continue and Stop are ignored to prevent a DAW echo loop. Enable external MIDI clock on the receiving instrument. Local audio and USB capture continue.
- **PLAY in OUT:** restart the arrangement with the current seed, send Start (`FA`), then Timing Clock (`F8`) at **24 pulses per quarter / six per sequencer step**, before the block's sequencer notes. The next PLAY pauses and sends Stop (`FC`) plus obsolete Note Offs. Another PLAY starts from the beginning again, rather than resuming with Continue.
- **K1:** change master tempo without resetting the current pulse phase. Clock is sent only while the backing runs, not while paused. No outgoing Continue or Song Position Pointer is added.
- **EDIT to OUT while playing:** switch from external following to internal tempo and restart both local arrangement and outgoing transport. **EDIT to IN:** send Stop/release outgoing notes, retain local playback, and accept subsequent fresh USB clock/transport as before. Pre-switch queued performance/real-time input is discarded. Ownership is not saved.
- **REC while playing OUT:** generate/restart the arrangement and send Stop followed by a new Start. A backing stop or output disable also ends outgoing transport; overload protection and update-entry behavior remain active.

Clock count derives from actual native sequencer phase, with no separate free-running timer. USB packet generation still runs at completed **256-frame block boundaries (~5.8 ms)**. This is not a submillisecond timestamped clock; target USB scheduling, receiver jitter, concurrent USB-audio CPU load and external hardware synchronization need a later authorized physical test.

If USB cannot drain a Clock before the next is due, a pulse is missed, or the queue fills, the master stops instead of sending catch-up bursts. The backing pauses and the header shows **USB MIDI WAIT**; Note Off/CC123 cleanup is attempted. Restore the USB/host connection and press PLAY explicitly to restart from the beginning. Reconnection does not automatically resume transport. A disconnected/non-reading receiver cannot be guaranteed to receive Stop/Note Off; use its own stop/panic if needed. USB SysEx/status keeps priority and no timing threshold is relaxed.

The keyboard still sends on channel 1 while paused or after a backing stop; existing sequence channel assignments and independent GLO/USB ONLY behavior remain unchanged. [Desktop build, tests and memory](verification/usb-midi-sync-0.11.0/README.md).

## EDIT / USB MIDI OUT — 0.10.0

Exact 0.10.0 has passed physical USB note input/output, held-key EDIT switching, USB Clock and simultaneous USB-audio/status testing. See the [hardware report](verification/usb-midi-out-0.10.0/hardware/README.md).

- **EDIT dim (boot default): MIDI IN.** USB and minijack notes/effect CCs play/edit Emerald as before.
- **EDIT bright: USB MIDI OUT.** The header changes to USB MIDI OUT. Physical keyboard and sequencer notes are sent to the computer; incoming USB/TRS notes and effect CCs are ignored. This prevents a DAW echo loop. Local synthesis and physical-key LEDs continue; no MIDI Thru is provided.
- USB Clock, Start/Continue/Stop, status v1 and USB audio remain available in both modes. OUT does not generate MIDI Clock, transport, Program Change or effect CC messages.
- The mode is session-only; SAVE does not store it. GLO / USB ONLY remains independent.

| MIDI channel (1-based) | Part |
|---|---|
| 1 | Physical keyboard, including chords |
| 2 | FM bass |
| 3 | FM lead |
| 4–6 | Three FM chord voices |
| 7 | PSG arpeggio when its local voice is available |
| 8 | PSG noise, note 76 |
| 10 | Kick 36, snare 38, closed hat 42, open hat 46 |

Use a multitimbral instrument or separate DAW tracks to assign sounds to these channels. Output velocity is fixed at 90; Emerald's mixer levels and audio effects remain local. K4 transposes the keyboard and melodic sequence. Physical key output is not limited by the local three-PSG-voice allocator or ARP switch. Output is observed at 256-frame block boundaries (~5.8 ms), not sample-accurate MIDI. Short drums are emitted as a pulse even when their local sample finishes inside a block. Rapid events during prolonged host backpressure can be coalesced; Note Off cleanup has priority over new notes.

Stopping the backing releases sequencer notes while held physical keys continue. Changing EDIT, transposition, seed or overload state releases obsolete notes. A reset/suspend recovery sends CC123 on channels 1–8 and 10 before rebuilding current notes. Use dedicated destination channels: these cleanup messages affect other notes sharing those channels. A disconnected/non-reading computer cannot receive Note Off; use its panic if it retains voices after disconnect. Update entry disables OUT and provides a bounded 20 ms drain opportunity, not an acknowledgement from the receiving instrument.

### Physical MIDI output through UM-ONE

The FM-1 minijack remains **MIDI IN**. The [official SDK](https://github.com/m-vave/fm1_open) documents DIN input with USB/BLE output, not a supported DIN/TRS output mode. EDIT does not change electrical direction or UART pins.

For external hardware output: connect FM-1 and UM-ONE to the computer by USB, route the DAW's **Emerald YM FM-1 MIDI input** to **UM-ONE MIDI output**, then connect Roland's **CONNECT TO MIDI IN** lead to the destination instrument's MIDI IN. The FM-1 minijack is not used in this output route. A standalone alternative needs a compatible USB MIDI host, not a passive adapter.

See [verification and memory limits](verification/usb-midi-out-0.10.0/README.md). The short hardware test measured the combined USB-audio/OUT load; long-session behavior, physical destination-synth rendering and separate UAC packet counters remain unverified.
