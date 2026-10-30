# Exact 0.11.3 physical validation

Package: `emerald-ym-0.11.3.fwsc`, 609,677 bytes; SHA-256 `a71dae83f0a83f035a60094eee4253be6d32f3d4ac44f50d74bcdda597c95591`. Source `b7c6d0f860eb294f2376476e37f6a20f54ab572b`, identity `FM-1_68001103`. The prior owner-authorized installation and test are archived here during the documentation/publication follow-up; no extra device access or rebuild was performed for publication.

**Passed:** 211.907 s music during simultaneous USB MIDI master capture, WinMM stereo USB audio recording and documented read-only status queries every half second. Physical PLAY produced Start, 11,364 Clock pulses and Stop. Average Clock arrival rate was 134.0634475 BPM. No Clock outside transport, duplicate Note Ons or held outgoing notes at the final stop.

547 queries / 547 valid replies across the entire session, including 419 music statuses after the two-second settling interval. No timeout. Mean response latency 3.0603 ms, maximum 6.0096 ms; maximum observed status gap 0.5052 s. Sampled music CPU average 45.7327%, maximum 72%, worst block 4189 us. XRUN, overload and failed-boot counters stayed zero.

Recorded 273.67 s stereo PCM16/44.1 kHz. Analyzed 180 s of music: RMS 668.734, peak 14662, zero clipped samples; longest identical/zero stereo run 0.3628 ms. The large WAV remains local in ignored `build/usb-midi-sync-hardware-0.11.3/clock-audio.wav`; its PCM SHA-256 is preserved in the raw report.

The owner answered yes to the combined layout/GLO/PLAY check: consistent panel gaps, equal lower cards, instrument bar below text, dim GLO with physical audio, bright GLO plus USB ONLY and physical mute, then EDIT OUT and playback. The owner also confirmed final Stop and correct screen. Routing is owner-observed: status v1 cannot read USB ONLY. Final state: stopped, EDIT OUT and USB ONLY selected. No persistent settings edits were sent.

Raw files are preserved without editing: [capture report](hardware-report.json.gz), [summary](summary.json), [owner feedback](owner-feedback.json), [test log](test.log.gz), [installation log](install.log.gz). The summary includes owner-observed route/UI metadata separately from measured fields. [Publication/download verification](publication.json) records stable-release assets and their anonymous download hashes.

Scope: finite physical observation, not proof against all future diagnostic timeouts. Clock timestamps are Windows/Python callback arrivals, not electrical or receiver jitter. No external synthesizer, separate USB packet counters, exact-version live-key/TRS-input retest, long session, power-loss recovery or cross-firmware transition test. Preserve the [failed 0.11.0 diagnostic evidence](../../usb-midi-sync-0.11.0/hardware/README.md) unchanged. [Original desktop-only evidence](../README.md) remains a separate historical stage.
