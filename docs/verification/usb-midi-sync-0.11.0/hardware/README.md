# Exact 0.11.0 physical USB Clock/audio/status test

Package SHA-256: `5dfcfb62e2f4f2d6ff30089cba4213db2cf1af9bf42603b36e586946d5d22487`; source `3f533ed8658fc3db9b40483634eb2ea4cbd1e602`, identity `FM-1_68001100`. These reports were captured in the preceding owner-authorized hardware task and archived during the 0.11.1 repair. No device was accessed to archive them.

**Functional checks passed; diagnostic continuity did not. Overall unqualified pass remains false.** Physical PLAY sent Start, Clock and notes; physical Stop / Start / Stop worked, with no idle Clock or remaining captured notes at the final stop. The owner confirmed smooth music, EDIT/header and 134 BPM. Measured average Clock arrival rate was 134.0669 BPM. These are Windows/Python callback arrivals, not electrical jitter or external receiver measurements.

The first combined run recorded 103.76 seconds of USB audio and aborted after a documented status query exceeded three seconds. Clock/audio continued. A 60-second music window had RMS 316.00, peak 3262, zero clipped samples. Sampled audio CPU averaged 45.67%, maximum 72%; global maximum block including follow-up was 4188 us. XRUN, overload and failed-boot counters stayed zero.

A fresh follow-up session captured the physical Stop / Start / Stop sequence and returned 54 status replies with no timeouts. This does not erase the earlier failure. The original first report ends while playing; its remaining-note set is not evidence of stuck notes. The initial Start and final Stop are in separate recordings with an observation gap. Do not infer continuity across that gap. No external synthesizer, physical jack output, long-session or USB packet-counter test was performed.

Raw reports are preserved, compressed without editing: [first failed report](hardware-report.json.gz), [follow-up](followup-report.json.gz), [owner feedback](owner-feedback.json), [combined summary](summary.json), [first log](test.log.gz), [follow-up log](followup.log.gz), [installation log](install.log.gz). The large PCM recording remains local in ignored `build/usb-midi-sync-hardware-0.11.0/clock-audio.wav`; its recorded hash is in the first report. No release/tag or merge was created.

The later [0.11.1 analysis](../../usb-midi-sync-0.11.1/README.md) reproduces a concrete shared-mailbox race consistent with a lost status request. The historical capture has no firmware-side per-request trace, so it cannot prove which event caused this particular timeout.
