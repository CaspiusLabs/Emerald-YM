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
