# Bluetooth SDK integration audit — development 1.0.7

Offline audit on 2026-10-31, continuing draft PR #9 after merging main
`d16ce4a534096aec4f0112cb30d80418792ef77c`. Production sources and build tools
match that main exactly. Stable installed/released firmware remains **1.0.4**.
No device, USB/MIDI port or radio was accessed. No loader or installable package
was built, no release/tag changed, and Bluetooth remains excluded from production.

## Result

**The isolated reference SDK links, but there is still no combined FM-1 firmware
link or working headphone transmitter.** Current production compilation passes
its existing placement, flash-off/no-call, no-soft-double, image, MMIO and pool
checks. Queue desktop tests pass. SDK default compilation succeeds but strict
linking fails on `wifi_calibration_param`; explicit reference RF linking passes
with zero undefined symbols, all eleven required roots and the SBC-only range.
Reference calibration belongs to the pinned SDK demo board, not validated FM-1
data. SDK commit remains `d179b4484759423312073f5fbb232501aa491047`.

## Current memory budget

These are occupied address spans, including linker alignment gaps. The older
0.8.1 tables sum section sizes; their reports remain unchanged. In this SDK run,
RAMTEXT and RAM spans each contain three more bytes than their section sums.

| Region | Current production 1.0.4 | Isolated SDK reference | Conservative sum | Capacity | Excess |
|---|---:|---:|---:|---:|---:|
| RAMTEXT | 24,508 | 10,737 | 35,245 | 24,576 | **10,669** |
| RAM | 40,320 | 68,650 | 108,970 | 98,304 | **10,666** |
| POOL static | 101,888 | 0 | 101,888 | 344,064 | 0 |

Production has **68 B RAMTEXT**, 57,984 B RAM and 242,176 B pool headroom.
Its image is 113,584 B. SDK XIP sections occupy 218,648 B; packaged flash size
cannot be inferred by adding this to a package containing a loader and metadata.

This sum is a screening budget, not a minimum possible cost or a combined link:
platform services may be replaced rather than duplicated, and a combined linker
can change GC/alignment. It nevertheless rules out simply appending this SDK
layout to the current production layout. Pool headroom is not executable space
or an automatically available RTOS heap. The production builder also requires
at least 8 KiB pool reserve; existing stacks, debug reserve, guards and regions
must remain protected.

The task policy describes another **29,440 B** of possible task stacks/queues,
excluding TCBs. The encoder requests a 2,048 B output buffer; codec/server/crypto
heap peaks are unmeasured. These are not added as measured runtime consumption.
The static reference already contains the prototype queues; do not count them
twice. Encoder queues alone are not the Bluetooth footprint.

## SDK integration findings and gates

| Area | Source evidence | Required integration before any FM-1 candidate |
|---|---|---|
| Startup and scheduler | Emerald `firmware/crt0.S` installs its stacks and enters `fm1_cstart`; `firmware/src/ym/main.c` clears/copies regions and enters a bare-metal loop. SDK `apps/common/system/init.c` uses `os_init`, `task_create`, initcall tables and `os_start`. | Choose a single startup/scheduler owner. Audit RTOS context-save/interrupt ABI and main-loop scheduling. Calling SDK initialization from the current loop is not a port. Link roots do not prove boot-time initialization. |
| IRQ and timing | `firmware/hal/fm1_irq.h` owns all 128 vectors. TIMER4 supplies timestamps; TIMER5 runs input/USB at 10 kHz; ALNK0 owns audio DMA. `source-only/platform_policy.c` has only the demo auto-assignment sentinel. | Produce a shared IRQ/timer/priority map and bounded nesting plan. Keep USB MIDI/Clock/status and UAC service guarantees; do not replace vector initialization blindly. SDK resource ownership has not been reconciled. |
| Clock, RF and board | SDK shared RF code requires `wifi_calibration_param` even without Wi-Fi networking. Current audit imports no FM-1 RF definition or demo GPIO/power startup. | Establish documented FM-1 RF/crystal/PA data and clock/power ownership. Demo RF values are offline only; no guessing calibration or enabling demo board startup. |
| RAM code and flash-off execution | Emerald copies `.ram_text` and `.audio_hot` explicitly; SDK audit has `.sdk_ram_code` and additional orphan constant sections. SDK VM/cache services are not production flash-driver substitutes. | Resolve executable-memory pressure, complete placement/copy/cache/PC-guard analysis and far calls. Reconcile flash critical sections with SDK interrupt activity. Never allow flash-off code to call XIP or consume the debug reserve. No limit/85% threshold changes. |
| Allocator and task storage | Emerald uses a fixed 12 KiB emulator arena; SDK uses OS/server allocations and a reference heap range in POOL. | Define exclusive heap ownership, static/dynamic stack accounting and failure handling; measure peak allocations later. Do not overlay existing UI, echo or DMA storage. |
| Pairing and flash layout | SDK `user_cfg.c` reads/writes MAC and RF configuration through syscfg and uses demo VM defaults of 64 KiB. Emerald `storage.c` uses existing A/B project and panel sectors; OTA owns additional regions. | Inventory actual SDK VM accesses, MAC identity source and pairing records, then define an approved isolated storage mapping. Demo VM defaults are not an FM-1 partition. Do not call SDK storage/update code on hardware yet. |
| SBC negotiation and lifecycle | `sdk_encoder.c` opens a virtual 44.1 kHz stereo source with SDK defaults. SDK example `bt_decode.c` has `a2dp_sbc_encoder_init`, negotiated fields and `stack_run_loop_resume`. Current adapter is not connected to those events. | Implement a strong negotiation/init/close adapter, validated codec/bitpool/frame bounds, complete-frame transport and wakeup. Define disconnect/reset quiescence before clearing queues. Weak callbacks may vanish with LTO; successful linking proves no playback. |
| Audio routing and throughput | Current output forks master mix to UAC before physical USB ONLY fade. Prototype queues are one-core SPSC, nonblocking and absent from production rendering. | Add only a bounded PCM tap at that fork after other gates pass; encode/transport outside the audio IRQ. Handle drift, backpressure and underrun. Establish core ownership; present queues are not approved for cross-core/DMA sharing. |
| Compiler/runtime rules | Reference retains 15 soft-double helpers; production passes no-soft-double. Source SDK code uses large-program calls/LTO; production uses its existing far-call rules. | Identify retained double call sites and remove/adapt them before the production gate; verify ABI, calls and linker maps in a real combined build. Link success alone does not pass production guards. |

The pinned SDK examples/headers and local production sources are the evidence
for this source audit. No claim is made about current external documentation,
SDK redistribution rights, actual headset compatibility, SBC bitstream validity,
latency, RF quality, whole-system CPU usage or long-session stability.

## Recommended next implementation gate

1. Design the single scheduler/IRQ/clock and flash-storage ownership plan; resolve
   RF provenance and license obligations from the pinned SDK before distribution.
2. Reduce/replace duplicated platform services and prove a feasible combined
   static map with explicit heap/stack reserves. Do not fill gaps with success
   stubs, enlarge protected regions or weaken guards.
3. Connect negotiated SBC lifecycle/wakeup and validate actual encoded frames
   and bounded queue behavior, including disconnect/reconnect handling.
4. Only after a combined build passes all original checks, propose a separately
   authorized hardware test. Current audio CPU measurements do not measure spare
   whole-system cycles for SBC/radio/RTOS.

## Reproduction and evidence

From repository root with the provisioned pinned SDK/toolchain:

```powershell
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --reference-rf
.runtime/python/python.exe prototypes/bluetooth-audio/audit_current.py
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

The first two scripts exit zero when the audit completes; inspect
`strict_link_passed` for link outcome. Their historical 0.7.0 comparison fields
remain explicitly historical; `audit_current.py` supplies the current comparison.
It compiles the production app with stable **1.0.4** identity, runs production
checks, and emits only ignored raw app/ELF/report files. It never invokes loader
building or packaging. SDK libraries are neither edited nor redistributed.

- [Production report](production-report.json.gz), [build log](production-build.txt.gz).
- [Default SDK report](sdk-default-report.json.gz), [link failure](sdk-default-link.txt.gz).
- [Reference SDK report](sdk-reference-report.json.gz), [link log](sdk-reference-link.txt.gz).
- [Desktop queue test output](queue-tests.txt.gz).
- [Uncompressed evidence hashes](manifest.json).

The production raw app SHA-256 is
`d9d84710a58c08d9122027a89c9d21f1b852935b0f84d406d6108ed235e84150`.
This is an offline application build, not a new tested package. The preserved
stable `.fwsc` remains SHA-256
`f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`.
Historical 0.8.x failures/reports and stable 1.0.4's earlier unexplained MIDI Stop
remain intact. No full audio/UI suite was repeated: production source is unchanged;
this task reran app build/guards, SDK link audits and prototype queue tests.
