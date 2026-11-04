# Firmware Hub listing preparation — Emerald YM 1.0.12

Updated 2026-11-04 for the latest **public, hardware-tested stable firmware
[v1.0.12](https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v1.0.12)**.
Project documentation `VERSION` was 1.0.16 before this documentation task;
it is **not** a replacement firmware. This page is the publication checklist,
not confirmation that Emerald YM is already listed. **No Hub submission was
made as part of this update.**

The [Firmware Hub submission page](https://thefirmwarehub.com/submit/) and
[knowledge base](https://thefirmwarehub.com/kb/) were reviewed again on
2026-11-04. The Hub requests a public open-source repository, a release with an
`.fwsc` asset, a firmware-specific `FM-1_` identity, the update loader,
physical installation/exit tests and a root `fmhub.json`. Its latest-release
card follows published GitHub releases automatically after the initial listing.
Do not treat this checklist as authorization to submit.

## Current submission information

| Field | Exact value or link |
|---|---|
| Firmware | **Emerald YM** |
| Author / creator | **Caspius Labs** |
| Device | **M-VAVE FM-1** only |
| License | **GPL-3.0-only** combined firmware; component notices in [LICENSING.md](../LICENSING.md) |
| Source repository | https://github.com/CaspiusLabs/Emerald-YM |
| Stable GitHub release | https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v1.0.12 |
| Firmware package | [emerald-ym-1.0.12.fwsc](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v1.0.12/emerald-ym-1.0.12.fwsc) |
| Checksums | [SHA256SUMS](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v1.0.12/SHA256SUMS) |
| Firmware source/tag commit | `2e4bf3b5a9b8418a5ba4c18616b19c2fd0e40da6` |
| Tested package SHA-256 | `0ebf70db09e5cf5536bd9ea2924eb1ddbe244732de22ac8c469efb5439440e86` |
| Package size | 609,677 bytes |
| Firmware / USB identity | `FM-1_68010012` |
| USB MIDI/audio name | `Emerald YM FM-1` |
| Hub card metadata | [fmhub.json](../fmhub.json) |
| Hub card image | [Native 240×240 preview](../releases/v1.0.12/ui-preview.png) (4,377-byte PNG; desktop renderer capture, not a physical device photo) |
| Install instructions | [INSTALLING.md](INSTALLING.md) |
| Hardware tests | [1.0.12 transport/USB test](verification/song-pause-1.0.12/README.md); [1.0.10 full-song test](verification/song-diversity-1.0.10/README.md) |
| Browser player/editor | **Not available**; do not link inherited DUB-1 applications |

Suggested short description (also represented by the root `fmhub.json`):

> Emerald YM turns the M-VAVE FM-1 into a generative 16-bit music instrument,
> with YM2612-style FM, DAC percussion and Sega-style PSG. Create evolving
> 88-bar songs, play live keys and shape twelve independent sound sources.

## Current physical and build evidence

- Firmware **1.0.12** was built with the Windows/JieLi toolchain, passed the
  desktop suite and was installed and tested on a real FM-1. The accepted
  package and logs are kept under `releases/v1.0.12/` and
  `docs/verification/song-pause-1.0.12/`.
- The 1.0.12 test verified PLAY pause/resume, REC reseeding, USB MIDI
  Start/Stop/Continue, 469/469 status responses, no remaining notes or
  MIDI Clock during pauses, and 0 XRUN/overload/failed boots. Maximum observed
  CPU was 72% and the worst audio block was 4,184 µs.
- The included **1.0.10** composer was independently exercised on the
  device: the whole 88-bar arrangement ended at bar 89, 413/413 diagnostic
  replies, no held notes, peak CPU 71%, no XRUN or overload. Distinct seeds
  and the original Endless PCM golden were checked by desktop tests.
- The `.fwsc` uses the inherited Felucca update loader. The package/tag is
  a locally built exact image with no independently verified GitHub build
  attestation. Do not claim a GitHub build or signer.
- Earlier complete official V15 ↔ Emerald and Dinghy ↔ Emerald installation
  round trips and a limited same-image USB interruption test apply **only
  to the exact 0.4.3 bytes**, not automatically to v1.0.12. See
  [round trips](verification/install-roundtrips-2026-10-27/README.md) and
  [USB interruption](verification/usb-data-interruption-2026-10-27/README.md).

## Firmware Hub checklist for stable 1.0.12

- [x] Public, browsable open-source repository and complete license notices.
- [x] GitHub stable release with the exact, hardware-tested `.fwsc` package,
  source tag, SHA-256 and preserved update loader.
- [x] Physical FM-1 boot, working USB identity, audio, MIDI and
  transport/status checks on exact firmware 1.0.12.
- [x] Root `fmhub.json` with `what`, `author`, `kinds` and `image`.
  Optional `play`, `editor` and `cheat` links are omitted until implemented.
- [ ] Verify identity **`FM-1_68010012`** is unique in the live Hub catalogue.
  Do not infer uniqueness from the string format or an incomplete text search.
- [ ] Physically check installing **exact 1.0.12 from official V15** and
  from another custom FM-1 firmware, using the normal update process.
- [ ] Physically check leaving **exact 1.0.12** for official V15 and another
  custom firmware, recording identities/versions and package hashes.
- [ ] Check that the Hub's browser **Connect** detects the exact Emerald USB
  identity/port, and validate the browser installation route for 1.0.12
  before claiming that path tested.
- [ ] Confirm final author/links/image presentation and **explicitly
  authorize** sending the listing submission to the Hub maintainer.

### Optional, higher-risk recovery qualification (not yet completed)

- [ ] Test recovery from an interrupted **changed-sector** write on a
  controlled device/recovery setup only when specifically authorized.
- [ ] Test actual power-loss recovery separately. The historical
  same-image USB data interruption did not exercise those cases.

Never run risky flash interruption automatically as a documentation or
publication side-effect. Do not invent results, credentials, or signatures.

## Submission handoff

When the outstanding Hub listing checks are complete and the owner explicitly
approves submission, send the maintainer links (not package attachments):
creator **Caspius Labs**, firmware **Emerald YM**, repository and current
stable release, identity **`FM-1_68010012`**, **GPL-3.0-only** license,
root `fmhub.json` (which includes the card image), install instructions,
and exact physical test results. The submission form on
[thefirmwarehub.com/submit](https://thefirmwarehub.com/submit/) currently
describes the maintainer contact route. Do **not** claim that an email or a
listing already exists.

After acceptance, the Hub says it follows updated GitHub releases and the
root card metadata periodically; confirm the actual resulting card before
claiming that automatic updates are working.
