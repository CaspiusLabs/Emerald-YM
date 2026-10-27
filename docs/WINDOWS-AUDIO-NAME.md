# Refreshing the FM-1 audio name on Windows

Firmware 0.5.1 already advertises `Emerald YM FM-1` through its USB product and audio interface/terminal strings. On the test computer, Windows retained `Felucca` in the Microsoft `usbaudio` MEDIA devnode while `DEVPKEY_Device_BusReportedDeviceDesc` was `Emerald YM FM-1`. Both 0.5.1 recording and MIDI/status worked. The endpoint-name check nevertheless failed.

Windows stores audio names independently in its device/interface properties and persistent audio endpoint properties. See [Microsoft device properties](https://learn.microsoft.com/en-us/windows/win32/coreaudio/device-properties) and [friendly endpoint names](https://learn.microsoft.com/en-us/windows-hardware/drivers/audio/friendly-names-for-audio-endpoint-devices). A firmware descriptor revision alone does not guarantee replacement of an existing registration.

## Targeted refresh

Close DAWs/recorders first. Leave the FM-1 connected, with no firmware update running. In PowerShell **as Administrator**, from the repository root:

```powershell
.\tools\refresh-fm1-windows-audio.ps1 -WhatIf
.\tools\refresh-fm1-windows-audio.ps1
```

The helper requires exactly one present MEDIA interface matching `USB\VID_1209&PID_0001&MI_00`, bus-reported name exactly `Emerald YM FM-1`, service `usbaudio`, and an FM-1 USB parent. It refuses unrelated or ambiguous devices. It records the old identity/name and invokes Windows PnP removal for **only that child instance**, then rescans to let the stock driver read the current name again. Audio/MIDI may disconnect temporarily.

It does not flash firmware, edit the registry, rename an endpoint manually, remove the driver package, remove a parent subtree or reboot Windows. Default report: ignored `build/windows-audio-refresh.json`. It refuses to mutate device state without Administrator rights. `-WhatIf` only inspects the target and needs no elevation.

Afterward, reopen your recorder and select the input identifying `Emerald YM FM-1`. Windows can format the full label as `Line (Emerald YM FM-1)`. If the interface does not return, reconnect its USB cable and inspect the report. The helper reports adapter return/name; check the actual recording endpoint separately. A returned device alone is not proof that Windows updated every cached label.

Source/tooling version 0.5.2 adds this repair procedure. The installed firmware remains 0.5.1; no 0.5.2 firmware package is needed for a host-side registration refresh.

## Verified result — 2026-10-27

On the test computer, the helper removed only the verified MEDIA child and rescanned successfully (both PnP exit codes 0, no restart requested). The adapter name changed from `Felucca` to **Emerald YM FM-1**. Windows exposed the recording endpoint as `Emerald YM FM-1 (Emerald YM FM-1)`; WinMM displayed its first 31 characters. This is the endpoint-plus-adapter format, not a remaining Felucca name.

A subsequent 15-second continuous backing/live-MIDI/status test passed while recording 17.3 seconds of PCM16 stereo/44.1 kHz audio from that refreshed endpoint. XRUNS/overload/failed boots remained 0. Lifetime CPU maximum was 72%, longest block 4197 us; these lifetime counters include earlier activity on the same 0.5.1 boot and are not a fresh-boot timing benchmark. The recorder body had no clipped samples or long repeated/silent stretches; no packet-level guarantee is inferred. Firmware remained `FM-1_68000501` throughout.

[Audit report, endpoint snapshots, recording/status results and tests](verification/windows-audio-name-refresh/README.md). No driver-package removal, registry naming edits or manual endpoint renaming was needed.
