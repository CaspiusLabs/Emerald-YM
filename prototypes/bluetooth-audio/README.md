# Bluetooth headphone audio prototype — development 0.8.0

**Status: target-compiled components, incomplete A2DP integration. Not installable.** No FM-1 access, flashing, pairing, microphone recording or radio transmission was performed. Installed/released Emerald YM remains exact **0.7.0**. VERSION 0.8.0 identifies this development work, not a new tested hardware image.

This directory is excluded from the production firmware and its build. Do not run the normal firmware packaging command expecting Bluetooth: it does not include this prototype. No .fwsc, release or tag is produced here.

## What is implemented

- A bounded 2,048-frame stereo PCM16 queue (8,208 B including cursors/counters), with signed 24-bit master-mix conversion and saturation. The producer never waits; overflow drops new frames. The reader pads underruns with silence and counts them. Capacity is about 46.4 ms at 44.1 kHz, not total Bluetooth latency.
- A bounded eight-slot encoded-frame queue (4,140 B), with complete-frame admission/dropping, capacity checks and the SDK's `a2dp_sbc_encoder_get_data` pull callback. A packet contains only whole frames of one frame size. The 512-byte slot limit must be checked against the eventually negotiated SBC profile.
- An adapter using the **actual SDK headers**: `server_open("audio_server", "enc")`, format `sbc`, source `virtual`, stereo 44.1 kHz, PCM input callback and nonblocking output callback. It never opens the microphone. It uses SDK default codec settings for compilation only; no codec settings are advertised to headphones.
- Separate target compilation, real SDK archive linking and ELF section accounting, plus desktop functional queue tests. The probe makes unresolved runtime dependencies visible instead of replacing them with stubs.

Future routing would copy the same master mix as USB, **before** the USB ONLY physical-output fade. The renderer would only publish bounded PCM; SBC encoding and Bluetooth work must run outside the audio interrupt. Nothing calls these functions from the current renderer.

## What is missing before headphones can work

This is not a working transmitter. The adapter does not initialize a Bluetooth role, scan, pair or connect. A2DP codec negotiation/init/close callbacks and media-start/stop events must be connected to the SDK; the virtual encoder's exact SBC format/bitpool/channel mode must match the negotiated configuration. The stack needs a tested wake/resume path when a frame arrives. Neither the packet callback alone nor opening an encoder starts radio playback.

The SDK requires tasks, timers, heap/server registration, controller initialization, interrupt ownership, RF clocks/calibration, board configuration and linker registration tables. Emerald currently uses its own bare-metal platform. The SDK's complete runtime cannot simply be treated as an ordinary codec library. Do not resolve hardware/configuration symbols with success stubs or import SDK board defaults into FM-1 blindly.

The queues are SPSC **on one core**, with naturally aligned 32-bit cursors and SDK-style `csync` publication plus compiler memory barriers. They are not approved for cross-core or DMA-shared memory. The pi32v2 compiler's generic atomics introduced `__sync` helper calls in an earlier probe, so they are not used. Final target disassembly is archived. Desktop tests are single-threaded and do not prove target concurrency or IRQ timing. Quiesce producer, reader and transport before reset/open/close; statistics are read only when their writer is quiescent. Scheduler pacing, underrun recovery, long-term producer/consumer clock drift and transport backpressure need physical validation later.

## Reproduce without a device

Use the existing [Windows build prerequisites](../../BUILDING.md). Provision the official SDK separately, pinned to **d179b4484759423312073f5fbb232501aa491047**, tag `AC79NN_SDK_V1.2.1_2023-12-13`. Headers/libraries are not redistributed. From the repository root:

```powershell
git clone --depth 1 --filter=blob:none --sparse --branch AC79NN_SDK_V1.2.1_2023-12-13 https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git .runtime/ac79-bt-sdk
git -C .runtime/ac79-bt-sdk sparse-checkout set include_lib cpu/wl82/liba
.runtime/python/python.exe prototypes/bluetooth-audio/build_probe.py
.runtime/python/python.exe prototypes/bluetooth-audio/build_probe.py --with-runtime
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

The build probe checks the SDK commit, compiles with the real JieLi pi32v2 r3 compiler and links the real LLVM SDK archives using `LLVMgold.dll`. It forces the SBC subdevice registration and audio server, rather than trusting a string-based `server_open` to pull the codec from an archive. SDK hashes and per-section sizes are recorded in JSON. It never fetches dependencies, opens devices or packages firmware. `--sdk` and `--toolchain` accept alternative provisioned paths.

Default outputs are under ignored `build/bt-prototype/`; the extended runtime probe writes `build/bt-prototype/runtime/`. Exit code zero means the **object/dependency audit completed**. Consult `strict_link_passed` in its report: it is **false** in the archived runs. A relocatable object permits unresolved symbols; it is not a successful firmware link. The strict executable attempt records its failure and removes any candidate output. Do not load these objects on the device.

## Compilation and memory results

See [archived reports and interpretation](evidence/README.md). Target compilation passes for all three C units with warnings treated as errors. PCM queue target code has no external calls. Desktop tests pass for stereo scaling/clipping, underrun silence, drop behavior, 10,000 production-sized blocks, cursor/ring wrap, and SBC whole-frame/capacity/size-change handling. These tests use synthetic frame bytes; they do not encode or validate SBC audio.

The small adapter/queues cost **12,352 B writable storage** before SDK allocations: PCM 8,208 B + encoded slots 4,140 B + encoder pointer 4 B. This is not the total Bluetooth cost. The virtual encoder requests another 2,048-byte output buffer; codec/server allocations, task stacks and controller buffers are additional.

The exact 0.7.0 baseline has data+BSS 39,120/98,304 B (59,184 B spare), pool 101,888/344,064 B (242,176 B spare), and RAMTEXT 24,492/24,576 B (**84 B spare**). See [0.7.0 build evidence](../../docs/verification/usb-only-0.7.0/README.md). Pool capacity is not automatically interchangeable with task/data/executable memory.

Both SDK audit profiles leave unresolved symbols and include unconfigured/unused features; their section totals are **not a final layout, a minimum A2DP requirement or proof that an optimized implementation cannot fit**. The extended runtime footprint already exceeds the free ordinary data RAM before any final placement. Supporting A2DP will require a deliberately reduced classic-source profile and a platform integration plan, followed by a real final link/map. No existing memory limit or guard was changed for these probes.

The 0.7.0 hardware maximum of 73% measures the **audio interrupt**, not total CPU. It cannot establish SBC/radio headroom or coexistence with USB audio/MIDI/status. The 85% overload guard remains unchanged. No Bluetooth load, latency, audio quality, power, pairing or RF coexistence result is available.

## Next development gate

Before proposing an installable candidate: select only classic A2DP source/SBC, audit the needed SDK runtime and licensing, resolve board/configuration symbols from evidence, plan interrupt/task/clock ownership, preserve the flash-off XIP rules and startup copies, and produce a final link with measured stacks/heap/RF buffers. Then test actual SBC output and negotiated frame/bitrate behavior. Only a later explicitly authorized hardware task can establish simultaneous YM+PSG+USB+MIDI+diagnostics timing and headphone playback. Keep 0.7.0 available unchanged.

## Primary references

- [Official pinned SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/tree/AC79NN_SDK_V1.2.1_2023-12-13), especially `include_lib/server/audio_server.h`, `apps/common/example/bluetooth/edr/bt_music/bt_decode.c` and `bt_emitter.c`.
- [JieLi Bluetooth emitter guide](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/module_example/bluetooth/bt_emitter.html): classic emitter role, search/connection and transmitter configuration.
- [JieLi encoder guide](https://doc.zh-jieli.com/AC79/zh-cn/master/module_example/audio/enc.html): SBC and virtual PCM input/output callbacks. The implementation compiles against the pinned SDK; the moving documentation is context, not a replacement for its headers.
