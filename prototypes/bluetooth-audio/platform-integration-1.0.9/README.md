# Flash/cache and RTOS port attempt — development 1.0.9

Continues draft PR #9 from [1.0.8](../lean-integration-1.0.8/README.md).
**Integration remains incomplete.** The strict combined link still fails, but
its RAMTEXT overflow decreases from **10,667 B to 9,455 B**. No hardware, USB/MIDI,
radio, flash installation, loader/package build, release/tag change or merge.
Stable installed/released **1.0.4** and production sources/tools remain unchanged.

## Implemented work

### Explicit session configuration backend

`session_store.c` implements a real fixed-capacity record store: sixteen records,
up to 256 B each, replacement by ID, full-record reads and explicit negative
errors for missing/full/invalid/short-buffer operations. No allocation or flash
backend exists in this component. Store structures consume 4,160 B in existing
non-executable POOL. Target `session_config.c` implements the actual SDK public
`syscfg_read/write/dma_write/read_string` signatures from its headers, with SDK
task-mutex serialization. These calls must never run in an ISR/critical section.

Initialization requires the SDK RTOS to be ready and a caller-provided valid
unicast MAC; there is no generated identity or unknown flash-sector read. Calls
fail until initialization, and the provisioned MAC cannot be overwritten through
the public write wrapper. Initialize once, quiescent, before starting consumers;
cross-core startup/publication is not validated. The generated private SDK
`user_cfg.c` uses that MAC accessor and the constant name Emerald YM FM-1 instead
of its flash-UUID fallback. The source in the SDK checkout is not modified.

This is an opt-in **syscfg session backend**, not proof that all Bluetooth
pairing/storage paths are now volatile or flash-free. The SDK still pulls its
separate `bt_vm_interface` and VM closure. Merely implementing the public syscfg
functions does not replace that private file-style interface. That distinction
is recorded as `bt_pairing_flash_access_eliminated=false` in the summary.
The backend does not preserve data across reboot, support arbitrary-size SDK
records or implement other opaque/private VM APIs. Unsupported string versions
and unaligned DMA-style writes return errors. No persistence claim is made.

### Per-function RAM sections

The pinned SDK explicitly groups numerous flash/cache functions into shared
sections, retaining unused functions together. `--split-ram-sections` transforms
only function section annotations in private LLVM IR: a function in `.spi_code`
becomes `.spi_code.<function>`, likewise for `.volatile_ram_code`, `.vm` and
`.flushinv_icache`. A reversible-text check verifies that this transformation
does not alter instructions, signatures, globals or call targets. The private
module is re-encoded by the real compiler and linked normally; compiler codegen
and timing equivalence are not physically established.

The transformation covers generated SDK source objects and the real `vm_sfc.c.o`
member in a private CPU archive. Original SDK libraries are not edited or
redistributed. **56 function annotations** change; final target symbol checks
verify **35 retained split symbols** still occupy the original RAMTEXT range.
All subsection wildcards remain collected in RAMTEXT; nothing is moved into XIP,
ordinary data or executable POOL to conceal the overflow. GC can now discard
unused functions independently. This is a link/size optimization, not a verified
flash-off execution or scheduling port.

## Compilation and memory

The session-only profile before section splitting still had the same **10,667 B**
combined overflow. That negative result is preserved. Splitting improves the
actual combined result by **1,212 B**, leaving **9,455 B** over capacity and an
overlap with `.data`. No undefined-reference or multiple-definition diagnostic
appears in the final combined log. No invalid combined ELF is retained.

| Occupied span | Production 1.0.4 | Isolated session/split SDK | Conservative sum | Capacity |
|---|---:|---:|---:|---:|
| RAMTEXT | 24,508 | 9,521 | **34,029** | 24,576 |
| RAM | 40,320 | 55,338 | 95,658 | 98,304 |
| POOL | 101,888 | 10,368 | 112,256 | 344,064 |

SDK XIP is 206,736 B. The conservative RAMTEXT excess is 9,453 B; the actual
failed combined link is 9,455 B because joint GC/alignment differs. Neither is
a valid final executable footprint. Ordinary RAM has 2,646 B headroom in this
screening budget, before any unaccounted allocation or a different final layout.
SDK task stack/queue policy remains 29,440 B excluding TCBs; encoder/server/codec
heap peaks are unmeasured. Original memory regions, debug/stack guards, 8 KiB pool
reserve and 85% audio protection are unchanged. The SDK still retains soft-double
helpers; passing production-only checks does not approve the combined SDK.

## What remains unresolved in flash/cache and RTOS

The dependency/map inspection identifies these concrete platform conflicts:

- SDK `btstack_main.c.o` references `bt_vm_interface` in `cfg_tool.a(vm.c.o)`.
  Inspection of the pinned module's compiler IR shows a pointer to a five-callback
  file interface, not simply the public syscfg read/write pair. Public headers
  expose an opaque setter, not a typed complete contract. No guessed replacement
  table or successful no-op flash methods were added. SDK VM/flash/cache services
  therefore remain linked and must not be called against Emerald storage.
- SDK `setup_arch()` initializes its own watchdog, clocks, interrupts, movable
  code and power latch. Its RTOS entry invokes `setup_arch`, `os_init`, task
  creation and `os_start`. Emerald already owns startup, vectors, clocks/timers,
  ALNK/USB service and flash critical sections. Namespacing `_start` fixes only
  the earlier symbol collision; it does not reconcile those owners.
- The pinned SDK defines **two cores**; its RTOS port pulls native `cpu1_start`.
  The existing SPSC queues require one core. Altering a header macro would not
  rebuild the precompiled kernel/assembly ABI, so no single-core macro workaround
  or pretend context-switch port was introduced.
- `power_hw.c.o` pulls cache services, while startup pulls movable/cache helpers.
  VM removal alone does not establish safe cache or flash-off operation. All
  code reachable while XIP is unavailable, including IRQs and CPU1, needs a real
  ownership/quiescence and call-graph/PC-guard proof before hardware use.

The next implementation gate is a real typed VM/BT persistence adapter or a
supported vendor mode that omits this backend, together with a single RTOS/IRQ/
clock startup owner. A complete RTOS port requires compatible startup/interrupt
sources or sufficiently documented vendor interfaces. Do not guess opaque ABI,
disable required services or place SDK code in unguarded buffers to obtain a
green link. Calibration/MAC provenance, isolated storage, SBC negotiation/wakeup,
buffer drift and combined runtime timing remain open as before.

## Validation and reproduction

- Existing production app-only build/placement/flash-off/no-soft-double/MMIO/pool
  checks pass on unchanged 1.0.4 sources; no new loader or installer is built.
- Isolated session/split SDK reference link passes with required roots, zero
  undefined symbols, SBC-only range, physical-output absence, POOL queue checks
  and retained split-function RAM addresses verified. Reference RF remains demo
  data for offline analysis, not FM-1 calibration.
- Without reference RF, strict linking still fails only on
  `wifi_calibration_param`. No device query supplies those data.
- Both queue profiles pass, and desktop store tests cover missing/full/oversize/
  null/short-buffer/replacement/isolation behavior. The real SDK mutex wrapper is
  target-compiled, not executed on the desktop. No actual SBC encoding, RTOS
  scheduling, IRQ timing, cache/flash operation or headset playback is validated.
- Overall integration remains **failed/incomplete**, as explicitly recorded in
  the summary. Original 0.8.x, 1.0.7 and 1.0.8 evidence is unchanged.

With the provisioned pinned SDK/toolchain and the production audit objects:

```powershell
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual --session-config --reference-rf --combined-preflight
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual --session-config --split-ram-sections
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual --session-config --split-ram-sections --reference-rf --combined-preflight
.runtime/python/python.exe prototypes/bluetooth-audio/audit_current.py --lean --session-config --split-ram-sections
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

Initial dependency provisioning and baseline object generation are described in
the preceding audits. A completed audit process is not a successful firmware
link: inspect the separate strict/combined booleans. Private SDK archives, IR and
objects remain ignored local build artifacts; no SDK code/IR is redistributed.

[Summary](summary.json), [RTOS/flash source contracts and hashes](rtos-flash-contracts.json),
[production report](production-report.json.gz), [build log](production-build.txt.gz),
[desktop tests](desktop-tests.txt.gz), [default SDK report](sdk-default-report.json.gz),
[reference SDK report](sdk-reference-report.json.gz),
[session-only negative result](session-only-before-split.json.gz),
[failed combined log](combined-link.txt.gz), [combined map](combined-map.txt.gz),
[SDK map](sdk-map.txt.gz), [uncompressed evidence hashes](manifest.json).

Stable 1.0.4 package SHA-256 remains
`f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`.
