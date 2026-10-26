# Desktop tests only. Does not open MIDI or flash a device.
param([string]$Python="$PSScriptRoot\.runtime\python\python.exe")
$ErrorActionPreference='Stop'
Set-Location -LiteralPath $PSScriptRoot
New-Item -ItemType Directory -Path build\temp -Force | Out-Null
$env:TEMP=(Resolve-Path -LiteralPath build\temp).Path
$env:TMP=$env:TEMP
if(-not $env:AC79_SDK){$env:AC79_SDK=Join-Path $PSScriptRoot '.runtime\sdk'}
& tools\test-ym-windows.cmd 2>&1 | Tee-Object -FilePath build\host-tests.log
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
& tools\test-ui-windows.cmd 2>&1 | Tee-Object -FilePath build\ui-tests.log
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
& $Python tools\verify_ym_package.py 2>&1 | Tee-Object -FilePath build\package-verification.log
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
& $Python tests\install_test.py 2>&1 | Tee-Object -FilePath build\installer-tests.log
exit $LASTEXITCODE
