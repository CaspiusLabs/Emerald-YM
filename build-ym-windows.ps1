# Build only. This script does not open MIDI ports or flash a device.
param(
 [string]$Python = "$PSScriptRoot\.runtime\python\python.exe",
 [string]$Toolchain = "$PSScriptRoot\.runtime\toolchain",
 [string]$Sdk = "$PSScriptRoot\.runtime\sdk",
 [string]$Version = ''
)
$ErrorActionPreference='Stop'
Set-Location -LiteralPath $PSScriptRoot
if(-not $Version){$Version=(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'VERSION') -Raw).Trim()}
$env:JIELI_TOOLCHAIN=(Resolve-Path -LiteralPath $Toolchain).Path
$env:AC79_SDK=(Resolve-Path -LiteralPath $Sdk).Path
$env:EN_FONT_NORAQM='1'
$env:X0X_FONTSET='terminus'
New-Item -ItemType Directory -Path build\temp -Force | Out-Null
$env:TEMP=(Resolve-Path -LiteralPath build\temp).Path
$env:TMP=$env:TEMP
& $Python tools\build.py --release $Version 2>&1 | Tee-Object -FilePath (Join-Path $PSScriptRoot 'build\build-ym.log')
exit $LASTEXITCODE
