# Physical installation round trips — 2026-10-27

Tested on the owner's M-VAVE FM-1 through normal USB MIDI updating. Exact Emerald package: **0.4.3**, SHA-256 `6669787e1d7a3b56f37bd0494c992fbdba5cb59e5dc13d5f2380d7c4fbd64925`, from merged main commit `b5848cd40b4d8dda4612cb9b8c3dc9687e29abc6`. Documentation source version 0.4.4 adds this evidence; no firmware 0.4.4 was built or flashed.

| Source | Target | Result | Evidence |
|---|---|---|---|
| Emerald 0.4.3 | Official M-VAVE V15 | Passed | [Transition 01](01-emerald-to-v15.json) |
| Official V15 | Emerald 0.4.3 | Passed | [Transition 02](02-v15-to-emerald.json) |
| Emerald 0.4.3 | Dinghy 0.3 | Passed | [Transition 03](03-emerald-to-dinghy.json) |
| Dinghy 0.3 | Emerald 0.4.3 | Passed | [Transition 04](04-dinghy-to-emerald.json) |

Each step began with a matching running-application identity, used the normal M-UPGRADE handler to start the target update loader, completed its transfer, rebooted and matched the target identity. No manual power cycle, OCT shortcut or ROM/USB rescue was needed. The backend allowed only a unique FM-1-family input/output pair and explicitly rejected Roland UM-ONE and Microsoft synth ports; opened-port names are recorded per step.

The owner confirmed ordinary screens and audible keys in V15 and Dinghy (raw replies in [user observations](user-observations.json)). After each return, Emerald passed 20 seconds of continuous backing playback/status monitoring: CPU MAX 67%, XRUNS 0, overload 0, failed boots 0. Longest blocks: 3929 us after V15 and 3922 us after Dinghy. These tests did not change effect CCs or assume neutral settings. Audio/status traces are separate from the transition traces.

## Packages and method

[Package manifest](package-manifest.json) pins URLs, identities and SHA-256 values. Official V15 was downloaded unchanged from [M-VAVE's pinned repository](https://github.com/m-vave/fm1_open/blob/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41/sdk/firmware/FM-1_015.fwsc); its hash matches the known stock image in tools/fm1_rescue.py and the inherited web stock installer. This is stock V15, not a build of FM1 Open V14. It has its manufacturer loader rather than the Felucca marker. The runner accepted only that exact hash through the existing stock-update method documented in web/amb_installer.html; no generic --force option was used. Dinghy came from the original [v0.3 release](https://github.com/zvenson/dinghy/releases/tag/v0.3), with the GitHub asset digest verified. No third-party firmware binaries are included in this evidence directory.

The executed runner is archived as [text](executed-runner.py.txt), with its original execution location `build/temp/run_install_transition.py`; it imports the repository installer library. It is an audit artifact, not a replacement general-purpose installer. Raw event logs and machine-readable reports are retained, including all opened MIDI port names and target checks. [Summary](summary.json).

## Scope

This proves these four normal update paths for this device and these exact package bytes. It does not test arbitrary versions/models, interrupted writes, power-loss recovery, preservation/import of other firmware's data, every parameter or long-session reliability. The Firmware Hub browser Connect/Install path was not exercised. The repository remains private; no submission email or new release tag was created. Earlier package reports and failed interactive traces remain unchanged.
