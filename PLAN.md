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

- [x] The grid on screen while REC is held; the keys lit by group; said whole to a screen reader
- [x] Each effect, eased in and out (Stutter borrows the Echoplex's buffer; Gate lets the chord ring on to be
      chopped; Freeze brings the frozen cloud forward). CPU (host): one filter effect +22%, all sixteen +55%
      (in Night Drive on the device that could reach ~100%: the effects are momentary, but worth watching)
- [x] PERFORM keeps the mix and mutes; the grid is the effects (`tests/scenarios/effects.amb`)
- Found on the way: at Cloud 30 with Reverb 35, the cloud was all but silent (its input came after the
  Reverb knob): it now listens as if Reverb were at least 63

## From the user's testing (2026-10-16)

- [x] PERFORM's Mix knobs in the layers' colours (chord teal, pings blue, bass brass, drums amber)
- [x] The Drums knob follows the Kick and Hats keys (0 when both are out, 100 when one comes back)
- [x] A muted layer (or one at level 0) leaves its lane at once
- [x] The stabs' lane shows the echo's repeats as rings, flashing as the playhead reaches them (made
      brighter, with a white flash: "they don't blink" on the device)
- [x] **A launch screen** (asked: "fancier", then "the idea of the echoes", the voices' colours): ~1.4 s
      while the FM-1 starts, a delay throw: the name struck, then four repeats dropping out of it, smaller and
      fainter, in the layers' colours, each with its ring; the line under it, the version (`ui_splash`; the
      simulator's `splash N` draws a frame)
- [ ] Idea: the launch heard too: a stab thrown into the echo as the screen plays, its repeats in time with
      the drawn ones
- [x] The FM pages always open on the third press; on a voice that isn't FM they say so and name the FM
      voices (ALGORITHM there picks one)
- [x] **Battery:** an icon on every page (three bars), a rough % on Setup with the raw reading. The
      thresholds are dxsloop's (591 / 561 / 531 on the divider's raw reading, PB1, ADC ch 3); on USB the
      bars fill in turn (no charger line). The % (521..601) is an estimate: to check against the
      FM-1's real readings, full and low
- [x] **The Chords page** (the user's design, in place of option 2): EDIT tapped, eight slots; KNOB 1 the
      slot, the keys its root, KNOB 2 its kind (Empty, Major, Minor, Sus2, Sus4, Dim, Aug), KNOB 3 its type
      (Triad, 6, 7, Maj7, 9, 11, Add9), KNOB 4 Play (Random / Prog); an empty slot between two holds the
      one before; EDIT held: the next chord. Project format 6 (`tests/scenarios/chords.amb`)
- [x] (replaced) **Editing the chords** (option 2): EDIT held + keys, a progression of up to eight chords (the keys'
      notes in the scale), played in order every Drift bars (with Hold, on EDIT's tap); held 1.5 s
      alone, random again; "1/4" under the chord; kept by the piece (project format 5;
      `tests/scenarios/chords.amb`)
- [ ] **Idea: the chord's type** (option 3): per piece (or per chord of a progression), the chord
      built as an open minor 7 (now), a 9 without its fifth (now), a sus4, a minor 11, a major 7

## Done: the drums' pattern page and a rim (2026-10-16)

- [x] A fourth drum voice, the **rim** (two tones, ~520 Hz and ~1.7 kHz, a click of noise, mostly into the
      echo): Auto plays it on two and four once Hits is 70 or more; its own lane on the main screen (pink)
- [x] The **Pattern page** (SEQ a third time): KNOB 1 the lane (Kick, Shaker, Open, Rim), the white keys
      its sixteen steps, a black key held + a white one an accent, KNOB 2 Accent, KNOB 3 the lane's Chance,
      KNOB 4 Play (Auto / Pattern); the first edit takes Auto's pattern over. Project format 7
      (`tests/scenarios/pattern.amb`)
- [x] Found on the way: with 103 parameters, the UI's own knob codes (from 100) collided with real ones
      (Play was "no knob"; Lights would have set the rim's chance): moved to 200, with a compile-time check
- [x] A fifth drum lane, the **clap** (three bursts and a tail, into the plate): rare: Auto ~5% on two and four
      (3% + 5% x Evolve, in Full and Groove); on the Pattern page its Chance starts at 5% (was 12%:
      "too conspicuous"). Hits that only
      sometimes come are rings on the screen, filled when they play. Format 8
- [x] The effects grid takes the whole screen while REC is held; the knobs rest
- [ ] **Later: pattern styles** (the user's idea 2): a short list of classic dub techno grooves (steady,
      Steppers, Half-time, Broken, Shuffle with rim) to load into the Pattern page as a start

## Release 1.0 (2026-10-16)

- [x] Repository: github.com/sbegueria/fm1-dub (`origin`; AMB-1 kept as `upstream`)
- [x] Release identity FM-1_8XXYYZZ (AMB-1 is 7..., X0X 9...): 1.0 is FM-1_8010000; packages `dub-X.Y.fwsc`
- [x] The site (`tools/make_pages.py`, `tools/publish_pages.sh`): landing, emulator, web installer, manual, the
      words files, the firmware; at https://sbegueria.github.io/fm1-dub/
- [x] The screen reader's words link: https://sbegueria.github.io/fm1-dub/words/
- [ ] GitHub Pages enabled on the gh-pages branch (repository Settings -> Pages), a GitHub Release v1.0 with
      dub-1.0.fwsc attached
- [ ] The phone/desktop web app (`web/app`) is still AMB-1's: not on the site; to port later
- [ ] Announce (later): Elektronauts, MatrixSynth, the FM-1 groups

## Done: a bolder screen, and the middle three ways (2026-10-16, after 1.0)

The user found the screen dim and its lines and features too small. They also felt the chord and
the ripples took too much of the middle.
- [x] Brighter greys and tints, ripples two pixels wide (a throw three), lane dots 6 px with
      rings to match, level and duck bars 8 px
- [x] The ping loops orbit the ripples: a dot each, a step at a time, lit as they play
- [x] HOME again on the Dub page: ripples, scope, VU meters (`proj.screen`, saved; said "SCREEN ...").
      The engine exports `en_scope` (256 samples, each the mean of 8), `en_vu` (smoothed RMS) and
      `en_pk` (held peaks). The scope triggers on a rising zero crossing, shows a phosphor of two
      traces and sets its own gain. The VUs put 0 VU at -24 dBFS RMS, with a peak light past -5 dBFS.
- [x] Test: `tests/scenarios/screen.amb`; screenshots from `tests/listen/shots.amb`

## Study: a sixth lane, one sample a piece (not started)

The user's idea: each piece has one sample of 1-4 seconds. It is thrown very sparsely by the
arrangement, or played only by hand (from PERFORM, a key). For now this is a study of where it
would fit.

- **RAM can't hold it.** RAM is 74.7 of 96 KB used and the pool 332 of 344 KB, which leaves about
  20 + 12 KB. One second of 16-bit audio at 44.1 kHz is 88 KB. The sample has to play straight
  from the SPI flash, which is memory-mapped (XIP at 0x02000000). Reading 2 bytes per output
  sample is little bandwidth; the cost is cache misses, to measure on the device.
- **Flash** is 1 MB. The app region (`app.ld` XIP) runs to about 0x8E000, and the image uses about
  170 KB of it. The calibration sits at 0xDC000, the update loader's staging area at
  0xE0000-0xFBFFF and the project at 0xFC000. That leaves roughly 0x2C000-0xDBFFF, about 700 KB.
  Some of it is the app's room to grow, so take about 300 KB for samples: from 0x90000 up to the
  calibration. All of this must be checked against the loader (what an update erases, and where
  it stages) before anything is written there.
- **Sizes, for 4 s mono:**
  - 16-bit at 22.05 kHz: 176 KB. Too big for eight.
  - 8-bit mu-law at 16 kHz: 64 KB.
  - 4-bit IMA ADPCM at 22.05 kHz: 44 KB. Eight pieces fit in about 350 KB; at 3 s each, 265 KB.
  - So: one shared bank of about 300 KB, with each piece pointing at a sample, or eight slots of
    about 36 KB (about 3 s of ADPCM each).
- **The engine side is small.** A voice with a position, a pitch (the key, or the root), a
  gain and a band-pass costs a few hundred bytes of RAM. ADPCM decoding is a few operations per
  sample, so about 1% CPU. The sample would go into the echo like a throw.
- **Getting a sample in:** built into the image (a fixed bank, the simplest), or uploaded from the
  web page over USB MIDI SysEx. The loader-style A/B scheme isn't needed: a torn sample only
  sounds wrong, but it needs a CRC check before it plays.
- **First steps:**
  - read the loader's flash use;
  - time XIP reads in the audio callback;
  - try one built-in ADPCM sample on a key in PERFORM.

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
- A spring reverb next to the plate
- MIDI CC out for the filter and the echo send, to drive other gear
- Keys as live mutes, or a held key as a manual throw
