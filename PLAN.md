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

- [ ] **Your ears**: listen to the pack (`build/listen/pack/`, rendered again with the cloud); what to
      change in the sound and the pieces
- [ ] The web app's own page (`web/app/index.html`, the phone/desktop app): still AMB-1's UI (the
      emulator page is done)
- [ ] **On the device**: install (`python3 tools/fm1_install.py build/amb.fwsc`), check the Setup
      page's CPU and the sound on the FM-1's speaker and on headphones
- [x] The name: DUB-1 (decided 2026-10-16)
- [ ] Publishing (the words file, a hosted emulator and installer): later, after trying it longer
      (decided 2026-10-16). Where (the `# url:` in
      `firmware/words.tsv` still points at AMB-1's site)
- [ ] With everything at maximum the mix drives the output limiter (peak 0.98): fine as a limit, but
      the levels could leave more room

- [ ] `tools/gen_manual_figures.py` and `tools/make_pages.py` still make AMB-1's panel figure and site
- [x] Decisions on behaviour (2026-10-16):
  - Bass Octave −1: a true octave down (E0..Eb1)
  - Fifth with Drone: how often a whole bar is the fifth
  - Evolve 0 (with Motion 0): a strict loop, no stab gaps, no throws, no fills
  - The kick follows the key: the root in F1..B1, below F its fifth (44..62 Hz)
  - Concrete keeps its chords in fourths (7sus4 sounds)
- [ ] **Sound design** with your ears: the stab's character, the kick, the bass, the hats, the
      echo's colour, the cloud, the crackle (a round of its own)

## Done: voices (a first approach)

- ALGORITHM on a layer's page steps through its voices; on Dub, Space, Setup and Mix it is still the
  mode. The voice shows at the left of the knob strip, is said ("VOICE Organ"), and is kept by the
  piece.
- Chord: Saws, Keys, Organ, Pad, Bell, Tonewheel (replaced Brass, too like the others), Noise. Pings: Blip, Marimba, Glass, Wood, Drop. Bass:
  Sub, Round, Growl, Pulse. Kits: Tight, Deep, Punch, Soft, Dusty.
- Each voice is a row of a table in `amb.c` (`CV`, `PV`, `BV_GAIN`, `KIT`). Levels are matched by
  measurement (within ~2 dB of the layer's first voice).
- The pieces got their own voices. The project format is now 2.
- CPU (host): Organ is the heaviest, 0.33% of real time (AMB-1's worst: 0.31%).

### Voices: feedback (2026-10-16)

- The Mix page's labels (CHORD, PINGS, BASS, DRUMS) are fine.
- [ ] The basses are too alike: something punchier, from the classic FM bass patches (later).
- [x] Brass was too like the other chord voices: now Tonewheel, a second organ.

## Next: a sound page for each layer (the user's plan)

Each layer gets a second page, its **sound page**, to shape its voice beyond the preset.

### How it works

- **Reaching it:** press the layer's button again (ENV on the Chord page: the Chord's sound page;
  ENV again: back). The page's name shows it ("CHORD · SOUND"), and its button blinks while there.
  SELECT's round of pages stays as it is (the sound pages are off it). ALGORITHM still picks the
  voice, on either page.
- **Four knobs a layer**, with the same meaning for every voice of that layer: each voice maps
  them onto its own insides (for a saw voice, "Colour" is the detune; for an FM voice, the
  modulation index; for the organ, the drawbars' balance).
- **Relative to the preset:** each knob's centre (shown as 0) is the preset as it is; turning
  moves away from it. Changing the voice with ALGORITHM keeps your turns, so a "brighter, slower"
  chord stays brighter and slower on the next voice. A press of SAVE held (the piece's reset)
  puts them back to 0, with the rest.
- **Saved with the piece**, like everything else: 16 more values a piece (4 layers x 4); the
  project grows from ~700 to ~980 bytes, under the 3840-byte limit. Project format 3.
- **The screen**, on a sound page: the main area shows the voice's shape instead of the ripples,
  its envelope drawn (attack, decay, the filter's own), redrawn as you turn. Later, for the FM
  voices, the algorithm as a diagram.
- Two values only the pieces set today get a knob here: the pings' tone and the hats' colour.

### The knobs

| Layer | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| Chord | **Attack** (a stab .. a swell) | **Colour** (detune / FM index / drawbars / noise colour) | **Resonance** (the filter's) | **Env** (how far each stab opens the filter) |
| Pings | **Tone** (round .. metallic: the index, today's Ping tone) | **Ratio** (the modulator: harmonic .. bell-like .. clangy) | **Length** | **Bend** (the pitch falling at the strike) |
| Bass | **Bite** (FM index / drive at each note) | **Shape** (sine .. square-ish) | **Length** (how long each note holds, against the groove's) | **Slide** (glide between notes) |
| Drums | **Kick tune** (semitones from the key's note) | **Kick length** | **Punch** (the kick's start pitch and click) | **Hats** (dark .. bright, today's piece value) |

### Then: a real FM core (phase 2)

- One 4-operator FM engine (sine operators, ratios, levels, feedback on one, an index
  envelope), with the classic algorithms (the eight 4-op layouts, as the FM-1's own firmware and
  the DX-style synths have them), for the chord and the pings. The FM voices (Keys, Bell, Marimba,
  Glass, Wood) become patches of it, and new ones come from the classic FM patch libraries (the
  punchier basses wanted, electric pianos, clavs, bells).
- The sound page gets a second set of four on an FM voice (the layer's button a third time):
  **Algorithm**, **Ratio** (of the modulator in focus), **Feedback**, **Index env** (its decay).
- **CPU first:** four chord voices x 4 operators is the heaviest thing DUB-1 would do. Measure on
  the device (the Setup page's CPU) before, and keep a lighter algorithm set if it's tight.

### Steps

- [x] The sound pages: the UI (the second press, the page, the help cards, the screen reader), 16
      parameters, project format 3, tests (`tests/scenarios/sound.amb`)
- [x] Each voice's mapping of the four knobs (a first pass; to tune by ear)
- [x] The envelope drawing on a sound page
- [x] The FM core (4 operators, the eight 4-op algorithms and a 2-op one, op 4's feedback, an envelope an
      operator), the FM voices moved onto it (Keys, Bell, all the pings); new basses: Punch, Rubber, Pluck
      (Round and Pulse gone), Growl
- [x] The FM page (Algo, Ratio of ops 3 and 4, Feedback, Mod dec), a third press on an FM voice; the
      algorithm drawn; project format 4; `tests/scenarios/fm.amb`
- [x] **CPU on the device** (2026-10-16, DUB-1 installed on the user's FM-1, over the stock V15 put back
      first): Night Drive shows 20..50% (smoothed). The host's 0.42% of real time is ~80x faster than the
      device. Setup now shows the peak too (now/peak over 2 s) and any dropouts (!n)
- [x] Readings (2026-10-16, now/peak, no dropouts anywhere; "sounds good"): Basic 20/50, Night Drive
      30/67, Night Drive stressed (Cloud 100, Density 100, the pings on algorithm 1) 50/72. About a
      quarter of the time is left at the worst moment: what's added must be light
- [x] Found on the way: the command queue (64) dropped commands once a piece had 62 parameters (PLAY
      among them): now 256, with a compile-time check and a test

## Done: DUB-1's own interface

- **Main screen, echo ripples:** each stab drops a ring that spreads and fades; the echo's repeats
  follow it at the echo's own time and feedback. Throws are double rings; dub hold is red and
  doesn't fade. Also the chord's name, the section and phrase, the layers' levels, a duck meter,
  and four lanes (stabs, kick, hats, bass) showing the bar as it plays. The scale ring, ping loops
  and drum rings are gone (the keys still light the scale).
- **PERFORM (SEL):** the knobs become the Mix page (chord, pings, bass, drums levels); white keys 1–5 mute the chord, pings, bass, kick and hats; 6–10, held, throw
  every stab, push the filter open or shut, drop the kick, push the echo. The keys light up for
  the layers playing and the moves held. Mutes keep the music's random choices, so a layer comes
  back where it would have been. The engine API is `en_mute`, `en_perf`.
- Scenario `tests/scenarios/perform.amb`; the manual, the README, the emulator page and the
  screenshots are updated.
- The web emulator (`web/emu`) is ported and builds with emsdk (`EMULATOR.md`); its test and the
  web app's engine test pass.

## Done: a granular reverb (CLOUD), after JIANT

After JIANT's CLOUD (github.com/juanjiant-bit/JIANT, GPL-3.0; credited in the README, the manual
and `amb.c`):
- A 0.74 s line at a quarter rate (11 kHz, 16 KB, in RAM: the pool is full), fed by the reverb send.
- Up to 6 grains of 80–250 ms (triangle windows, random place and pan, a few cents off, sometimes
  an octave down, rarely up), denser as Cloud turns up and as the filter opens.
- The grains go into the plate, and the plate's tail back into the line, so the wash grows with
  a bigger plate. More of it in a Break; dub hold freezes it.
- It has its own random numbers, so turning Cloud doesn't change the music's choices.
- **Cloud** is the Texture page's third knob; the pings' tone is now set by the piece.
- Measured:
  - in Haze the cloud is ~10 dB under the mix;
  - at everything max it stays bounded and dies once the layers stop;
  - CPU within ~3% of AMB-1's (host);
  - RAM 69.2 of 96 KB.

## Next after the FM core: an effects grid on REC (the user's idea)

Holding REC shows a grid of effects; the white keys play them while held (a tap on REC stays dub
hold). Sixteen in four groups of four, the first of each group lit:

| Group | 1 | 2 | 3 | 4 |
|---|---|---|---|---|
| Echo | Throw | Time x2 / /2 | Tape stop (the echo slowing, pitch diving) | Swell (all in, feeding back; was Reverse: the tape can't run backwards without much more memory) |
| Filter | Low-pass sweep | High-pass ("radio") | Wah (resonant sweep) | Telephone (narrow band, lo-fi) |
| Space | Reverb throw | Spring crash | Cloud freeze | Wash (echo and reverb flooding) |
| Rhythm / FX | Stutter (1/8 or 1/16 repeat) | Gate (the chord in 16ths) | Siren | Noise riser |

- [ ] The grid on screen while REC is held; the keys lit by group
- [ ] Each effect, eased in and out; a stutter needs a short buffer of the mix (memory: RAM ~28 KB free)
- [ ] PERFORM keeps the mix and mutes; the grid is the effects

## Ideas for later

- **Korg NTS-3 kaoss pad integration** (a future project) (the user's own setup; a Kaoss pad is great for dub techno).
  Send one voice to the NTS-3 and, ideally, bring it back and mix it with the rest.
  - **The NTS-3** (Korg Nu:Tekt kaoss pad kit):
    - stereo AUDIO IN and OUT (3.5 mm), SYNC IN and OUT (3.5 mm TRS), USB-C (power; MIDI over USB
      not confirmed);
    - up to four effects at once, 200 programs;
    - custom effects through Korg's logue SDK (`platform/nts-3_kaoss/`, SDK v2, Cortex-M7).
    - One retailer says an internal switch makes the sync jacks MIDI in and out (to check).
  - **The FM-1:** a stereo audio out, MIDI in over USB and the TRS jack, MIDI out over USB only, USB
    audio *to* a computer. No audio input is known.
  - **Sending one voice (doable): a "send" mode in DUB-1.** A setting (Off / Chord / Pings /
    Chord + Pings / Drums) puts that layer dry (after its filter, before DUB-1's echo) alone on the
    left channel, and the rest, with DUB-1's own echo and reverb, in mono on the right.
  - **The return, two ways:**
    1. *A mixer.* A TRS Y-splitter: left into the NTS-3; its output and the FM-1's right into a mixer.
       Works with the NTS-3's own effects; the mix happens outside.
    2. *The NTS-3 as the mixer (the elegant one).* The FM-1's whole output into the NTS-3, with a
       custom logue SDK effect that treats the left channel as the send (into the effect) and passes
       the right dry, summing both to stereo. No mixer, one cable each way, and the NTS-3's pad
       plays the send voice only.
       - To check: whether a custom unit can sit before the built-in effects in the chain (so
         Korg's own effects process only the send), or whether the custom unit has to be the dub
         effect itself, e.g. a tape echo with a band-pass in the feedback, written for the NTS-3.
    3. *Back into the FM-1:* only with an input it doesn't have: research whether the board's codec
       has an ADC and an input point (a hardware mod), or USB audio both ways through a computer
       (no longer standalone).
  - **Sync:** the NTS-3 has SYNC IN (Korg's analog sync pulses) and Tap BPM. The FM-1 has no sync
    out and sends MIDI clock over USB only. Ways to try:
    - MIDI clock over USB to the NTS-3, if its USB-C carries MIDI;
    - through a USB-MIDI host into the sync jack in its MIDI mode, if that switch exists;
    - a sync pulse on the audio (a click on the send channel's first sixteenth, which a custom
      unit could read as a clock);
    - else tap.
  - **Control from DUB-1:** if MIDI reaches the NTS-3, its X/Y and depth by CC, so the arrangement
    plays the pad: a sweep through a Break, the effect changing on a phrase.
  - First steps:
    - read `platform/nts-3_kaoss/` in the logue SDK (unit types, the chain, the I/O it gets);
    - check what the NTS-3's USB-C carries;
    - try the split mode with a Y-cable and the NTS-3's own effects.
- A rim or clap on 2 and 4 (a fourth drum voice; the screen has room for three rings now)
- A spring reverb next to the plate
- MIDI CC out for the filter and the echo send, to drive other gear
- Keys as live mutes, or a held key as a manual throw
