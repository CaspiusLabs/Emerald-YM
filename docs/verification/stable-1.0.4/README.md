# Exact stable 1.0.4 — final validation and publication

The owner explicitly selected a new build/retest under 1.0.4 instead of relabeling the tested 1.0.8 binary. The PLAY triangle was then corrected before publication. Stable `v1.0.4` identifies only the final package below, not the earlier unpublished same-number candidate.

- Source/tag: `7bb4efdd02499664296e96f134371c68403b22de`.
- File: `emerald-ym-1.0.4.fwsc`, 609,677 bytes; identity `FM-1_68010004`.
- SHA-256: `f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`.
- Documentation VERSION 1.0.5 is publication-only; no rebuild after the final hardware test.

## Final hardware result

**Passed:** 162.40 seconds of the complete 88-bar song with outgoing USB MIDI Clock, stereo WinMM USB recording and documented status queries every half second. Stop arrived automatically at bar 89; no idle Clock or held captured notes remained. The owner confirmed a complete PLAY tip, smooth music and automatic END/DONE.

Status 407/407 replies, no timeout; max response 6.55 ms. Sampled music CPU average 47.59%, lifetime maximum 72%, worst block 4233 us. XRUN/overload/failed boots: 0/0/0. MIDI Clock average Windows arrival rate 130.061225 BPM.

Recorded 203.81 s stereo PCM16/44.1 kHz; analyzed 160.10 s of music, RMS 739.229, peak 5875, clipped samples 0. Large WAV remains local in ignored build/stable-hardware-1.0.4; PCM hash is in the raw report. No settings edits were sent. Final state: backing stopped at END/DONE, HOME bright/song, EDIT OUT.

Raw evidence: [hardware report](hardware/hardware-report.json.gz), [summary](hardware/summary.json), [owner feedback](hardware/owner-feedback.json), [test log](hardware/test.log.gz), [install log](hardware/install.log.gz), [public download verification](hardware/publication.json).

## Desktop result and limits

Native build/full desktop audio/UAC/settings/MIDI/song/UI/LED/update/installer suites pass, with an additional final UI run displaying song mode and the full PLAY tip. [Build](desktop/build.txt.gz) · [Full tests](desktop/tests.txt.gz) · [Final UI](desktop/ui-final.txt.gz) · [Package verification](desktop/package-verification.json). Optional inherited JS comparison skipped. Image 113,584 B; data+BSS 40,320/98,304 B; pool 101,888/344,064 B; RAMTEXT 24,508/24,576 B (68 B spare). Existing guards/regions/85% protection unchanged. No new generic comparison was run during this renumber/UI fix; earlier DSP comparisons remain version-specific.

These are finite tests, not electrical/receiver jitter, external synthesizer synchronization, separate USB packet counters, new TRS/live-key/recovery paths or long-session reliability. Arrangement changes/end are control-rate (~20 ms plus one block), not sample-exact.

## Earlier short-trace anomaly

A prior trace of the same final package captured an unexpected MIDI Stop after about 2.52 s and the original helper aborted immediately. The owner reported one PLAY press and USB MIDI OUT, without MIDI WAIT. A later read found the local song stopped at bar 89, with no audio/boot faults. The cause of the early transport event is unestablished. No firmware change was made before the later full repeat reported above; that repeat contained one Start and the automatic end Stop. Do not relabel the short run as passed or infer universal transport reliability from the repeat.

Preserved: [failed short report](early-stop/hardware-report.json.gz), [log](early-stop/test.log.gz), [later read-only inspection](early-stop/after-short-stop.json). This limitation is also disclosed in README/release notes.

## Preserved historical stages

- [Original 1.0.4 desktop candidate](../song-form-1.0.4/README.md): different source/bytes, never published as v1.0.4.
- [Exact 1.0.8 physical results](../boot-spectrum-size-1.0.8/hardware/README.md): passed, but not transferred to the new 1.0.4 bytes.
- [Aborted pre-PLAY-fix 1.0.4 capture](pre-play-fix/aborted.json), [progress](pre-play-fix/progress.json), [log](pre-play-fix/test.log.gz): remains passed=false. Owner reported the truncated PLAY triangle; no outgoing Start/Clock had been captured before stopping this stage. Its e6957829... package was not published and is not the stable file.

Stable release: [v1.0.4](https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v1.0.4). Older published releases/tags are preserved. This publication stays in PR #19 without merge or Firmware Hub submission.
