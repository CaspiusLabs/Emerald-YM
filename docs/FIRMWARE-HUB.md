# Firmware Hub submission preparation

Prepared against the [submission requirements](https://thefirmwarehub.com/submit/) and [knowledge base](https://thefirmwarehub.com/kb/) checked on 2026-10-27. No submission email has been sent. The owner requested that GitHub remain **private for now**.

## Submission details

- Firmware: **Emerald YM**
- Creator display name: **CaspiusLabs** (repository owner; confirm before sending)
- Repository: https://github.com/CaspiusLabs/Emerald-YM
- Release: https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v0.3.1
- License of combined firmware: **GPL-3.0-only**; component notices in LICENSING.md
- Device: **M-VAVE FM-1**
- USB identity: **FM-1_68000301**; port name: **Emerald YM FM-1**
- Package: **emerald-ym-0.3.1.fwsc**, includes Felucca update loader
- Screenshot: https://github.com/CaspiusLabs/Emerald-YM/blob/main/releases/v0.3.1/ui-preview.png
- Screenshot provenance: 240 × 240 render from the actual desktop UI test, not a device photograph
- Browser player/editor: none; do not link the inherited DUB-1 web tools as Emerald

Description:

> Emerald YM turns the M-VAVE FM-1 into a generative 16-bit music instrument with YM2612 FM, DAC drums and a Sega-style PSG. Play live keys over an evolving backing and shape twelve sources with independent levels, decay, color and echo.

The root `fmhub.json` provides the shorter card text and `groove` category. It intentionally omits unimplemented browser/player links. The Hub reviews changed card metadata before displaying it.

## Ready in this repository

- [x] Browsable source and preserved upstream attribution/history (currently private).
- [x] English README, user/build/install instructions and license notices.
- [x] Release package, SHA-256 and source/tag association.
- [x] Package verification and update-loader simulations, kept separate from hardware evidence.
- [x] Physical boot, matching USB identity, music and representative real-time tests.
- [x] Data incompatibility and volatile settings disclosed in README.
- [x] 240 × 240 UI image and card metadata.

## Before asking the Hub to list it

- [ ] Owner authorizes making the repository public; then verify anonymous source and release-asset access.
- [ ] Check identity uniqueness against the Hub's live catalogue. The static identity page does not populate its catalogue in the text fetch; uniqueness has **not** been confirmed.
- [ ] Install the exact release `.fwsc` on physical FM-1 from official **V15**.
- [ ] Install it from another custom firmware (not merely an earlier Emerald build).
- [ ] From running Emerald, install another firmware through the ordinary update route and confirm it boots.
- [ ] Confirm the Hub recognizes the reported MIDI port and identity after Connect.
- [ ] Review the KB's interrupted-update recovery check in a separately planned hardware task. It has not been done; do not substitute a simulation for it or interrupt updates casually.
- [ ] Confirm the creator display name and image, then send the repository/release links and test results to the Hub maintainer.

Keep raw reports for each physical transition, including both identities and package hashes. Do not label missing tests as passed. The existing 30-second audio tests do not establish these cross-firmware installation paths.

A GitHub-built, attested firmware release is optional. The current binary was built locally with the documented Windows toolchain; it has **no GitHub build attestation**. Do not add a workflow that merely signs a committed binary and call it built from source.
