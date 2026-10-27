# Emerald YM 0.4.3 user manual

Emerald YM makes original, continuously generated music with five YM2612 FM voices, DAC percussion and a Sega-style PSG. It starts paused. Turn MASTER down, press PLAY, then raise the level gradually. In validated 0.4.3, audio comes from the FM-1 speaker/headphone output, not USB audio. Development version 0.5.0 also enables USB recording of the stereo master mix at 16-bit / 44.1 kHz while retaining MIDI and status diagnostics. Exact 0.5.0 passed a short physical recording/MIDI/status test (CPU MAX72%, XRUNS0). The 0.5.1 follow-up advertises the audio name Emerald YM FM-1 but is not yet flashed; 0.5.0 appeared as Line (Felucca) on the test computer. In a recorder, select Emerald YM FM-1 as the recording input, stereo, 44.1 kHz; MASTER affects the captured mix. No computer-to-FM-1 audio or individual instrument tracks are provided.

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

The port is `Emerald YM FM-1`. Standard Start/Continue resumes playback; Stop pauses. Notes 53–79 on any MIDI channel join the physical keyboard mask. Velocity is a gate, not volume scaling. CC120/123 releases USB-held notes. Incoming MIDI Clock is unused.

For remote effects, MIDI channels 1–12 select the sources in the table. CC20 = DECAY, CC21 = COLOR, CC22 = ECHO, mapped from 0–127 to 0–100%. Details and status-query bytes are in [YM-USB-PROTOCOL.md](../YM-USB-PROTOCOL.md). Do not send this private query to other firmware or unrelated devices.

## Data, installation and troubleshooting

Emerald does not load another firmware's projects, presets or samples. It saves its own instrument settings only, not complete songs/projects. Saving uses the existing 0xFC000/0xFD000 project-sector pair and may overwrite another firmware's project data there. Back up before switching; see [installation](INSTALLING.md).

If there is no sound, check MASTER, PLAY, ARP for PSG, and the selected source levels. Three live notes can temporarily replace the PSG arp. FX values of zero are neutral.

`OVERLOAD: AUDIO STOPPED` means the protective limit has latched after eight over-budget blocks. Restart to clear it. Do not raise/remove the 85% guard. Report the version, actions and USB status if possible; physical test results cover representative scenes, not every setting.

The inherited update route and OCT− + OCT+ held for five seconds remain. There is no guaranteed automatic rollback. Recovery and the remaining physical update tests are described in [installation](INSTALLING.md) and [verification](VERIFICATION.md).
