# Installing and leaving Emerald YM

Target: M-VAVE FM-1 only. Current package: `emerald-ym-0.4.0.fwsc`, identity `FM-1_68000400`, USB MIDI name `Emerald YM FM-1`. This is beta firmware. Read the [test scope](VERIFICATION.md) before installing.

## Back up first

Emerald does not import stock or other custom firmware's presets, projects or samples. It does not provide a backup/restore tool for them and does not promise that switching back recovers them. Use your current firmware's backup tools first. From 0.4.0, instrument levels/FX are saved to the existing FC000/FD000 project-sector pair while paused; this may overwrite another firmware's data there. Other performance settings remain volatile. Installation and recovery also write flash.

## Get and check the package

Download the `.fwsc` and `SHA256SUMS` from the [0.4.0 release](https://github.com/CaspiusLabs/Emerald-YM/releases/tag/v0.4.0). While this repository is private, GitHub requires authorized access; it is not yet installable from a public Firmware Hub card.

Expected SHA-256:

```text
0f9d39e6cb5158fbc066ad63dfdc8932ab103014f1d0b4218acc5d88b0d3c6af
```

On PowerShell:

```powershell
Get-FileHash .\emerald-ym-0.4.0.fwsc -Algorithm SHA256
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
python tools/fm1_install.py .\emerald-ym-0.4.0.fwsc --port "YOUR VERIFIED FM-1 PORT"
```

The installer asks before writing. Keep power and USB connected until it reports completion. Do not use `--force` to skip the loader check. After reboot, verify `FM-1_68000400`, turn MASTER low and press PLAY.

## Leaving Emerald and recovery

The running application retains the Felucca update handler. Install another compatible, checked `.fwsc` with the same installer, selecting `Emerald YM FM-1` as the current port. **The release-specific physical round trip to another firmware and the V15 → Emerald path still need recorded verification**; simulations and successful Emerald-to-Emerald updates do not replace those checks.

The inherited OCT− + OCT+ hold (five seconds) enters update mode. There is no guaranteed automatic rollback. If normal updating fails, consult the [Firmware Hub recovery guidance](https://thefirmwarehub.com/kb/) and the official M-VAVE recovery instructions for your device. Do not intentionally interrupt a write as part of ordinary installation.

No public Emerald browser installer/player is provided yet. Files under `web/` are inherited DUB-1 tools, not a supported Emerald installation page.
