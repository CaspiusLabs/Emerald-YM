# PR #22 FM-1 verification

On 2026-11-04 the owner tested firmware 1.0.10 from
8de7bce8b3132db67ddede55e84219ae6f36cd9c. The installed identity was
FM-1_68010010. The owner answered yes to the combined prompt about END/DONE,
smooth audio and an audible difference in melody or rhythm.

| Measurement | Result |
|---|---:|
| Complete song | 88 bars in 150.81 s |
| Seed | 20261026 |
| MIDI Start / automatic Stop | 1 / 1 |
| MIDI Clock pulses | 8,450 |
| Remaining notes | 0 |
| Status replies / queries | 413 / 413 |
| Sampled music CPU average / peak | 48.54% / 71% |
| Worst audio block | 4,174 us |
| XRUN / overload / failed boots | 0 / 0 / 0 |
| USB stereo PCM16 at 44.1 kHz | 206.62 s recorded |
| Clipped samples in analyzed music | 0 |

The last status reported bar 89 with playback stopped. The host sent documented
identity and status queries. The WAV remains in local build output and the
raw capture includes its PCM digest. This physical run covered one seed.
Desktop tests cover same-seed repeatability, different-seed PCM and the Endless
golden. RAMTEXT was 24,544 of 24,576 bytes. The earlier unexplained 1.0.4
MIDI Stop remains unresolved.

[Exact tested package](../../../releases/v1.0.10/emerald-ym-1.0.10.fwsc) ·
[SHA-256](../../../releases/v1.0.10/SHA256SUMS) ·
[Raw capture](hardware-report.json.gz) · [Install log](install.log.gz) ·
[Owner feedback](owner-feedback.json) · [Build](pr22-build-transcript.txt.gz) ·
[Desktop tests](pr22-test-transcript.txt.gz) · [Package verification](package-verification.json)
