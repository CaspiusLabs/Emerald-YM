# DUB-1: plan and progress

A fork of AMB-1 (generative ambient for the M-VAVE FM-1) turned toward dub techno: chord stabs into a
tape echo, a sub bass, a four-on-the-floor kick, vinyl crackle, and an arrangement that drops and
brings back its layers in eight-bar phrases. Working name **DUB-1** (easy to change). Work happens on
the `dub` branch; `EMULATOR.md` says how to build and listen on the computer.

## The design

| AMB-1 | DUB-1 |
|---|---|
| Pad: four breathing voices | **Chord**: a four-voice stab (two detuned saws a voice, or FM keys) on a Euclidean pattern off the beat, through a resonant low-pass with its own envelope. One voicing (open m7, or a 9 without its fifth) shifted whole to each root, as a chord memory |
| Bells: four tape loops (13/17/23/29 quarters) | **Pings**: short FM blips on the same loop lengths, in eighths, mostly into the echo |
| Bass: sine and sub-octave, gliding | **Bass**: a sub (E1..Eb2) on five grooves: Drone, Offbeat, Dotted, Roller, Sync |
| Pulse: three Euclidean patterns (16/12/7) | **Drums**: four-on-the-floor kick with a click, a shaker on the sixteenths (Hits), an open hat off the beat; the kick ducks everything else (Duck) |
| Arc: a minutes-long tide | **Arrangement**: eight-bar phrases (Full, Break, Groove, Thin), a fill before the kick returns, the filter sweeping with it |
| Tape echo + Echoplex + plate | The same, with a band-pass that moves in the echo's feedback, a soft clip so the feedback can pass 100%, dub throws (a stab sent hard into the echo) |
| Freeze: the chord holds, the reverb rings | **Dub hold**: the echo stops listening and repeats its loop for ever; the chord holds |
| (none) | **Crackle**: vinyl hiss and clicks |

Tempo 90–140 BPM (default 122). Echo times 1/8, 1/8. (default), 1/4, 1/4., 1/2, 1 bar. Drift in bars
(2..64) or Hold.

### The pages (planned)

| Button | Page | KNOB 1–4 |
|---|---|---|
| HOME | Dub | Tempo, Evolve, Motion, Drift |
| ENV | Chord | Level, Stabs, Decay, Cutoff |
| ARP | Texture | Pings, Density, Tone, Crackle |
| LFO | Bass | Level, Octave, Groove, Fifth |
| SEQ | Drums | Level, Hits, Swing, Duck |
| FX | Space | Echo, Echo time, Feedback, Reverb |
| GLO | Setup | Root, Tune, MIDI out, Lights |

Set by the piece only: Mode (ALGORITHM), Chord tone (Saws / Keys), Hats colour, Plate size, ping octave
(OCT- / OCT+).

## Steps

1. Tempo range and the four-on-the-floor kick
2. The chord stab and its filter
3. The echo's feedback filter, throws, dub hold
4. The phrase arrangement
5. The pieces (eight recipes)
6. The UI (pages, labels, help cards, screen reader words, colours), storage, tests, manual

Steps 1–4 are done as one engine rewrite (`firmware/src/dsp/amb.c`), then tuned by ear (yours) and by
measurement (renders analysed for level, timing and spectrum).

## Done

- [x] Local simulator set up (`EMULATOR.md`, `.venv`, `tools/gen_font_noraqm.py`)
- [x] `dub` branch
- [x] Engine rewrite (steps 1–4): chord stabs, pings, bass grooves, drums with ducking, arrangement,
      dub echo (band-pass + soft-clipped feedback, throws, hold), crackle (`amb.c`, `amb.h`)
- [x] `host/en_render.c`: the engine alone to a WAV (parameter overrides, timed changes `NAME=V@T`,
      `freeze=T0-T1`, a bar-by-bar log); `tools/analyze_wav.py`: levels, bands, kick timing, spectrogram
- [x] First tuning by measurement: layer balance (the chord +12 dB, the pings +9 dB), longer echo tails,
      no half-diminished chords, fewer throws, keys as loud as the saws, crackle only while playing,
      feedback eased under 90% while stopped (tails always die); feedback at 100% stays bounded
- [x] CPU: on the host the new engine costs the same as AMB-1's or less (0.20–0.30% of real time vs
      0.27–0.31%)
- [x] Eight pieces, first draft: Basic, Chamber, Steppers, Fog, Concrete, Night Drive, Pressure, Haze
- [x] UI: pages, labels, help cards, messages, a dark palette, the section and phrase on the main
      screen, "hold"; screen reader texts
- [x] Storage: project magic "DUB1", format 1 (AMB-1's saved projects give the recipes)
- [x] Engine unit tests rewritten (`tests/host/amb_test.c`)
- [x] A listening pack: `build/listen/pack/` (each piece, dub hold, feedback swell, the arrangement)
- [x] Simulator scenarios and the screen reader's words for DUB-1; the whole suite passes
- [x] README, manual, licensing note and screenshots for DUB-1
- [x] The device build works here (`EN_FONT_NORAQM=1 ./build.sh` -> `build/amb.fwsc`): RAM 52.6 of
      96 KB, pool 332 of 344 KB, no soft doubles
- [x] DUB-1 by name on the device: splash, crash and safe-mode screens, USB strings; MIDI out off
      silences every channel

## TODO

- [ ] **Your ears**: listen to the pack; what to change in the sound and the pieces
- [ ] The web app's own page (`web/app/index.html`) and the emulator page (`web/emu/index.html`):
      pages, names, colours (needs emsdk to build and check)
- [ ] **On the device**: install (`python3 tools/fm1_install.py build/amb.fwsc`), check the Setup
      page's CPU and the sound on the FM-1's speaker and on headphones
- [ ] The product's name; where the words file and a hosted build are published (the `# url:` in
      `firmware/words.tsv` still points at AMB-1's site)
- [ ] With everything at maximum the mix drives the output limiter (peak 0.98): fine as a limit, but
      the levels could leave more room

- [ ] `tools/gen_manual_figures.py` and `tools/make_pages.py` still make AMB-1's panel figure and site
- [ ] Decisions on behaviour (each works as the manual says now):
  - Bass Octave -1 is C1..B1, not the E1 band an octave down (too low for most speakers)
  - Fifth does nothing with the Drone groove (its only note is the bar's first, always the root)
  - In a Break all three drum rings fade, though the open hat may still play (Motion under 50)
  - At Evolve 0 a stab still drops out ~6% of the time, and a phrase can end in a fill at Motion 0
  - The kick is a fixed ~49 Hz (it follows Tune, not the key)
  - Pentatonic pieces (Concrete) have three-note chords in fourths (7sus4 sounds), not 7ths

## Ideas for later

- A rim or clap on 2 and 4 (a fourth drum voice; the screen has room for three rings now)
- A spring reverb next to the plate
- MIDI CC out for the filter and the echo send, to drive other gear
- Keys as live mutes, or a held key as a manual throw
