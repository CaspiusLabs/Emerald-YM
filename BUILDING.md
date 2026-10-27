# Building Emerald YM on Windows

The supported, tested build environment is Windows with the official JieLi toolchain. `VERSION` is the version source; the output is `build/emerald-ym-<VERSION>.fwsc`. Neither build nor desktop tests opens MIDI or installs firmware.

## Dependencies

Dependencies are **not committed or included in a clone**. The original development machine has them in ignored `.runtime/`. Provision them yourself using [tools/runtime-manifest.json](tools/runtime-manifest.json):

| Path | Dependency |
|---|---|
| `.runtime/python/python.exe` | Python 3.12.10, Pillow 13.0.0, mido 1.3.3, python-rtmidi 1.5.8, packaging 26.3 |
| `.runtime/toolchain/pi32/bin/` | Official JieLi Windows 2.5.3, clang 4.0.1, pi32v2 r3 |
| `.runtime/sdk/` | Three AC79NN SDK V1.2.1 build inputs listed below |
| Installed separately | Visual Studio / Build Tools C++ tools, detected through `vswhere`; needed for desktop tests |

Get [Python](https://www.python.org/ftp/python/3.12.10/), the [JieLi toolchain](https://pkgman.jieliapp.com/s/win-toolchain) and the [AC79 SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK). With a regular Python installation, install the packages through that interpreter's `-m pip install` command. An embedded Python installation requires its package/import paths to be configured; a fresh clone does not bootstrap them automatically.

Copy these SDK files to the paths expected under `.runtime/sdk/`. The build verifies their exact reference hashes (see the manifest):

- `uboot.boot`
- `cfg_tool.bin`
- `cfg/eq_cfg_hw.bin`

The compiler and Python are not redistributed here. No WSL or Docker is required for the tested Windows route. This project does not yet claim a verified GitHub-hosted firmware build or artifact attestation.

## Build and test

From the repository root:

```powershell
.\build-ym-windows.ps1
.\test-ym-windows.ps1
```

On Windows PowerShell where script policy blocks local scripts:

```powershell
powershell -ExecutionPolicy Bypass -File .\build-ym-windows.ps1
powershell -ExecutionPolicy Bypass -File .\test-ym-windows.ps1
```

Build paths can be overridden:

```powershell
.\build-ym-windows.ps1 -Python C:\tools\python\python.exe -Toolchain C:\tools\jieli -Sdk C:\tools\ac79
```

See the test wrapper parameters before overriding its paths. Logs, WAV renders and UI previews go to ignored `build/`. Building an already preserved release must reproduce its `.fwsc` byte for byte. Never replace the v0.1.4 package/tag; a behavioral change needs a new version and release directory.

## Checks and limits

The build checks image and RAM sizes, linker ranges, RAM copying/PC-guard coverage, flash-off code and absence of soft-double routines. Desktop tests cover deterministic streaming, source isolation, effects, no render-time allocation, signal bounds, real UI/LED code, OTA/loader simulation and a mock installer transport. They do not establish real-time performance on the FM-1.

`tools/test-ym-generic-windows.cmd` restores generic operator paths for comparison with the optimized static profile. Do not use inherited DUB-1 scenarios in `tests/run_tests.sh` or `host/build_host.sh` as evidence for the current application.

The source tree retains historical DUB-1 browser/host tools. Their build scripts are not the Emerald quickstart or a supported Emerald web release.

## Source export and history

After committing, export an exact ref with:

```powershell
.runtime\python\python.exe tools\export_ym.py --ref v0.1.4
```

The ignored `dist/` receives a source archive, a local Git bundle and checksums. Runtime dependencies are excluded. For the current source, use its published release tag or commit instead. GitHub's source archives also refer to the release tag.

Installation is a separate, explicit operation: see [docs/INSTALLING.md](docs/INSTALLING.md). Update simulations are not physical install/recovery tests. Historical physical evidence and remaining checks are in [docs/VERIFICATION.md](docs/VERIFICATION.md).

## Windows audio registration helper tests

Run `.\tests\windows_audio_refresh_test.ps1` in PowerShell 7 or Windows PowerShell 5.1. These tests mock PnP reads and use WhatIf to verify selection/refusal behavior; they do not touch physical devices or require Administrator rights. The actual refresh helper is an explicit maintenance operation, never part of building or ordinary tests. See [Windows audio naming](docs/WINDOWS-AUDIO-NAME.md).
