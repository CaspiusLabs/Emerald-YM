# Lean SDK and Emerald integration attempt — 1.0.8

Offline work on 2026-10-31 in draft PR #9, following the
[1.0.7 baseline audit](../current-audit/README.md). **There is still no bootable
Bluetooth firmware.** The isolated reference SDK links; the real combined
preflight now fails on RAMTEXT capacity, not missing symbols or duplicate entry
definitions. No startup/renderer hook, device access, radio activity, loader,
installable package, release, tag or merge was performed. Production firmware
sources/build tools and stable installed/released **1.0.4** are unchanged.

## Changes and measured savings

The opt-in `--lean-virtual` profile:

- Removes `subdevice_dac.c.o` from an ignored **private copy** of pinned SDK
  `cpu.a`. This removes its physical DAC/IIS output registrations and dependent
  code. Required SBC/virtual-input roots remain and strict linking verifies real
  dependencies; no hardware functions are replaced with success stubs. Original
  SDK archives retain their prior hashes and are not redistributed.
- Uses a 1,024-frame PCM ring and four complete-SBC-frame slots, saving **6,160 B**
  in queue structures versus the default 2,048/eight profile. Default sizes remain
  available. PCM capacity is about 23.2 ms at 44.1 kHz, not total Bluetooth latency;
  reduced scheduling tolerance is unvalidated on hardware.
- Places only those **6,188 B non-executable queues** in `.pool.bt`, collected by
  Emerald's existing `.pool*` map/zeroing. Target symbol checks prove their POOL
  addresses; no code is relocated there. The isolated audit heap begins after
  them, with 20 B alignment padding. This relocation frees ordinary RAM but is
  not itself a reduction in total memory consumption.

Before queue relocation, the lean SDK saved 7,168 B in ordinary RAM. The final
occupied RAM+POOL spans save **7,152 B** against the 1.0.7 reference, including
alignment. XIP drops by **10,504 B**. Static sizes exclude dynamic stack/heap peaks.

| Occupied span | Production 1.0.4 | Lean isolated SDK | Conservative sum | Capacity | Result |
|---|---:|---:|---:|---:|---|
| RAMTEXT | 24,508 | 10,737 | 35,245 | 24,576 | **10,669 B over** |
| RAM | 40,320 | 55,290 | 95,610 | 98,304 | 2,694 B spare |
| POOL | 101,888 | 6,208 | 108,096 | 344,064 | 235,968 B spare |

SDK XIP is 208,144 B. This table remains a conservative sum of separate builds,
not a successful combined map or proof of runtime fit. POOL capacity is not an
approved RTOS allocation budget; the existing 8 KiB minimum reserve is retained
in combined heap bounds. The SDK task policy still describes **29,440 B** possible
stacks/queues excluding TCBs, plus the encoder's 2,048 B output request and
unmeasured codec/server/controller allocations. No task stack was shrunk.

## Integration changes and strict combined result

`--combined-preflight` links the actual current Emerald app objects and the real
SDK closure with GC and `--no-undefined`. A private generated linker script keeps
the original regions, audio placement, debug reserve, stacks and NOINIT, adds real
SDK registration/data sections, and defines cache/data bounds from actual sections.
The original production linker script is not edited. Both object sets and private
archive hashes are recorded in the raw report.

The first integration obstacle was a second `_start`: SDK `startup.S.o` is pulled
by the RTOS reference to `cpu1_start`. Removing that member would discard real
dependencies. Instead, the combined-only private copy namespaces its boot entry
to `eym_sdk_boot_start`, leaving `cpu0_start`, `cpu1_start`, relocations and all
allocated executable/data bytes unchanged. The supplied old objcopy lacks symbol
renaming, so `elf_namespace.py` changes only ELF string-table/symbol metadata.
The build validates the original/global symbol, verifies the final name through
the target nm tool, and checks allocated bytes remain identical. SDK startup
still exists: **this fixes a link collision, not scheduler/CPU1 ownership**.

The final combined link has no undefined-reference or multiple-definition
diagnostics. It **fails** with:

```text
section .sdk_ram_code will not fit in region RAMTEXT
section .data ... overlaps section .sdk_ram_code ...
region RAMTEXT overflowed by 10667 bytes
```

The 10,667 B failed-link result differs slightly from the 10,669 B conservative
sum due to shared sections/alignment. Failed-link addresses are diagnostics, not
a valid executable layout. Any temporary ELF is removed. No ignored-symbol mode,
duplicate-definition override, synthetic service implementation, RAMTEXT expansion
or protective-threshold change is used.

## Remaining barrier and next implementation work

Queue/driver reductions resolve ordinary-RAM pressure in the screening table,
but do not reduce the SDK's flash-off/volatile-code footprint. The isolated map
attributes 4,766 B to `.volatile_ram_code`, 3,438 B to `.spi_code`, and 634 B to
cache/movable-code helpers, plus RAM-resident strings/crypto constants and padding.
Moving these into XIP blindly would violate flash-off execution requirements.

The next substantive port must reconcile the SDK flash/cache/VM implementation
with Emerald's existing flash driver and protected `.ram_text` rather than link
both platforms unchanged. It must also establish one scheduler/IRQ/vector/clock
owner and account for the SDK's actual CPU1 path. In this pinned SDK `asm/cpu.h`
defines two cores; the prototype's same-core SPSC queues cannot be assumed safe
if a callback migrates to CPU1. No core-count macro was guessed or patched.

Other 1.0.7 gates remain: verified FM-1 RF calibration and MAC provenance, isolated
pairing storage, negotiated SBC init/close/bitpool and stack wakeup, drift and
backpressure handling, startup copies/PC guards/far calls, and heap/stack peaks.
The reference still retains **15 soft-double helpers** and has not passed the
production no-soft-double/flash-off gates. Link-root presence is not playback.
No CPU load, headset latency, SBC bitstream correctness or USB/audio/MIDI/status
coexistence has been established for Bluetooth.

## Checks and evidence

- Production app-only 1.0.4 compilation and existing build/placement/flash-off/
  no-soft-double/MMIO/pool checks pass; no production source or package change.
- Lean SDK with explicit reference RF strictly links, has zero undefined symbols,
  retains all eleven roots, SBC-only selection and POOL queue addresses, and has
  no physical output driver. Default lean strict link still fails only on
  `wifi_calibration_param`; demo calibration is never silently used for FM-1.
- Both default and lean queue desktop tests pass: stereo conversion/saturation,
  silence, overflow, cursor/ring wrap over 10,000 blocks and complete SBC-frame
  admission/drop/size handling. Synthetic frames do not test SBC encoding or RF.
- Real combined preflight fails as disclosed above; overall integration is not
  passed. Earlier 0.8.x/1.0.7 raw evidence remains unchanged. No full production
  audio/UI suite was repeated because its sources are unchanged.

Reproduce with the provisioned pinned SDK/toolchain, from repository root:

```powershell
.runtime/python/python.exe prototypes/bluetooth-audio/audit_current.py
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual --reference-rf --combined-preflight
.runtime/python/python.exe prototypes/bluetooth-audio/audit_current.py --lean
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

First-time users must produce the unmodified reference SDK report before the
first comparison command, as described in the 1.0.7 audit. Audit exit zero means
the audit completed: read the strict/combined link booleans, not just process exit.
Private modified archives and raw app objects stay in ignored `build/`.

- [Summary](summary.json), [production report](production-report.json.gz),
  [production build log](production-build.txt.gz).
- [Lean reference SDK report](sdk-reference-report.json.gz),
  [lean default report](sdk-default-report.json.gz).
- [Failed combined linker diagnostics](combined-link.txt.gz),
  [combined map](combined-map.txt.gz), [isolated SDK map](sdk-map.txt.gz).
- [Namespaced entry symbols](namespaced-startup-symbols.txt.gz),
  [queue tests](queue-tests.txt.gz), [uncompressed evidence hashes](manifest.json).

Stable 1.0.4 `.fwsc` SHA-256 remains
`f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`.
