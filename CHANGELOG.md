# Emerald YM changelog

## 0.11.4 — 2026-10-30 — stable 0.11.3 publication

- Publish stable v0.11.3 (not a prerelease) from exact tested source b7c6d0f860eb294f2376476e37f6a20f54ab572b, without rebuilding. Preserve package SHA-256 a71dae83f0a83f035a60094eee4253be6d32f3d4ac44f50d74bcdda597c95591 and all older releases/tags.
- Update README/manual/installation with the aligned screen, dim/bright GLO, USB MIDI master behavior, exact-package physical results and current native UI preview. Archive raw hardware evidence separately from desktop tests.
- Exact 0.11.3 physical run: 211.91 s music with MIDI Clock, USB ONLY recording and diagnostics; 547/547 status replies, no timeouts, sampled CPU average 45.73%, max 72%, max block 4189 us, XRUN/overload/failed boots 0. Owner confirmed layout, instrument bar, GLO and stop behavior.
- Documentation/publication follow-up in existing PR #14 at the owner's request. VERSION 0.11.4 is documentation only; installed/released firmware stays exact 0.11.3. No build, flash, further device access or PR merge in this task.
## 0.11.3 — unreleased — aligned main-screen panels

- Use a consistent six-pixel gutter between the volume rail, transport and parameter/mixer panels. Both bottom cards now match the 86-pixel columns and 44-pixel heights above; extend the rail footer to the same bottom edge.
- Place the instrument level bar below its text, at the same inset, width and baseline as the other parameter bars. Keep full instrument names and remove the redundant SEL prefix so long names fit the narrower card.
- Preserve changed-only native tile rendering, existing canvas, DSP/USB behavior and LED changes from 0.11.2. Separate branch/PR based on PR #13; no device access or flash. Installed firmware remains exact 0.11.1.
## 0.11.2 — unreleased — discoverable GLO illumination

- Keep GLO dim while physical audio is enabled and bright in USB ONLY, matching the existing discoverable-control convention. Output routing and session-only behavior remain unchanged.
- Record the owner's rule in AGENTS.md: every available assigned panel function has visible button illumination, dim when inactive and bright when active or acknowledging an action, with meaningful transport/error colors preserved.
- Separate branch/PR based on unmerged PR #12 (0.11.1). Native build/package and existing UI/LED regression checks pass; no device access or flash. Installed exact 0.11.1 and public release 0.10.0 remain unchanged.
## 0.11.1 — unreleased — diagnostic SysEx mailbox race

- Route the shared SysEx mailbox through one main-loop dispatcher. A status request arriving after the old diagnostic check could previously be consumed and discarded by the following OTA handshake service. Late arrivals now wait for the next iteration; non-status frames still use the unchanged OTA decoder. No wire-protocol, audio-renderer, USB queue, memory-region or protection changes.
- Reproduce the competing-consumer loss deterministically and test the production dispatch order at arrival boundaries in IN/OUT, all 11 status fields, OTA identity and malformed/unknown-frame recovery. Native compilation, full desktop regressions, package/update simulations and generic emulator comparison pass.
- Preserve exact 0.11.0 physical reports, including the original diagnostic timeout (overall unqualified pass remains false), separately from the 0.11.1 desktop evidence. No per-request device trace proves which event caused the historic timeout; the reproduced race is a concrete loss path consistent with it.
- Owner-requested follow-up in existing PR #12. No MIDI ports opened, device access, flash, release/tag or merge in this task. Installed exact 0.11.0 and public 0.10.0 remain unchanged. Physical validation of the fix is pending.

## 0.11.0 — unreleased — USB MIDI master Clock and Start/Stop

- In EDIT OUT, PLAY resets the current-seed arrangement and sends USB Start (FA), Timing Clock (F8) at 24 PPQ and notes. The next PLAY sends Stop (FC). No Continue, idle Clock or Song Position Pointer output. K1 changes master tempo without resetting pulse phase.
- Derive outgoing pulse count from the actual native sequencer step/phase in the existing XIP snapshot. No independent timer, sample-loop callback, allocation, libm/double, memory-map or protective-threshold change. Emission is quantized to 256-frame audio blocks; physical jitter and receiver behavior are unmeasured.
- OUT ignores incoming USB Clock/Start/Continue/Stop to prevent feedback and clears external follower mode. EDIT OUT during playback restarts the arrangement; EDIT IN sends outgoing Stop/Note Off while keeping local playback. IN retains the existing clock follower.
- Stop on pause, output disable, reset, overload and update entry. A missed/still-queued Clock or full output queue stops synchronization without catch-up bursts, clears uncertain queued performance data and repairs notes. USB MIDI WAIT requests an explicit PLAY restart after a stall/disconnect.
- Add production-queue/actual-engine master tests, 60-second renders at 80/132/180 BPM, ordering, tempo changes, phase/cursor wrap, same-seed restarts, Stop retry, stall/reconnect and IN/OUT ownership/UI tests. Separate branch/PR from merged main. Compile/desktop tests only: no device access, flash, release/tag or merge; installed/released exact 0.10.0 is preserved.

## 0.10.1 — 2026-10-29 — 0.10.0 publication and hardware evidence

- Publish exact tested 0.10.0 without rebuilding: tag v0.10.0 at e906fbc5eb3c91e45c95101c46bd0d674eef533d; package SHA-256 195cd2d1a504d5936e79cde227d64d2d8d1bd407945e8156dac795f682d59401. Archive raw physical reports separately from desktop evidence.
- Update English README/manual with MIDI IN/OUT, EDIT, channel map, USB Clock, 60-second combined MIDI OUT/audio/status results and the native 0.10.0 screen render. Link the release download and checksums.
- Owner-requested follow-up in existing PR #11. Documentation-only version; no firmware-source change, rebuild, additional device access or PR merge.

## 0.10.0 — 2026-10-29 — EDIT / USB MIDI note output

- EDIT switches between note input (dim) and USB note output (bright); the header shows USB MIDI OUT. The mode resets to input at boot and is not saved. USB Clock/transport, diagnostics and USB audio remain available in both modes; note/FX input is ignored in OUT, with no MIDI Thru or feedback echo.
- Export physical keyboard notes on channel 1 and separate FM, PSG and percussion sequencer parts on channels 2–8 and 10. Local synthesis continues. Retry Note Off under USB queue pressure; clean up mode/transpose/seed/Stop/overload transitions and recover after USB suspend/reset.
- Read a block-end engine snapshot in XIP within the measured audio budget; small trigger counters preserve retriggers and sub-block drums. Move only sequence-selection constants to the existing XIP pitch section, outside sample loops. No memory-map, flash-off, startup-copy or protective-threshold change.
- The FM-1 minijack remains MIDI IN: the official SDK documents DIN input and USB/BLE output, with no supported jack-output mode. Physical DIN/TRS output requires a USB host/PC routing Emerald to a MIDI interface such as UM-ONE. No UART TX pin/register changes.
- Initial implementation used a separate branch/PR based on unmerged PR #10 and compile/desktop tests only. Subsequent owner-authorized exact-0.10.0 installation and USB MIDI/audio/EDIT tests passed; see the 0.10.1 publication/evidence entry above. No automatic PR merge.

## 0.9.0 — unreleased — USB MIDI Clock and TRS MIDI IN

- Follow USB Timing Clock at 24 PPQ: six pulses per sequencer step. USB Start resets the arrangement with the current seed and waits for Clock; Continue preserves position; Stop pauses. A 500 ms clock gap or MIDI input loss stops external playback until explicitly resumed. Turning K1 returns to saved/manual internal tempo.
- Enable the inherited PH8/UART1 TRS MIDI IN path. Keep USB/TRS held-note masks separate and reserve clock/transport ownership for USB; TRS supports live notes and existing FX CCs, with no SysEx/update entry.
- Timestamp input at ISR queue publication, estimate 30–300 BPM over a quarter note for display/gates and consume external steps at audio-block boundaries with bounded catch-up. Move the existing gate-duration division to control rate; internal reference audio stays bit-exact.
- Add real dispatcher/UART parser/queue/renderer/clock tests, wrap/jitter/loss/transport/overflow coverage and changed-tile external-clock UI checks. Compile/package/memory, generic emulator and full desktop/simulated-update suites pass. RAMTEXT is 24,560/24,576 B; no guard or region expansion.
- No FM-1 access or flash; installed/released 0.7.0 stays unchanged. Separate branch/PR from the unmerged Bluetooth prototype. This candidate has no physical clock/TRS/CPU validation and no public release/tag.

## 0.7.1 — 2026-10-28 — publication documentation

- Publish exact tested 0.7.0 with matching source tag, update README/preview and archive physical evidence. No rebuild or new device image. Merge PR #8 at owner request.


## 0.7.0 — unreleased — GLO USB ONLY

- GLO toggles session-only digital muting of the speaker/headphone output while USB audio, MIDI, diagnostics, sequencer and live keys continue. Bright GLO and a USB ONLY footer indicate the mode; a second press restores normal output. MASTER still controls USB level.
- Fork USB before the local 256-frame fade/mute, with unity bit-exact and protected overload silence retained. No codec reset, DMA stop, persistence/schema change, new allocation, memory-region change or raised 85% guard.
- Add production routing, fade/stereo/buffer/protection tests plus real LED/UI checks. Separate branch/PR; previous public release 0.6.0 is unchanged.

## 0.6.2 — unreleased — 2026-10-27 — publish tested 0.6.0 release

- Publish the exact tested 0.6.0 package as a public beta release without rebuilding; preserve its SHA-256 and matching source tag at c0530a6cd7d026554e21b957665edfd7fe7d51d9. Archive the unchanged package, checksum and native preview under releases/v0.6.0.
- Point README and installation downloads to v0.6.0; keep version-specific test limits and earlier releases/tags unchanged. Documentation/packaging only; installed firmware remains 0.6.0.
- Continue in PR #7 and merge to main at the owner's explicit request after download verification. No new hardware test or Hub submission.

## 0.6.1 — unreleased — 2026-10-27 — 0.6.0 hardware verification documentation

- Feature the actual 0.6.0 UI-renderer image at the top of README and describe the physical MASTER slider. Clearly separate hardware-tested development 0.6.0 from the unchanged public release download 0.4.3.
- Archive raw 0.6.0 recording/MIDI/status reports and the owner's screen/slider confirmation: 30-second continuous test, average sampled audio CPU 46.93%, maximum 72%, max block 4196 us, XRUN 0 / overload 0 / failed boots 0.
- Documentation only, retained in PR #7 at the owner's request. No rebuild, new firmware image, release/tag change, additional hardware test or automatic merge; installed firmware remains exact 0.6.0.

## 0.6.0 — unreleased — 2026-10-27 — console-style MASTER slider

- Reflow the 240x240 main screen around a left-hand volume rail, red slider handle, headphone symbol and narrower framed parameter/mixer cards, following the owner's reference. Preserve the native Emerald title, badge/version and step strip.
- Show the existing filtered physical MASTER pot position on a 0-10 scale, with pixel-level visual hysteresis and a 22x108 changed-only tile. Do not change the squared gain law, DSP, USB audio/MIDI/status, master persistence or protective thresholds.
- Split the static rail into tiles within the existing 240x144 canvas; no new framebuffer, renderer allocation or memory-region change.
- Add actual-renderer tests for endpoints, complete ADC sweep, jitter/hysteresis, unaffected surrounding pixels and changed-only redraws; retain UI/LED/USB/DSP/settings/update regression checks. Desktop only; no installation or physical timing claim.

## 0.5.2 — unreleased — 2026-10-27 — Windows audio registration refresh

- Diagnose the Windows name mismatch on exact 0.5.1: USB reports Emerald YM FM-1, but the Microsoft usbaudio MEDIA devnode retains FriendlyName Felucca. Preserve that failed name check alongside passed recording/MIDI/status evidence.
- Add a targeted, Administrator-only PnP audio-child refresh helper with WhatIf, exact identity/service/parent guards and an audit report. No registry label edits, driver-package deletion or firmware change.
- Physically verify that the targeted refresh replaces the adapter name with Emerald YM FM-1; Windows now exposes Emerald YM FM-1 (Emerald YM FM-1). A post-refresh 15-second recording/live-MIDI/status check passed on unchanged 0.5.1 (XRUN0, overload0).
- Document the repair procedure and archive 0.5.1 hardware results. Source/tooling version only; the installed 0.5.1 image and published 0.4.3 assets are unchanged. Continue in PR #6 at the owner's request.

## 0.5.1 — unreleased — 2026-10-27 — USB audio name and physical evidence

- Name the Emerald audio-control/streaming interfaces and terminals explicitly using product string Emerald YM FM-1; increase the Emerald UAC USB device revision from 3.10 to 3.11, without changing VID/PID or loader descriptors. Windows may retain a cached friendly name; physical display verification is pending.
- Archive exact 0.5.0 recording/MIDI/status results and update README/manual: 30-second continuous test, average sampled CPU 46.93%, maximum 72%, XRUNS 0, overload 0. Keep the older image evidence distinct from unflashed 0.5.1.
- Continue in PR #6 at the owner's request. No flash, public release or automatic merge for this follow-up.

## 0.5.0 — unreleased — 2026-10-27 — USB audio capture

- Enable the inherited UAC1 stereo 16-bit / 44.1 kHz USB capture of the master mix while preserving USB MIDI and status v1 diagnostics. Identity FM-1_68000500; no computer audio playback or multitrack endpoints.
- Add Windows desktop UAC clock-drift and composite audio/MIDI descriptor checks to the Emerald test suite. Allow the new identity in the hardware diagnostic tool without running it or weakening persistent-FX protections.
- Build and desktop regression checks pass within unchanged memory limits and the 85% guard. No device flashing, MIDI access, release publication or hardware performance claim; validated 0.4.3 is unchanged.

## 0.4.7 — unreleased — 2026-10-27 — author display name

- Correct the author/creator display name to Caspius Labs in README and Firmware Hub submission details; preserve GitHub account names, repository identifiers and URLs.
- Record the display-name convention in the developer handoff. Documentation only; firmware 0.4.3, release assets and tags are unchanged.
- Commit directly to main as explicitly requested by the owner for this correction; the normal branch/PR policy remains in place for subsequent work.

## 0.4.6 — unreleased — 2026-10-27 — release downloads and public repository

- Publish the unchanged, physically tested 0.4.3 package as a GitHub beta release, with tag v0.4.3 at its matching source commit b5848cd40b4d8dda4612cb9b8c3dc9687e29abc6. Keep all previous release bytes and tags unchanged.
- Replace the README/install package links with direct release downloads and refresh publication guidance. Keep source/documentation versions separate from hardware version 0.4.3.
- Verify downloaded firmware against the accepted SHA-256 and make the repository public with the owner's authorization. No Hub submission or additional hardware tests.
- Continue on PR #5 at the owner's request; documentation only, no new firmware build or automatic merge.

## 0.4.5 — unreleased — 2026-10-27 — USB data interruption evidence

- Record the authorized battery-powered USB interruption at 25% of the loader transfer, reconnection and successful restart of installation of the exact accepted 0.4.3 package.
- Verify a subsequent 20-second continuous playback run: CPU MAX 67%, XRUNS 0, overload 0.
- Preserve raw reports and distinguish same-image data-link recovery from changed-sector write interruption, power loss and the Hub browser path. Documentation only; no new binary or changed release/tag.

## 0.4.4 — unreleased — 2026-10-27 — installation verification and documentation

- Documentation/source version only; physical cross-firmware tests target the accepted, unchanged Emerald 0.4.3 package from merged main.
- Verify merge of PRs #1–#3, package provenance/hashes, and ordinary USB MIDI update transitions with official V15 and Dinghy 0.3. Record per-step evidence and user boot/audio observations separately.
- Refresh README/install/verification guidance from actual results. No deliberate interrupted write, recovery shortcut, new firmware build or automatic main merge.


## 0.4.3 — unreleased — 2026-10-27 — uppercase title and badge detail

- Further PR #3 review, same version by owner request: extend the badge and its inner fill by one pixel downward, leaving one inner-background row below the second stripe.

- PR #3 review adjustment, explicitly retaining 0.4.3 at the owner's request: align the version at y=28 with YM + PSG / LIVE using the same FONT_XS; both badge stripes use Y_BLUE.
- Follow-up to user review of 0.4.2: restore the larger title font and uppercase EMERALD YM, fitting it beside the compact 16-BIT badge. Add a second badge stripe while retaining the version gap.
- Record the user confirmation that 0.4.2 restores K1–K4 and retains previous mixer/FX settings after complete power-off. Settings schema and audio DSP are unchanged.
- Separate branch/PR, based on the pending 0.4.2 PR; no automatic merge or published tag replacement.


## 0.4.2 — unreleased — 2026-10-27 — header and K1–K4 persistence

- Main title reads Emerald YM; smaller 16-BIT badge with more space above the version.
- Schema 2 stores tempo, density, mutation and signed transpose with instrument levels/FX. Load schema 1 without losing the mixer or writing on boot; new knob fields use defaults until saved.
- Knob and SEQ edits use the existing quiet/debounce SAVE policy; no writes while playing or holding keys. Seed, master, PSG enable and selection remain outside the snapshot.
- Firmware DSP, guard, memory regions and old package/tag are unchanged. Desktop checks include migration, all four knobs, boundary validation, interrupted writes and UI layout.


## 0.4.1 — unreleased — 2026-10-27 — contribution workflow

- Require a VERSION increment for each change, including documentation, on a separate branch with a GitHub pull request targeting main.
- Leave pull requests for review; no direct pushes to main or automatic merging. Merge only at the owner's request.
- Documentation and version metadata only. Installed firmware and the published 0.4.0 package/tag remain unchanged; no new binary or release is created.

## 0.4.0 — 2026-10-27 — persistent mixer and startup screen

- Save/restore all twelve instrument levels and 36 FX values in the existing A/B project sectors, with schema/range validation and CRC/header-last commit.
- Debounced saving only when paused and live keys released for three seconds; SAVE pauses and requests a save. Visible pending/saving/saved/error states and SAVE LED; failed writes require an explicit retry or new edit.
- Version from VERSION below the 16-BIT badge; original console-inspired boot silhouette, without an additional framebuffer or blocking the service loop.
- New persistence tests use production storage with interrupted writes/corrupt records; audio DSP, RAMTEXT bounds and 85% protection unchanged.
- User confirmed the representative physical power-cycle retention test, version label and boot screen on 2026-10-27; recorded separately from desktop evidence. Cross-firmware update/recovery remains unverified.


## Repository preparation — 2026-10-27 (firmware remains 0.3.1)

- Adopt CaspiusLabs/Emerald-YM as the canonical GitHub repository; retain full upstream history and immutable v0.1.4.
- English README, current user/install/build instructions, developer notes, release summaries and historical summaries.
- Move the inherited DUB-1 manual into the historical docs; do not present legacy web tools as Emerald features.
- Add fmhub.json, a 240×240 UI render link and an honest Firmware Hub readiness checklist. Repository remains private by owner request; no submission sent.
- Preserve the installed/tested 0.3.1 binary unchanged. No device operations during this documentation task.

## 0.3.1 — 2026-10-26 — encoder swap

- After user FX acceptance: PRESETS controls individual volume, ALGORITHM the edited FX amount. SELECT and FX unchanged.
- PRE / ALG tile labels updated. DSP/MIDI unchanged. Physical neutral-FX backing/live run:65%, XRUN 0, overload 0.

## 0.3.0 — 2026-10-26 — three simultaneous effects

- Independent DECAY/COLOR/ECHO for 12 instruments, initially 0; FX chooses the edited parameter without disabling others.
- Per-source sends to shared stereo echo (~186 ms), bounded feedback; FX LED/tile, standard CC20–22 on channels 1–12.
- EMERALD_STATIC_OPN skips unused SSG-EG/AM/PM. Generic comparison matches dry/FX hashes.
- 32 KiB pool buffers; critical FX code remains in the original guarded RAM range, without raising memory limits or the 85% threshold.
- 30 s continuous all-source FX25/100/100 + live:68%, XRUN 0, overload 0. Sustain run contains pauses, retains passed=false.
- User confirmed effects/independent values and requested the encoder swap in 0.3.1.

## 0.2.4 — 2026-10-26 — individual levels

- SELECT chooses one of 12 sources; ALGORITHM sets its independent 0–100% level. PRESETS still controls the percussion bus in this version.
- New instrument tile, isolated-source/live-mute tests; default audio matches 0.2.2 hash 7ce44fbe.
- PSG tuning table moves to setup-time XIP, hot oscillator state stays in RAM.
- Physical 30 s continuous + live:85%, max 4985 us, XRUN 0, overload 0. Very little margin. User confirmed independent controls.

## 0.2.3 — 2026-10-26 — FM/PSG/drum mixer

- SELECT = melodic FM0–100%; PRESETS = DAC drums/PSG noise 0–100%; ALGORITHM retains PSG mix.
- Three percentage/bar tiles replace CPU/XRUN and the hint. USB counters and 85% guard remain.
- FM scaling before DAC preserves unity audio; main-loop leaf controls move to XIP .ym_control, without enlarging RAM regions.
- Physical 30 s continuous + live:84%, XRUN 0, overload 0. User requested individual-source control next.

## 0.2.2 — 2026-10-26 — adjustable PSG mix

- ALGORITHM sets PSG 0–400%, default 300% (+9.54dB over 0.2.1); applies to arp/live/noise, not FM/DAC.
- On-screen value, changed-tile updates and volatile settings. Desktop 400% /three-live-note test without limiter hits.
- Physical 30 s at default 300% + live:84%, XRUN 0, overload 0. User accepted level and control.

## 0.2.1 — 2026-10-26 — musical arpeggio

- PSG arp +6 dB, one octave lower, longer gates, repeating chord tones 0–1–2–1.
- Interactive stress run retains passed=false after PLAY/REC. Separate 60 s backing-only pass, XRUN 0/overload 0; lifetime 84% includes earlier live notes.
- Test tool preserves partial transport-error reports and can require continuity. User found it more musical but still too quiet.

## 0.2.0 — 2026-10-26 — PSG and live keys

- Original Sega-style PSG: NTSC 3579545 Hz, three 10-bit tone dividers,2 dB attenuation,16-bit LFSR, white noise clock/512.
- Generative arp/noise and up to three live F3–G5 notes from panel/USB, including pause. Latest-note priority/return. ARP toggles all PSG.
- Dim backing/bright live LEDs. Five FM+DAC,256-frame blocks and 85% guard retained.
- Final interactive run:84%, XRUN 0,47/60 samples playing. Separate continuous run ended in USB timeout, not passed. Prototype 85% is separate evidence.

## 0.1.4 — 2026-10-26 — accepted baseline

- Real YM2612 emulation, five FM voices + DAC drums and generative sequencer.
- RAM/operator/output-channel/repeated-DAC-write optimizations retain NTSC clock and 85% guard.
- Changed-tile UI, four parameter cards, step strip, original Emerald mark/16-BIT badge, PLAY/REC/SEQ LEDs.
- USB diagnostics and MIDI Start/Continue/Stop. User accepted sound, no flicker, LEDs and controls.
- Interactive CPU 80%, XRUN 0, overload 0; raw continuity result remains false due to REC. Earlier 0.1.3 continuous test also 80% /XRUN 0.
- Standalone runtime/build/tests/docs, preserved package and real tag added without changing the firmware.

## Earlier experiments — 2026-10-26

- 0.1.0: first hardware boot; reported 92% /XRUN 0, protective stop.
- 0.1.1: code/tables moved to RAM; user reported that this alone did not solve overload.
- 0.1.2: five-FM+DAC optimization and USB status;87% /XRUN 0, protective stop.
- 0.1.3: inline hot clock functions; successful continuous test and listening confirmation.

These early stages predate the separate Emerald repository and are not fabricated as separate commits. Historical summaries and available traces remain in docs/history. The first actual Emerald commit imports the complete working 0.1.4 on the preserved DUB-1 history.
