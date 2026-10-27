# Official M-VAVE SDK feasibility assessment

Historical assessment performed 2026-10-26, translated and summarized for the repository. This is **not the active platform**. Examined FM1 Open V14 at commit[4c7496daac04a95ff10d1afdcc8f9ead2dbabc41](https://github.com/m-vave/fm1_open/tree/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41). No MIDI/USB was opened or prototype flashed; accepted Emerald 0.1.4 was unchanged.

## Finding

Replacing the source audio application's renderer with YM2612 compiled/linked and had sufficient static RAM in the probe, but **real-time performance was not verified**. Adding it to the full stock application exceeded its flash application partition. This is a porting direction, not a working release.

The source [audio IRQ](https://github.com/m-vave/fm1_open/blob/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41/app/board/audio.c) calls fm1_engine_render then fm1_output_mix and copies to DMA. Replacing the first call is possible. fm1_block.h specifies nominal 44100 Hz/128 stereo frames, while fm1_audio.h gives 44118 Hz physical clock. Interleaved int 32 signed Q23 matches Emerald's variable-block output. This requires replacing/rebuilding the app, not installing a plugin into unchanged stock firmware. sealed_audio_api does not remove source-IRQ access. Audio and needed tables/functions must fit SRAM via adapted .volatile_ram_code/.volatile_ram.fm1 linker/startup handling.

## Static probe

Pinned source archive SHA-256 aeb20247865b5e068671fd2bcada9a9eae96f50b5fdc782d7d4e1454b6c3ae4b; local JieLi Windows 2.5.3,pi32v2 r3. Ignored working evidence: build/official-sdk-audit/assessment.json; these files are not shipped as a reproducible prototype.

| Measurement | Stock app | YM2612 probe |
|---|---:|---:|
| Application bytes |453576|472872|
| Linker heap reserve before SDK startup allocations |247340|212844|
| SDK linker/SRAM/ABI checks |passed|passed|
| Stock-layout packaging |passed|failed|

Image increase 19296 B; RAM increase 34496 B. The minimum SDK heap reserve 49152 B was met statically, not measured after Bluetooth/USB/RTOS startup. Four unchanged Emerald C++ units were compiled, code/tables placed in SRAM, ym_init inserted before audio and IRQ renderer replaced. SDK binary libraries/UI stayed. Address checks extended rather than disabled; source manifest reflected changes. PLAY/panel/Emerald UI were not connected; stock synth initialized but was not rendered. No release identity, Emerald 85% guard or hardware acceptance. **Do not flash this probe.**

Retained global symbols were checked for SRAM placement, not a complete flash-off call graph. New objects added no soft-double, but the original SDK already includes such routines; the whole SDK does not pass Emerald's global no-soft-double claim. Separate desktop 60 s block 256/128 renders matched hash 6e282fd6, seed 20261026, arena 2304 B, peak 6116278 Q23, with pause/extremes/reseed/no-allocation checks. PC timing is not an FM-1 benchmark.

## Flash and timing limits

Package.cs reserves 536576 B user data,32768 B VM,4096 B BTIF and other resources/final sector, leaving 453921 B maximum app. Probe exceeds it by 18951 B and correctly fails packaging. No partition/check bypass. A real port must remove unused stock code or verify a redesigned layout against libraries/storage/updating/recovery.

At44118Hz,128 frames allow about 2901 us; Emerald 85% would allow 2466 us. Halving the earlier 0.1.4 maximum 4653 us suggests 2327 us, only about 140 us margin. This is illustrative, not a prediction: event cost is nonlinear, IRQ frequency doubles, RTOS/transports/priorities and CPU-clock parity are unverified. SDK 87.5% DMA commit boundary is not Emerald's eight-block 85% protection and cannot substitute for it.

A future separate task would first fit a minimal app, retain five FM+DAC/RAM/no-allocation/85% protection, audit startup/calls/stacks, then measure 128-frame blocks on hardware under UI/USB/Bluetooth load. There is no evidence that official SDK is faster. The active project stayed on the existing platform; SDK probe is not an active dependency. Baseline package SHA-256 5fcae705878056c52588b4af9a98902a556c26488926a98d0cbcd7e6e79bd52a unchanged.
