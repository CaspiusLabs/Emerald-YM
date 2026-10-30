# 0.11.1 — diagnostic timeout repair (desktop only)

Owner-requested follow-up in PR #12. VERSION is 0.11.1. **No MIDI ports opened, device access, flashing, release/tag or merge in this task.** Installed exact 0.11.0 and public release 0.10.0 remain unchanged. Physical validation of the repair is pending.

## Reproduced loss path

The old main loop called `yusb_service(); ota_service();`. Both inspect the same ISR-published SysEx mailbox. If the diagnostic service finds it empty, then USB publishes a status request before the OTA service runs, `ota_take` unpacks it as an unrecognized OTA message and releases it without replying. The host then times out while synthesis, outgoing MIDI Clock and USB audio can continue.

The desktop regression reproduces this old two-consumer ordering with the production diagnostic handler and actual OTA handshake decoder. It asserts that a late status request is released with no reply. This proves the loss path exists. It is consistent with the [original failed hardware capture](../usb-midi-sync-0.11.0/hardware/README.md); without firmware-side request tracing, the exact historical occurrence cannot be established conclusively. Recovery in a reopened session is not proof of uninterrupted diagnostics.

## Fix and coverage

The main loop now calls only `yusb_service`. That function checks the shared mailbox once to select the handler. Empty means return and wait for the next iteration; a status frame is answered; any other claimed frame goes to the unchanged OTA handshake service. The ISR cannot replace a claimed frame until its selected consumer releases it. No second unguarded consumer runs afterward. No new protocol messages, timeouts, automatic retries, long IRQ masking, queue priorities or audio-path changes are introduced. Update sessions retain their existing parser.

`tests/ym_midi_test.c` tests the actual dispatch statement extracted from `ym_loop.c`, with arrival injection at successive mailbox-check boundaries in MIDI IN and OUT. It validates exactly one response/release, every one of the 11 status fields, the real OTA identity decode/reply and malformed/unknown-frame recovery. The old-order reproduction remains an explicit test of the failure mechanism. Existing actual-engine Clock/notes, queue-stall, follower, USB audio/routing, settings, UI/LED, installer and OTA/loader simulations also pass. Generic PCM references remain dry `7ce44fbe` / FX `f9fae4ef`.

## Build and limits

Native application: 111,848 B. Data+BSS: 40,272 / 98,304 B. Pool: 101,888 / 344,064 B. Audio hot code/tables: 21,604 B. RAMTEXT: **24,492 / 24,576 B (84 B spare)**. Identity: `FM-1_68001101`. The existing flash-off, no-soft-double, address/MMIO and startup-copy checks pass. The 85% protection, memory regions and synthesis code remain unchanged. Two existing ymfm missing-braces compiler warnings remain.

Build and full desktop suite pass; optional inherited JavaScript package comparison is skipped as before. See [summary/hash](summary.json), [package verification](package-verification.json) and compressed raw logs: [build](build.txt.gz), [desktop/audio/MIDI](desktop-tests.txt.gz), [UI/LED](ui-tests.txt.gz), [package/update](package-tests.txt.gz), [mock installer](installer-tests.txt.gz), [generic emulator](generic-tests.txt.gz).

Reproduce with `./build-ym-windows.ps1`, `./test-ym-windows.ps1` and `cmd /c tools\test-ym-generic-windows.cmd` using the [provisioned toolchain](../../../BUILDING.md). These commands never open MIDI or flash hardware. The ignored local candidate is `build/emerald-ym-0.11.1.fwsc`. Desktop results do not establish target CPU/jitter or uninterrupted physical diagnostics; a later authorized hardware run must check combined status/Clock/audio on these exact new bytes.
