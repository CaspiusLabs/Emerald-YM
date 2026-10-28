# Physical USB MIDI IN/OUT, EDIT and audio — exact 0.10.0

The owner authorized installation and physical testing after the desktop stage. Exact 0.10.0 from PR #11 was installed and verified as **FM-1_68001000**, without rebuilding. The source commit is `e906fbc5eb3c91e45c95101c46bd0d674eef533d`; the 609,677-byte package SHA-256 is `195cd2d1a504d5936e79cde227d64d2d8d1bd407945e8156dac795f682d59401`. These bytes are preserved in [releases/v0.10.0](../../../../releases/v0.10.0/) and the [GitHub beta release](https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v0.10.0).

## Measured result

The main session captured USB MIDI performance packets and stereo PCM16/44.1 kHz audio while polling the documented read-only status query. It included physical keyboard input, a paused negative-control USB note, **60 seconds of backing driven by 4,320 USB Clock pulses at 180 BPM**, Stop and release observation. The complete recording is 81.41 s; CPU averages refer to the 60-second sequence, not the whole recording.

| Check | Result |
|---|---|
| Average sampled audio CPU | 45.9833% |
| Maximum audio CPU | 72% |
| Longest measured audio block | 4,212 us |
| XRUN / overload blocks / failed boots | 0 / 0 / 0 |
| MIDI OUT | 2,047 Note Ons captured across keyboard channel 1, melodic/PSG channels 2–8 and drums channel 10 |
| Duplicate active Note Ons / notes left after Stop | 0 / 0 |
| Outgoing keyboard chord in separate EDIT test | Up to four simultaneous notes observed; all released when EDIT changed to IN |
| Input before and after OUT | Three USB notes and chord audible in captured audio; release returned to quiet; no performance echo |
| Incoming USB note while OUT and paused | No audio response or MIDI echo |
| Audio body, 54 s analyzed | RMS 280.18; peak 2,495; zero clipped samples |
| Longest zero / repeated stereo run in body | 0.2494 / 0.2721 ms |
| Owner feedback | Bright EDIT/USB MIDI OUT header, smooth backing, return to dim EDIT while holding keys |

The short-test checks in [summary.json](summary.json) all passed. The first main recording captured single physical keys rather than a simultaneous chord. A separate held-key transition test then established polyphony and Note Off cleanup; the original first-session result was not rewritten. The test ended stopped in MIDI IN, with host test notes released. No instrument-level, effect or performance-setting edits were sent.

## Publication check

The owner-authorized beta release was published afterward. All three public browser-download URLs (firmware, SHA256SUMS and ui-preview.png) were downloaded anonymously and compared byte for byte with the preserved local assets. The tag was verified at the exact firmware source commit. See [publication.json](publication.json). PR #11 remains open; no merge was requested.

## Raw evidence

- [Summary](summary.json) and [owner observations](owner-feedback.json).
- [Installer log](install.log.gz), [main MIDI OUT/audio/status report](output-report.json.gz) and [main log](output.log.gz).
- [Initial input report](input-before-report.json.gz) and [log](input-before.log.gz).
- [Held-key EDIT transition report](edit-transition-report.json.gz) and [log](edit-transition.log.gz).
- [Input after returning to IN](input-report.json.gz) and [log](input-after.log.gz).
- [Complete-WAV sizes and SHA-256](recordings.json). Raw WAV files remain local under `build/usb-midi-out-hardware-0.10.0`; the reports contain PCM hashes and measurements. No bulky recording is required to install the release.

Reports/logs are archived without changing their results. The summary's last limitation describes the original test task, when evidence was still local and no release was created. Publication happened afterward in the owner-requested documentation 0.10.1 follow-up; that does not turn a desktop report into a hardware pass or create a new firmware image.

## Limits

This validates actual USB MIDI reception on the computer, EDIT cleanup, clock-driven sequence progress, concurrent USB capture and status/timing measurements. It does not validate destination-synth/DAW audio rendering, long-session operation, isolated USB packet glitches, separate UAC scheduling counters, USB disconnect recovery or update interruption. Continuity measurements are signal run-length checks, not proof of glitch-free packet delivery. CPU MAX is lifetime since this installation; the main run raised it from the earlier input-test maximum of 41% to 72%.

The FM-1 minijack remains MIDI IN. It was not retested during this USB-focused 0.10.0 session. Earlier exact 0.9.0 owner-confirmed UM-ONE/Launchpad Pro MK3 adapter input observations remain version-specific. No physical minijack MIDI OUT is implemented or claimed. The older exact 0.4.3 V15/Dinghy installation round trips and interrupted same-image USB transfer results do not transfer to this release.

The [screen preview](../../../../releases/v0.10.0/ui-preview.png) is a 240×240 capture from the exact 0.10.0 native UI renderer, not a device photograph. The owner independently confirmed the actual EDIT LED/header behavior during this session.
