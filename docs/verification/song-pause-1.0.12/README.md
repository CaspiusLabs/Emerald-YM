# FM-1 pause and resume verification

The owner tested PR #23 firmware 1.0.12 on 2026-11-04 and confirmed that playback
resumed at the paused position and REC restarted the song. The package came from
source 2e4bf3b5a9b8418a5ba4c18616b19c2fd0e40da6 and reported FM-1_68010012.

| Measurement | Result |
|---|---|
| Pause and resume status bars | 4, 6, 13 and 7, with the same seed across each pause |
| REC | Bar 14 to bar 1, seed 20261026 to 2668484726 |
| MIDI Start / Stop / Continue | 2 / 6 / 4 |
| MIDI Clock during pause / remaining notes after final Stop | 0 / 0 |
| Status replies / queries | 469 / 469 |
| Peak audio CPU / worst block | 72% / 4,184 us |
| XRUN / overload / failed boots | 0 / 0 / 0 |
| USB stereo PCM16 at 44.1 kHz | 234.52 s captured |

The host sent documented identity and status queries. The capture shows Stop
followed by a fresh Start at REC. The final manual pause was at bar 77.
Status reports bars, and the desktop test checks retained step and clock phase.
The WAV remains in local build output. Receiver position and long-term transport
reliability were outside this run. The earlier unexplained 1.0.4 Stop remains open.

[Exact tested package](../../../releases/v1.0.12/emerald-ym-1.0.12.fwsc) ·
[SHA-256](../../../releases/v1.0.12/SHA256SUMS) · [Raw capture](hardware-report.json.gz) ·
[Install log](install.log.gz) · [Owner feedback](owner-feedback.json)
