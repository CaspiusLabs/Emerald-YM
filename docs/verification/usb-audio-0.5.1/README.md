# USB audio naming 0.5.1 — desktop-only follow-up

Build and full desktop suite passed on 2026-10-27. [Package and memory report](package-verification.json), [raw test log](desktop-tests.log). The inherited optional JavaScript installer comparison remains skipped. No MIDI port was opened and no device was flashed in this follow-up.

Audio-control, audio-streaming (both alternate settings) and terminal names reference USB product string 2, `Emerald YM FM-1`. Emerald UAC bcdDevice changes from 3.10 to 3.11; VID/PID are unchanged. Loader and non-Emerald descriptor values remain unchanged. The test checks the name string, revision, interface references and presence of the composite descriptor in the built application. Firmware audio DSP, ISR scheduling and status layout are unchanged.

Application 107060 B; data/BSS 39120/98304 B; pool 101888/344064 B; RAMTEXT 24492/24576 B, all unchanged from 0.5.0. Guard limits are unchanged.

Windows can retain an endpoint friendly name in its persistent property store. A descriptor revision and explicit audio names do not guarantee that an existing cached name is replaced. The Windows display name after installing 0.5.1 remains to be checked. No registry edits, device removal or driver changes were performed. [Microsoft endpoint naming documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/audio/friendly-names-for-audio-endpoint-devices).

[Physical 0.5.0 results](../usb-audio-0.5.0/README.md) remain version-specific and are not relabelled as 0.5.1 measurements.
