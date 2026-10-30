# GLO illumination — development 0.11.2

Owner-requested change on a separate branch based on unmerged PR #12, exact source `654c99115b90812cfd154b62246b203a3b2a9dcf`. GLO now publishes its dim LED bit while physical audio is enabled, and its bright bit in USB ONLY. Returning to normal audio clears the bright bit and restores dim. Complete-row publication and the existing dim scanning divisor remain unchanged. There is no audio, routing, settings, USB or memory-map change.

AGENTS.md records the owner's rule for future work: every available assigned panel function must have visible illumination, dim when inactive and bright when active or acknowledging an action; preserve meaningful transport/error colors.

Native build and full existing desktop tests pass, including updated production LED assertions, native UI, output fade/mute, MIDI, audio, settings and simulated OTA/loader/installer checks. See [summary](summary.json), [build log](build.txt.gz), [desktop checks](tests.txt.gz). The optional inherited JavaScript package comparison is skipped as before. No new test harness is added for this small LED change.

**No device accessed or flashed in this task.** Installed exact 0.11.1 and public release 0.10.0 remain unchanged. Previous hardware timing and diagnostic results do not validate 0.11.2's physical LEDs or performance. No release/tag or automatic merge. This PR includes the unmerged PR #12 ancestry and should be reviewed/merged after that dependency.
