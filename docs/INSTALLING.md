# Installing and leaving Emerald YM

Target: M-VAVE FM-1 only. Current package: `emerald-ym-0.6.0.fwsc`, identity `FM-1_68000600`, USB MIDI name `Emerald YM FM-1`. This is beta firmware. Read the [test scope](VERIFICATION.md) before installing.

## Back up first

Emerald does not import stock or other custom firmware's presets, projects or samples. It does not provide a backup/restore tool for them and does not promise that switching back recovers them. Use your current firmware's backup tools first. From 0.4.0, instrument levels/FX are saved to the existing FC000/FD000 project-sector pair while paused; this may overwrite another firmware's data there. K1–K4 are persistent since 0.4.2; seed, master, PSG enable and selection/cursor remain outside the snapshot. Installation and recovery also write flash.

## Get and check the package

Download [emerald-ym-0.6.0.fwsc](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v0.6.0/emerald-ym-0.6.0.fwsc) and [SHA256SUMS](https://github.com/CaspiusLabs/Emerald-YM/releases/download/v0.6.0/SHA256SUMS) from the public [0.6.0 beta release](https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v0.6.0). Choose the `.fwsc` asset, not a source archive. This is the exact physically tested file, with no rebuild; tag `v0.6.0` points to matching source commit `c0530a6cd7d026554e21b957665edfd7fe7d51d9`. Documentation/source version 0.6.2 does not identify a new firmware image. The 0.6.0 recording/MIDI/status and screen tests are separate from the older 0.4.3 V15/Dinghy round trips. No Hub browser installation is claimed.

Expected SHA-256:

```text
ea7a5bff5fcf2671debad1ee32f13b50843d6f51d7fe879f14798097a56fbe89
```

On PowerShell:

```powershell
Get-FileHash .\emerald-ym-0.6.0.fwsc -Algorithm SHA256
```

The package includes the inherited Felucca update loader. Do not substitute a raw application `.bin` for it.

## Command-line installation

Use Python with `mido` and `python-rtmidi`, from a clone of this repository:

```powershell
python -m pip install mido==1.3.3 python-rtmidi==1.5.8
python -c "import mido; print('\n'.join(mido.get_output_names()))"
```

Connect the FM-1 with a USB data cable. Verify its exact MIDI port; exclude unrelated interfaces such as Roland UM-ONE. Check identity first, then install with the same verified port substring:

```powershell
python tools/fm1_install.py --info --port "YOUR VERIFIED FM-1 PORT"
python tools/fm1_install.py .\emerald-ym-0.6.0.fwsc --port "YOUR VERIFIED FM-1 PORT"
```

The installer asks before writing. Keep power and USB connected until it reports completion. Do not use `--force` to skip the loader check. After reboot, verify `FM-1_68000600`, turn MASTER low and press PLAY.

## Leaving Emerald and recovery

The running application retains the Felucca update handler. Install another compatible, checked `.fwsc` with the same installer, selecting `Emerald YM FM-1` as the current port. **Physical round trips passed on 2026-10-27:** Emerald 0.4.3 ↔ official V15 and Emerald 0.4.3 ↔ Dinghy 0.3, using normal USB MIDI updates with no rescue shortcut. [Exact packages, user observations and reports](verification/install-roundtrips-2026-10-27/README.md). This is not a guarantee for arbitrary versions or interrupted writes.

The inherited OCT− + OCT+ hold (five seconds) enters update mode. There is no guaranteed automatic rollback. If normal updating fails, consult the [Firmware Hub recovery guidance](https://thefirmwarehub.com/kb/) and the official M-VAVE recovery instructions for your device. Do not intentionally interrupt a write as part of ordinary installation.

No public Emerald browser installer/player is provided yet. Files under `web/` are inherited DUB-1 tools, not a supported Emerald installation page.

## Persistent settings since 0.4.2

PRs #1–#3 are merged. Accepted 0.4.3 includes the K1–K4 persistence introduced in 0.4.2. Schema 1 from 0.4.0 keeps saved mixer/FX values; schema 2 adds tempo/density/mutation/transpose. Migration does not write on boot. Save after an actual edit and wait for SAVED before power-off. Older 0.4.0 cannot load schema 2 and uses defaults when downgraded.

For returning to official V15, use M-VAVE's updater or the Firmware Hub's official-image path with the genuine stock file. The generic custom-package CLI loader-marker check is not a reason to accept arbitrary markerless files. Our audit used only the exact manufacturer image/hash through its documented stock-update path.
