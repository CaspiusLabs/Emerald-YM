# Firmware Hub submission preparation

Prepared against the [submission requirements](https://thefirmwarehub.com/submit/) and [knowledge base](https://thefirmwarehub.com/kb/) checked on 2026-10-27. No submission email has been sent. The owner authorized public visibility after checking the release download and README on 2026-10-27. The repository is public; no further hardware tests are planned in this task. Unverified paths remain explicitly listed below.

## Submission details

- Firmware: **Emerald YM**
- Creator display name: **CaspiusLabs** (repository owner; confirm before sending)
- Repository: https://github.com/CaspiusLabs/Emerald-YM
- Tested package: releases/v0.4.3/emerald-ym-0.4.3.fwsc on merged main (exact hash in the test report).
- Current validated beta release: https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v0.4.3 (exact tested package, matching source tag).
- Direct firmware download: https://github.com/CaspiusLabs/Emerald-YM/releases/download/v0.4.3/emerald-ym-0.4.3.fwsc
- License of combined firmware: **GPL-3.0-only**; component notices in LICENSING.md
- Device: **M-VAVE FM-1**
- USB identity: **FM-1_68000403**; port name: **Emerald YM FM-1**
- Package: **emerald-ym-0.4.3.fwsc**, includes Felucca update loader
- Screenshot: https://github.com/CaspiusLabs/Emerald-YM/blob/main/releases/v0.4.3/ui-preview.png
- Screenshot provenance: 240 × 240 render from the actual desktop UI test, not a device photograph
- Browser player/editor: none; do not link the inherited DUB-1 web tools as Emerald

Description:

> Emerald YM turns the M-VAVE FM-1 into a generative 16-bit music instrument with YM2612 FM, DAC drums and a Sega-style PSG. Play live keys over an evolving backing and shape twelve sources with independent levels, decay, color and echo.

The root `fmhub.json` provides the shorter card text and `groove` category. It intentionally omits unimplemented browser/player links. The Hub reviews changed card metadata before displaying it.

## Ready in this repository

- [x] Browsable source and preserved upstream attribution/history (public).
- [x] English README, user/build/install instructions and license notices.
- [x] Tested committed 0.4.3 package, SHA-256 and matching merged source commit.
- [x] Package verification and update-loader simulations, kept separate from hardware evidence.
- [x] Physical boot, matching USB identity, music and representative real-time tests.
- [x] Data incompatibility and settings persistence disclosed in README.
- [x] 240 × 240 UI image and card metadata.

## Before asking the Hub to list it

Physical evidence: [2026-10-27 round trips](verification/install-roundtrips-2026-10-27/README.md). These results apply to the exact committed 0.4.3 bytes, not the older published 0.4.0 asset.

- [x] Attach the exact tested 0.4.3 bytes to release v0.4.3, tagged at matching source commit b5848cd40b4d8dda4612cb9b8c3dc9687e29abc6; no rebuild or substitution.

- [x] Owner authorizes public visibility; repository and release downloads are public, with anonymous access and the downloaded firmware hash checked.
- [ ] Check identity uniqueness against the Hub's live catalogue. The static identity page does not populate its catalogue in the text fetch; uniqueness has **not** been confirmed.
- [x] Install exact Emerald 0.4.3 package on physical FM-1 from official **V15**.
- [x] Install exact Emerald 0.4.3 package from **Dinghy 0.3**, not merely an earlier Emerald build.
- [x] From running Emerald 0.4.3, install official V15 and Dinghy 0.3 through the ordinary updater; both booted with user-confirmed screen/key audio. Restore Emerald at the end.
- [ ] Confirm the Hub recognizes the reported MIDI port and identity after Connect.
- [x] Limited first step: physical USB data interruption on battery at 25% of the loader transfer; reconnection and restart of the same-image installation passed. [Evidence and limitations](verification/usb-data-interruption-2026-10-27/README.md).
- [ ] Complete the broader interrupted-write recovery check separately. The same-image test may skip equal sectors and does not establish recovery from interrupted changed-sector writing or power loss; the Hub browser route remains untested.
- [ ] Confirm the creator display name and image, then send the repository/release links and test results to the Hub maintainer.

Keep raw reports for each physical transition, including both identities and package hashes. Do not label missing tests as passed. Earlier audio tests alone did not establish these paths; the linked physical transitions now do for the pinned 0.4.3 package. From 0.4.0, settings writes reuse the FC000/FD000 project-sector pair; this is disclosed in README and the install guide.

A GitHub-built, attested firmware release is optional. The current binary was built locally with the documented Windows toolchain; it has **no GitHub build attestation**. Do not add a workflow that merely signs a committed binary and call it built from source.
