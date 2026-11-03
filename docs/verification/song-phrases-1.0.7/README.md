# FM-1 song phrase verification

On 2026-11-04 the owner authorized installation of the PR #21 candidate and
confirmed END/DONE with smooth sound after one physical PLAY press. Source
commit ee4cd8531075e5533f042c337fc30a07faacc7e0 produced firmware identity
FM-1_68010007. The exact 609,677-byte tested package is archived in the repository.

| Measurement | Result |
|---|---:|
| Complete song | 88 bars in 150.80 s |
| USB MIDI Start / automatic Stop | 1 / 1 |
| MIDI Clock pulses | 8,449 |
| Remaining MIDI notes | 0 |
| Status replies / queries | 515 / 515 |
| Sampled music CPU average / maximum observed | 49.02% / 71% |
| Maximum audio block | 4,178 us |
| XRUN / overload / failed boots | 0 / 0 / 0 |
| USB stereo PCM16 recording at 44.1 kHz | 257.69 s |
| Clipped samples in the analyzed music window | 0 |

The last status reported bar 89 and playback stopped. The host sent documented
identity and status queries. MIDI Clock arrival averaged approximately 140.07 BPM
on the Windows host. The recording includes pre-roll and the tail after Stop.
The raw report includes the PCM digest. The large WAV remains in local build output.
These observations cover a single run and leave the earlier 1.0.4 early-Stop
anomaly unresolved.

[Exact tested package](../../../releases/v1.0.7/emerald-ym-1.0.7.fwsc) ·
[SHA-256](../../../releases/v1.0.7/SHA256SUMS) ·
[Raw capture](hardware-report.json.gz) · [Install log](install.log.gz) ·
[Owner feedback](owner-feedback.json)
