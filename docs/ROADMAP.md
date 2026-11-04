# Emerald YM — RAMTEXT-aware development roadmap

Updated 2026-11-04. Baseline: **public stable firmware v1.0.12**,
`FM-1_68010012`, with 24,544 / 24,576 bytes used in guarded RAMTEXT
(**32 bytes free**). Development VERSION 1.0.18 adds host-only memory
diagnostics, not a new tested or installable firmware image.

The FM-1 already runs five YM2612 FM voices, DAC drums, a three-voice PSG
with noise, 12-source level/FX control, seed-driven 88-bar arrangements,
endless generation, live keyboard, USB Audio and USB/TRS MIDI functions.
We should improve **creative control** before adding another synthesis engine.

## Memory-first design rules

1. **Do not spend the last 32 bytes as a feature budget.** The next compiler
   or minor bug fix may exceed RAMTEXT. No new ISR/sample-loop or audio
   hot-path feature until we recover meaningful space.
2. **First milestone: reclaim at least 1 KiB of guarded RAMTEXT; target
   2 KiB** while preserving the current fixed memory map, startup copy,
   cross-section call constraints and the 85% CPU safety threshold. These
   are engineering goals, not established available capacity.
3. Prefer **main-loop/XIP control code, compile-time constant tables,
   small bitmasks and reuse of existing settings/FX/mixer/transport**.
   Small app-state additions still require linker checks; XIP is not safe
   inside flash-off audio paths. Never move unsafe tick code to XIP just
   to bypass the size guard.
4. Every new firmware candidate must pass the local Windows/JieLi build,
   RAMTEXT link guard, full desktop suite, deterministic same-seed PCM,
   different-seed regression, unchanged Endless golden, bounded peak
   audio-block time, note release and MIDI transport cleanup.
5. Publish documentation/GUI simulations without flashing automatically.
   FM-1 installation and electrical/USB tests require separate scoped
   authorization; the exact installed image and results must be archived.

## Near-term ideas — maximize music with existing synthesis

| Priority | Proposed feature | Musician benefit | Low-memory implementation hypothesis | Main risk |
|---|---|---|---|---|
| P1 | **Performance scenes A/B** | Instantly switch between two arrangements while jamming | Snapshot existing 12 mixer levels and selected FX at main-loop control rate; avoid extra audio buffers | UI/flash settings migration, click-free transitions |
| P1 | **Live fill / break trigger** | Build-ups and temporary dropouts on demand | Schedule a mask/density override at the next bar using existing song-form control logic; no new samples | Precise control timing and incoming/outgoing MIDI consistency |
| P1 | **Seed favorites and A/B comparison** | Save compelling generated songs and compare versions | Store a small number of seed integers; regenerate from the seed instead of storing patterns or audio | Persistence schema and write safety |
| P2 | **Style presets: CHiP, ACID, DUB, AMBIENT** | Quickly explore different musical moods | Reuse existing tempo, mutation, source levels and DECAY/COLOR/ECHO; presets are control data, not new DSP | Avoid overstating true analog filter/reverb capabilities |
| P2 | **Musical overview on the 240×240 screen** | Display seed, current phrase, chord/root and fill state | Existing changed-only UI tiles and compact control snapshots | Layout readability, stale redraw, framebuffer limits |
| P2 | **External performance MIDI mapping** | Control scene/fill/seed from a MIDI controller | Add bounded USB/TRS CC or note mappings in the existing command path; no MIDI Thru by default | Feedback, status collision and stuck notes |
| P2 | **Nonstop session / Auto Next Song** | After END, start a new seed when enabled | Main-loop transport state machine, explicit opt-in; maintain predictable Start/Stop semantics | Accidental reseed and external clock/DAW confusion |
| P3 | **Song-form templates** | Alternative 64/96/128-bar structures and phrase arcs | Compact static phrase descriptors, regenerate all notes on demand | New timing/transport regressions; potential tick-code expansion |
| P3 | **Expanded sound palette** | New operator patches or additional percussion feel | Only after RAMTEXT optimization and measured CPU headroom; use current voice count | Extra synthesis code and real-time DSP cost |

### What can be tried first?

**Performance scenes, seed favorites and simple on-screen chord/seed information**
are the best initial candidates because they can mostly reuse controls already
implemented. This is a feasibility hypothesis, **not a guarantee that they will
fit with only 32 bytes of RAMTEXT free**. Test the linked firmware before
claiming a no-cost feature.

A **live fill** could be the most musically valuable immediate control,
provided a tempo-synchronized state change can be implemented through the
existing main-loop song profile and outgoing source mask without introducing
new sample-loop decisions.

For styles, start with **preset macros** rather than attempting a new acid
resonant filter, convolution/long reverbs, sample upload, or Bluetooth audio.

## Recommended work packages

### Milestone A — RAMTEXT and baseline protection

- Build exact v1.0.12 locally with the documented JieLi toolchain and inspect
  the full linker map. Record which functions and constants occupy guarded RAM.
- Identify duplicate note/step operations, unnecessary inlining and
  constant placement opportunities, **without changing audible output**.
- Prove old Endless PCM signatures, current song output/transport and target
  audio load remain safe; record reclaimed bytes. Stop if the measured build
  exceeds the unchanged 24,576-byte limit.

**Exit criterion:** target build/tests pass, output deterministic,
0 XRUN/overload in controlled target validation when authorized, and preferably
at least 1,024 verified bytes of reclaimed RAMTEXT.

### Milestone A progress — profiling tooling (1.0.18)

- Windows builds now preserve the linker symbol table as `build/amb.syms`
  and generate `build/ramtext-report.json` with guarded RAMTEXT totals,
  largest named functions and flash-driver/FX sizes.
- Parser regressions exercise section accounting, memory limits and the
  1 KiB goal without accessing USB or modifying the firmware.
- **Not yet achieved:** compile with the JieLi Windows toolchain, inspect
  the real linked symbol report, implement targeted reductions and confirm
  original MIDI/PCM/CPU behavior. No bytes have yet been measured reclaimed.
- Acceptance target: **used ≤ 23,520 B**, yielding **≥ 1,056 B free** in the
  existing 24,576 B region. Enable `--require-goal` only for a candidate
  optimization; the initial profiling stage must still report baseline builds.

### Milestone B — Performance controls

- Implement scene A/B or live fill on a separate PR, one narrowly scoped
  functional change at a time; map controls only after a physical panel UX
  review so existing button functions remain predictable.
- Add tests for rapid toggles, cross-bar transitions, USB MIDI OUT source
  masks, clocks/notes, and all 12 persisted level/FX values.
- Expose a clear screen/LED state using existing tile redraw rules.

### Milestone C — Compositional usability

- Add 4–8 favorite seeds, A/B seed replay and optional style presets after
  settings schema migration is designed and safely tested.
- Provide a quick recording workflow in the manual: **USB Audio captures
  sound; USB MIDI OUT captures performance events in the DAW**. No new
  on-device audio/MIDI file recorder is needed for this first step.
- Compare several song seeds by listening, not only by PCM/hash differences.

### Milestone D — Advanced musical structures

- Consider additional song lengths/structures, arrangement editing, externally
  controlled fills and richer timbres only after explicit RAMTEXT headroom is
  available. Keep sample rates, hardware safety controls and release/install
  compatibility intact.

## Deliberately deferred

- **Bluetooth A2DP**: the old isolated prototype exceeded RAMTEXT by 9,455
  bytes, and did not become a stable release.
- **New real-time synthesizers, long DSP effects, audio sample banks or a
  second audio engine**: the current 32-byte margin does not support an
  honest promise of feasibility.
- **Unverified browser installer, automatic Firmware Hub listing or
  interrupted-flash recovery claims**: use the separate
  [Firmware Hub checklist](FIRMWARE-HUB.md).

No item in this roadmap is a commitment that it already works on the device;
features need a new code PR and measured validation.
