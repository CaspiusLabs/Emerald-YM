# Build only. This script does not open MIDI ports or flash a device.
param(
 [string]$Python = "$PSScriptRoot\..\tooling\python\python.exe",
 [string]$Toolchain = "$PSScriptRoot\..\tooling\jieli\C`$\JL",
 [string]$Sdk = "$PSScriptRoot\..\tooling\ac79-sdk",
 [string]$Version = '0.1.4'
)
$ErrorActionPreference='Stop'
Set-Location -LiteralPath $PSScriptRoot
$env:JIELI_TOOLCHAIN=(Resolve-Path -LiteralPath $Toolchain).Path
$env:AC79_SDK=(Resolve-Path -LiteralPath $Sdk).Path
$env:EN_FONT_NORAQM='1'
$env:X0X_FONTSET='terminus'
New-Item -ItemType Directory -Path build\temp -Force | Out-Null
$env:TEMP=(Resolve-Path -LiteralPath build\temp).Path
$env:TMP=$env:TEMP
& $Python tools\build.py --release $Version
exit $LASTEXITCODE
