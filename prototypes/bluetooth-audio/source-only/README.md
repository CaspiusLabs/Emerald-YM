# Reduced A2DP/SBC SDK audit — historical development 0.8.1

For the refreshed audit against stable 1.0.4 production sources, see [current integration audit](../current-audit/README.md). Historical 0.7.0 comparisons below remain unchanged.

This continues **draft PR #9**. It fixes the SDK reference link, not the FM-1 port. Production Emerald, the installed/released 0.7.0, its package and all memory/85% guards are unchanged. No hardware was accessed and no .fwsc, release or tag was created.

## What changed

`APP_PRIVATE_PROFILE_CFG` is the manufacturer's supported path for replacing the broad demo profile. The local configuration selects classic A2DP **source** with SBC, one remote device and virtual PCM input. SDK semantics use `USER_SUPPORT_PROFILE_A2DP=0` to disable the sink; emitter enabled then defines source support as 2 in `bt_profile_config.c`. Sink, BLE, AAC, AVRCP, HFP/AG, SPP, HID, PNP, PBAP, PAN and dual-source are disabled. A compiled contract checks those choices.

The build generates configuration copies from the pinned SDK, disabling the explicit DUT/FCC/eSCO-master booleans and private PBG/ADT/HFP helpers. Every replacement must match exactly once. SDK OTA mode bitmask is zero. These are feature selections, not success stubs for missing services. SDK source/configuration/library hashes are recorded; libraries are not edited or redistributed.

Actual runtime dependencies now come from the SDK's initialization, clock/IRQ helpers, event system, configuration storage, update-result reader, shared RF configuration, SBC/virtual input and compiler libraries. The SDK's shared RF implementation uses `wifi_*` names even in a Bluetooth-only build; no Wi-Fi networking archive or Wi-Fi mode is enabled. Runtime sources are compiled for the audit, not installed or connected to Emerald's bare-metal startup. The demo board's GPIO, DAC/power configuration and board startup are never imported.

The important linker fixes are LTO plus per-function/data **backend** sections, real registration-table boundaries, the correct SDK `USE_SDFILE_NEW` ABI, compiler runtime and the manufacturer's large-program call generation for the XIP/RAM distance. The ordinary 0.7.0 firmware build, far-call rules and linker script are unchanged.

The SDK places SBC and JLA descriptors in one input section. Blindly keeping that table exposes both. The audit bounds the selection range to the SBC symbol using its measured 24-byte ELF size, asserted for this pinned SDK, and verifies the final begin/end addresses. The unselected JLA descriptor can remain physically in that shared input section; **it is outside the advertised codec range**, and no JLA encoder library is linked. This is not a claim that every unused SDK byte has been removed.

## Reproduce without FM-1

Use the [existing prerequisites](../../../BUILDING.md) and the SDK commit already pinned by the prototype: `d179b4484759423312073f5fbb232501aa491047`. The reduced audit additionally needs these SDK paths (not committed):

```powershell
git -C .runtime/ac79-bt-sdk sparse-checkout add apps/common/include apps/common/config apps/common/audio_music apps/common/system apps/common/net apps/common/update apps/demo/demo_edr cpu/wl82 include_lib
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --reference-rf
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

The first command audits **without a calibration definition**. Compilation succeeds, but the strict link must still require `wifi_calibration_param`. Those are hardware-specific crystal/PA calibration data. The script does not guess FM-1 values or obtain them through USB.

`--reference-rf` explicitly extracts the reference declaration from the pinned SDK's `demo_edr/board/wl82/board.c`. That resolves the reference link for offline resource analysis. It is **not evidence that those values suit FM-1**. Reports record `reference_rf=true` and `fm1_port_validated=false`. Never silently promote this option into a firmware build.

Outputs are under ignored `build/bt-prototype/source-only/`, with the reference run in `reference-rf/`. Exit zero means the audit completed: inspect `strict_link_passed` and `strict_undefined_symbols` for the actual link outcome. The reference link requires all eleven explicit roots, verifies there are no undefined symbols and checks the SBC-only range. Temporary ELF output is inspected and removed; no installer is built. Gzip diagnostics and JSON reports are [archived here](evidence/README.md).

## Memory interpretation

Use the **final linked reference sections**, not the larger relocatable closure, for the reference footprint. SDK user-stack input sections lack the usual writable flag, so the linker explicitly puts them in RAM; otherwise orphan placement can incorrectly put stacks in XIP. SDK volatile code has its own `.sdk_ram_code` output, separate from production flash-off `.ram_text`.

The report compares static use against the existing 0.7.0 baseline. Their sum exceeds both ordinary RAM and the RAMTEXT region. This is a conservative coexistence budget, **not a combined link**: the SDK also brings services/startup that would replace or conflict with Emerald's current platform. Pool space cannot automatically substitute for executable memory or fixed data. Region sizes are not enlarged to make the test pass.

Dynamic memory remains additional: the eleven listed SDK tasks request **29,440 B** for stacks and message queues if all are created, excluding TCBs; the virtual encoder requests a 2,048 B output buffer; codec, crypto, server and heap overhead are not fully measured. No runtime stack high-water, total heap peak, Bluetooth/USB coexistence or load measurement has been performed. The reference also retains soft-double helpers outside the unchanged Emerald renderer; this does not pass production's no-soft-double requirement.

## Remaining work before any FM-1 candidate

The source-codec range is restricted, but the **negotiation/init/close adapter is still unwired**. The SDK has weak default callbacks, potentially inlined away by LTO; absence of a weak symbol in the final symbol list proves nothing about audio integration. Headphone negotiation must configure the actual encoder format/bitpool, start/stop it correctly and wake the stack as frames arrive. No SBC bitstream was executed or decoded in these tests.

Still required: verified FM-1 RF calibration, a deliberate scheduler/IRQ/clock ownership port, pairing-storage isolation from existing preset/calibration/OTA regions, RAM/code placement and XIP-call analysis, startup copying and guards, removal/adaptation of soft-double paths, actual negotiated-frame tests and a final combined link with heap/stack reserves. The audit's SDK VM sizes are reference compile defaults, **not an approved FM-1 flash partition**. Do not invoke its storage/update/runtime functions on hardware.

Only a later authorized hardware task can establish headset playback, latency and simultaneous YM+PSG+USB audio+MIDI+diagnostics timing. Keep exact 0.7.0 available unchanged.
