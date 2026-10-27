# Windows audio-name refresh — physical result, 2026-10-27

Source/tooling 0.5.2; the physical firmware remains exact 0.5.1, SHA-256 `f3b73c8eeb275c04fc819ad858d6326733853c9cfef4597f9985465894e813b2`. No new firmware image was built or flashed for this repair.

The stock Microsoft `usbaudio` MEDIA devnode retained FriendlyName `Felucca` while its BusReportedDeviceDesc was already `Emerald YM FM-1`. The helper selected exactly the verified VID1209/PID0001/MI00 MEDIA child, verified its USB product/service/parent, and removed/re-enumerated only that child through PnP. The Windows Administrator/UAC requirement was satisfied. It did not delete the driver package, change an endpoint name manually or perform direct registry naming edits.

- [Actual refresh report](refresh-report.json): remove/rescan exit codes 0, no restart requested, adapter returned named `Emerald YM FM-1`.
- [Actual endpoint snapshot](after-endpoints.json): recording endpoint `Emerald YM FM-1 (Emerald YM FM-1)`; no Felucca in the present FM-1 audio name. WinMM's 31-character name is truncated.
- [Selection tests](selection-tests.log): valid WhatIf plus refusal of absent/ambiguous/unrelated targets, wrong product/service/parent. The tests mock PnP reads only and execute no physical device mutation. Live WhatIf also passed under PowerShell 7 and Windows PowerShell 5.1.
- [Post-refresh MIDI/status](midi-status.json), [raw log](midi-status.log), [capture analysis](capture-report.json): 15 seconds continuous backing with alternating two/three MIDI notes and diagnostics while recording 17.3 seconds, stereo PCM16/44.1 kHz. Pass, XRUN0/overload0/failed boots0. Lifetime max CPU72%, max block4197us includes prior activity on the same boot; no fresh-boot performance comparison is claimed.

Only the renamed FM-1 audio input and the verified Emerald MIDI pair were opened, not microphones or Roland UM-ONE. Body clipping count 0, longest repeated frame run 0.272ms, longest zero run0.181ms. These are signal checks, not proof that every USB packet arrived. Status does not expose UAC packet counters. Playback was stopped by cleanup; device remains on 0.5.1. The WAV remains local under build/windows-audio-name-refresh/recording/ and is not committed.

The earlier 0.5.1 name-test failure remains preserved under ../usb-audio-0.5.1/hardware/. It is not rewritten into a pass: this is a subsequent Windows-registration repair.
