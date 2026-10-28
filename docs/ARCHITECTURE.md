# Project architecture

| Area | Purpose |
|---|---|
| firmware/src/ym/ym_engine.cpp | Streaming sequencer, five FM + DAC, resampling, fixed initialization arena |
| firmware/src/ym/ym_psg.h | Sega musical PSG profile, three tones/white noise, live voice allocation |
| firmware/src/ym/ym_settings.c | Main-loop settings codec, dirty/debounce policy and existing A/B project storage |
| firmware/src/ym/ym_fx.h | DECAY/COLOR/ECHO, 12×3 matrix, per-source sends to shared stereo echo |
| firmware/src/ym/ymfm | ymfm core and local restricted-profile optimizations |
| firmware/src/ym/ym_tables.h | Tuning tables and original short drum samples |
| firmware/src/ym/main.c, ym_loop.c | Startup, audio interrupt, watchdog, timing, panel and USB |
| firmware/src/ym/ym_clock.h, ym_midi.c, src/midi_input.h | USB clock follower, per-port live input, timestamped shared ISR queue |
| firmware/src/ym/ym_ui.c, ym_leds.c | Changed-tile UI and complete-row LED publication |
| firmware/hal | CPU, display, panel, DMA, flash and guards |
| firmware/app.ld | XIP, .ram_text, .audio_hot, data, pool and stacks |
| firmware/loader, src/ota.c, src/usb.c | Retained Felucca/DUB-1 update platform |
| tools/build.py, fm1pkg_make.py | Compile, check memory/image and package .fwsc |
| tests/ym_stream_test.c, ym_fx_test.cpp, ym_ui_test.c | Current desktop audio/FX/UI tests |
| releases/ | Preserved packages and separate offline/physical evidence |
| docs/history/ | Historical designs, early experiments and inherited DUB-1 docs |
| .runtime/ | Ignored, separately provisioned local tools |

The emulated YM2612 clock is 7,670,454 Hz. Native audio at clock/144 is converted to 44.1 kHz; a 256-frame stereo DMA block has about 5804 us. Eight blocks above the deliberate 85% limit after warm-up latch a stop until restart. Diagnostics cannot bypass it.

Channel six is permanently DAC. EMERALD_FIXED_DAC omits unused channel-six FM work while retaining DAC/shared clocks. This is not a general VGM/register stream player. Since 0.3.0, EMERALD_STATIC_OPN skips unused SSG-EG, operator AM and dynamic PM paths. Patches/FX keep these modes disabled. Adding them requires adapting/disabling the profile and comparing with EMERALD_GENERIC_OPN. Dry and FX reference hashes are recorded in docs/releases/v0.3.0.md.

The renderer allocates no memory. Core objects are created in a fixed arena before IRQ startup; no double/libm is used. Critical audio code/tables are in RAM. `.audio_hot.fx` is inside the original `.audio_hot`, covered by its startup copy and executable PC guard, without enlarging region limits or exposing stacks/data as code. Flash-off `.ram_text` remains independently checked for forbidden calls.

Main-loop leaf setters/getters live in XIP `.ym_control` and are reached through platform FL_FAR. They are not called by the renderer. Configuration/tuning tables in `.ym_pitch` are read at note/period setup, outside oscillator sample loops. Runtime periods and hot sample work stay in RAM.

Echo uses 32 KiB of the existing pool for a stereo int16 buffer and generation tags. It averages pairs of samples at 22.05 kHz, delays about 186 ms, uses feedback 5/16 and shared return gain. Each source has its own send, not its own delay line. Reset advances the generation rather than clearing a large buffer in the IRQ. Since 0.4.0 instrument levels/FX are persistent; 0.4.2 also stores tempo/density/mutation/transpose: a 72-byte EMS1/schema2 record including K1–K4 (56-byte schema1 migration accepted without a boot-time write), range checked, using existing OBJ_PROJ at FC000/FD000 with storage.c CRC/header-last commit and readback. No memory/flash map change. This reuses other firmware's project space; no compatibility/import is promised. Writes wait three seconds after the last edit and audio activity; SAVE pauses backing. Flash-off code and stopped-audio behavior are unchanged. There are no writes from the audio renderer.

UI uses the existing 240×144 canvas as tile workspace, not another full framebuffer. Finished tiles are sent only after state changes. The complete screen is 240×240. DMA synchronization protects canvas reuse. The Emerald mark is drawn in code.

Inherited app/dsp, host and web code belongs to DUB-1. EN_YM2612 selects the current app; the inherited pages are not a supported Emerald emulator or installer. Archived migration scripts are historical artifacts, not setup instructions.

## USB audio capture (development 0.5.0)

EN_UAC=1 selects the existing UAC1 implementation in usb.c. Each rendered master block feeds uac_tap; the existing adaptive cubic resampler bridges device/host clock drift into a 2048-frame stereo ring. EP4 IN sends 44/45 frames per millisecond at nominal 44.1 kHz; MIDI keeps EP1 IN/OUT. UAC servicing may nest in audio rendering; MIDI polling remains outside it. Tap cost and nested ISR time contribute to measured render duration; hardware timing still needs verification. No linker layout, startup copy, flash-off code, allocation policy or 85% guard changes. Status v1 is unchanged and does not export UAC-specific counters.

## Development 0.6.0 MASTER UI

The main loop copies the existing smoothed MASTER ADC value to ymaster_adc before applying the unchanged squared master_q12 gain calculation. UI position is computed from that linear 0..1023 reading in 90 steps, with visual-only hysteresis. This does not add ADC reads or render-IRQ work and does not enter the persistent settings snapshot.

The 40px left rail is drawn once as 40x136 and 40x32 static tiles. The moving track/handle uses one 22x108 tile only when the quantized position changes. Right-side transport and parameter/mixer tiles are reflowed within 240x240. Every tile fits the existing 240x144 canvas; no framebuffer or allocation is added. Initial invalidation alone may clear the full screen. Code/memory layout, startup copying, flash-off rules and the 85% guard are unchanged.

## Development 0.7.0 output routing

ym_output.h owns the output fork: generate the existing master mix (or protected silence), feed uac_tap, then fade/zero only the physical I2S/DMA buffer. A 0..256 integer local gain ramps one step per stereo frame; unity bypasses scaling bit-for-bit and a settled mute uses memset. No allocation, codec reset, stopped DMA, 64-bit sample multiply or gain-stage change before USB. The route helper remains inside render_block timing, including the existing 85% guard.

GLO changes a 32-bit volatile session flag on button edges; it does not dirty persistent settings. Complete LED-row publication adds bright GLO only while muted. A separate changed-only 40x32 tile replaces the headphone symbol with USB ONLY; MASTER and other UI tiles retain their behavior. Status v1 is unchanged and does not report USB ONLY state; physical confirmation is separate evidence.

## Development 0.9.0 MIDI scheduling

USB and UART1 producers share the existing TIMER5 context outside the renderer. The bounded input queue publishes a TIMER4 arrival timestamp alongside each complete packet, before advancing the write index. Emerald normalizes USB's source to 0 and tags TRS as 1 internally; these tags never alter the outgoing USB descriptor or wire protocol. MIDI masks are separate per input and combined with the physical keyboard. FIFO/DMA loss clears MIDI-held masks and stops the external follower rather than silently losing timing pulses.

The main-loop clock follower consumes USB real-time messages, counts 24 pulses/quarter and publishes one sequencer boundary per six pulses. A 24-interval timestamp window estimates integer BPM only for gate duration and UI; it does not drive external sequencing. On Start the seed is preserved but the renderer resets the arrangement; Continue preserves phase. Missing Clock for more than 500 ms pauses until explicit resume, without automatic internal fallback. Clock metadata is not persisted. TRS real-time/SysEx does not control the follower/updater.

The renderer's existing tempo accumulator is bypassed in external mode and consumes at most one queued boundary per block after FX synchronization, preserving bounded render work. The old native-sample gate unit division is now computed in XIP control code and read at tick time, leaving internal-mode reference PCM bit-exact. Audio code still fits in the original RAMTEXT with 16 B spare, and the new setter uses existing FL_FAR and linker address checks. USB Clock timing accuracy/USB+TRS load are not physically measured; no allocation, libm/double, region expansion, flash-off XIP call or higher overload threshold was introduced.

## Development 0.10.0 outgoing notes

EDIT selects note/FX input or USB performance output; it does not reroute the minijack electrically. OUT ignores both incoming note/CC sources, clears their held masks and discards pre-switch note/CC packets while preserving queued Clock/transport. Status v1 remains outside this gate. The mode/revision is not part of settings.

The engine increments per-source trigger serials at note/hit boundaries, plus a restart epoch. Its XIP-only ym_midi_snapshot is called by the audio owner after rendering; it copies current FM/PSG/drum note states, serials and applied transpose. The XIP publisher runs within render_block timing, after the unchanged DSP/UAC/local-output fork. It exports physical keyboard keys separately, so incoming MIDI is never echoed. Serial counters retain retriggers and sub-block drum pulses; fixed arrays and bounded loops replace event allocation. Under prolonged host backpressure intermediate events can coalesce. Accepted Offs are queued before new Ons, and rejected Offs remain pending for retry. USB SysEx retains priority over performance packets.

Only event-selection constants (motifs, roots, chord/scale/rhythm tables) move to the existing .ym_pitch section; sample-loop oscillator tables and critical FX code stay in copied RAM. Build guards check these XIP addresses, the snapshot address, unchanged region limits, .audio_hot.fx startup/PC coverage and the flash-off call prohibition. There is no map expansion or threshold change. A subsequent [physical USB MIDI/audio/status test](verification/usb-midi-out-0.10.0/hardware/README.md) measured 45.98% sampled average CPU and 72% maximum with XRUN/overload 0. It verified captured output and EDIT cleanup; separate endpoint counters, destination-synth rendering and long-session operation remain unverified.
