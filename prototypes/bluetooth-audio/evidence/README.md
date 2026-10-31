# Offline Bluetooth prototype evidence — 2026-10-28

These are desktop/toolchain results, not measurements on FM-1. There was no device access and no installation package. Production firmware sources/build selection and released 0.7.0 bytes were not changed. Full production regression/build scripts were not rerun: they exclude this prototype and would not validate Bluetooth.

## Results

| Check | Result |
|---|---|
| pi32v2 r3 compile, three units, `-Wall -Werror` | Pass |
| PCM queue external/atomic/allocator calls | None; target disassembly archived |
| Host functional tests | Pass; [output](desktop-tests.txt) |
| Relocatable link against real SDK libraries | Pass in both profiles; unresolved symbols are permitted at this stage |
| Strict SDK executable link | **Fail** in both profiles; no executable retained |
| Final firmware memory placement / fit | **Not established** |
| SBC encoding execution / Bluetooth pairing/playback | **Not tested** |
| CPU / interrupt / USB coexistence / audio quality | **Not tested** |

## Measured section totals

| Profile | Allocated section bytes | Writable bytes | Executable bytes | Unresolved symbols |
|---|---:|---:|---:|---:|
| Audio server + SBC + stack + controller | 738,872 | 52,755 | 536,794 | 423 |
| Above + SDK system, CPU, common, FS, crypto, RF | 1,292,935 | 74,779 | 832,300 | 439 |

The first profile already includes both prototype queues and its adapter (12,352 B writable), so **do not add those again**. The 2,048-byte requested encoder output buffer is dynamically allocated and not included in the static queue total. Actual codec/server heap, task stacks, negotiated transport buffers and explicitly reserved ACL pools have not been budgeted.

These are raw section sums from an **unconfigured, relocatable SDK closure**, without final garbage collection, alignment/placement, profile pruning or fixed board configuration. Some dependencies include BLE/TWS/sink/other audio/driver paths that a source-only configuration should remove. More runtime libraries solve some references while pulling new board/configuration references, hence the higher unresolved count in the second profile. This is neither the minimum A2DP footprint nor a complete runnable image. It cannot prove that an optimized transmitter will or will not fit.

The extended profile's writable sections exceed the 59,184 B free ordinary data RAM in 0.7.0 by 15,595 B, before a final allocator/stack plan. There is 242,176 B free pool space, but using it for SDK structures/stacks requires explicit placement and ownership work. RAMTEXT has only 84 B spare; no extra executable RAM or relaxed guards were introduced. The raw SDK executable total also demands a reduced profile rather than naive import into Emerald's existing XIP region. No final combined Emerald+SDK link has passed.

## Why the strict link fails

The [core report](core/report.json) and [extended runtime report](runtime/report.json) contain all unresolved names, library SHA-256 hashes and allocated sections. The exact pinned SDK commit is `d179b4484759423312073f5fbb232501aa491047`. The actual toolchain is the provisioned JieLi Windows 2.5.3 / clang 4.0.1 / pi32v2 r3 route described in BUILDING.md.

Core failures include `server_open`, `server_request`, `server_close`, SDK configuration constants, OS functions, controller/linker table symbols and math helpers. The extended run links the actual SDK system/CPU libraries, but still requires board-specific configuration, RF/clock/interrupt glue, memory/registration boundaries, profile selections and compiler runtime. Those have deliberately not been fabricated. The closures also reference soft-double helpers; this does not mean the new PCM queue uses doubles, and it does not pass Emerald's production no-soft-double requirement.

Full raw linker diagnostics are preserved, gzip-compressed without editing:

- [Core strict-link diagnostics](core/sdk-strict-link.txt.gz)
- [Extended runtime strict-link diagnostics](runtime/sdk-strict-link.txt.gz)
- [PCM target disassembly, unedited and gzip-compressed](pcm-disassembly.txt.gz)

Host tests exercise synthetic PCM and synthetic byte frames: saturation/stereo order, silence on underrun, dropping new data on overflow, 10,000 256-frame blocks read as 128-frame chunks, cursor wrap near UINT_MAX and physical ring wrap, complete encoded-frame admission, exact packet capacity, mixed-size separation and drop counts. They do not validate SBC bitstream generation, codec negotiation, SDK task behavior, cross-core coherency or radio performance.

## Preserved hardware baseline

`releases/v0.7.0/emerald-ym-0.7.0.fwsc` remains 609,666 B with SHA-256:

```text
b575995cb7c829acc97a3672579212767346194811c18a66323621b95d7b64b6
```

The stored baseline build evidence reports image 108,032 B; data+BSS 39,120/98,304 B; pool 101,888/344,064 B; RAMTEXT 24,492/24,576 B. These are prior 0.7.0 results, not a Bluetooth firmware build. No new package/tag/release or physical observation was created for 0.8.0.
