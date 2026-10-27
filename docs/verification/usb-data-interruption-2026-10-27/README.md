# USB data interruption on battery — 2026-10-27

**Passed within the limited scope below.** Firmware stayed powered by its battery, as confirmed by the owner before the test. No device power cut was requested or performed.

- Running and target firmware: exact accepted Emerald YM 0.4.3, identity `FM-1_68000403`.
- Package SHA-256: `6669787e1d7a3b56f37bd0494c992fbdba5cb59e5dc13d5f2380d7c4fbd64925`.
- The normal application check started the target package's loader, identity `ota-FM-1_900`, MIDI pair `Felucca Update`.
- At 25% of the loader transfer, the host deliberately paused responses and signalled the owner to unplug USB. This progress is a package-read offset, not proof that 25% of flash was programmed.
- USB absence was observed at 17:58:07.876 UTC; presence returned at 17:58:20.876 UTC (about 13 seconds). The instruction was to wait five seconds before reconnecting; enumeration times are the measured evidence.
- After reconnection, the host allowed 17 seconds for the previous read attempt to expire, verified the loader identity and restarted installation using the existing documented updater. It did not continue the old host connection or resume at an assumed byte offset.
- The transfer completed and the normal application returned `FM-1_68000403` at 17:59:05.511 UTC, without a power cycle, button shortcut or ROM rescue.
- A subsequent 20-second continuous backing test passed: CPU MAX 67%, longest block 3923 us, XRUNS 0, overload 0, failed boots 0. Playback was stopped by normal test cleanup. No effect CCs or settings edits were sent.

## Evidence and limits

[Raw transition events](report.json), [raw audio report](audio-after.json), and [executed scratch runner](executed-runner.py.txt). The runner is archived evidence, not a general recovery tool. It uses `tools/fm1_install.py` and refuses unrelated or ambiguous MIDI pairs; Roland UM-ONE was not opened. The runner path recorded here was `build/temp/test_usb_interruption.py`.

This reinstalled the **same bytes** already running. The loader compares sectors and skips equal ones. Consequently, this proves USB disconnection/reconnection and restarting this same-image loader session on this device; it does **not** establish recovery from loss of power, an interrupted changed-sector write, arbitrary corrupt flash, or the Firmware Hub browser Install path. No flash erase/program trace was collected. The broader interrupted-write checklist remains open.

Source/documentation version 0.4.5 records this evidence. No new firmware image was built and the installed application remains 0.4.3. Existing release binaries and tags are unchanged.
