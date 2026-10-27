# Refresh only the verified Emerald FM-1 audio devnode; no firmware or direct registry edits.
[CmdletBinding(SupportsShouldProcess=$true)]
param([string]$ReportPath)
$ErrorActionPreference = 'Stop'
if (-not $ReportPath) { $ReportPath = Join-Path $PSScriptRoot '..\build\windows-audio-refresh.json' }
$devices = @(Get-PnpDevice -PresentOnly -Class MEDIA | Where-Object {
    $_.InstanceId -match '^USB\\VID_1209&PID_0001&MI_00\\'
})
if ($devices.Count -ne 1) { throw 'Expected exactly one connected FM-1 audio interface; nothing changed.' }
$target = $devices[0]
function Read-Property([string]$Key) {
    (Get-PnpDeviceProperty -InstanceId $target.InstanceId -KeyName $Key -WhatIf:$false -ErrorAction Stop).Data
}
$busName = Read-Property 'DEVPKEY_Device_BusReportedDeviceDesc'
$service = Read-Property 'DEVPKEY_Device_Service'
$parent = Read-Property 'DEVPKEY_Device_Parent'
if ($busName -cne 'Emerald YM FM-1' -or $service -ne 'usbaudio' -or
    $parent -notmatch '^USB\\VID_1209&PID_0001\\') {
    throw 'USB product/service/parent is not the verified Emerald FM-1 audio interface; nothing changed.'
}
$report = [ordered]@{
    startedUtc = [DateTime]::UtcNow.ToString('o')
    instanceId = $target.InstanceId
    parent = $parent
    busReportedName = $busName
    oldFriendlyName = $target.FriendlyName
    service = $service
    driverInf = (Read-Property 'DEVPKEY_Device_DriverInfPath')
    performed = $false
    driverPackageRemoved = $false
    registryEdited = $false
}
if ($PSCmdlet.ShouldProcess($target.InstanceId, 'Re-enumerate the Emerald FM-1 audio interface (MIDI may disconnect briefly)')) {
    $principal = [Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw 'Run this script from PowerShell as Administrator. No device was removed.'
    }
    $reportDir = Split-Path -Parent ([IO.Path]::GetFullPath($ReportPath))
    New-Item -ItemType Directory -Path $reportDir -Force | Out-Null
    $report | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $ReportPath -Encoding utf8
    try {
        # Exact child instance only: never /subtree, /delete-driver or another interface.
        $removeOutput = & "$env:SystemRoot\System32\pnputil.exe" /remove-device $target.InstanceId 2>&1
        $removeCode = $LASTEXITCODE
        $report['removeOutput'] = @($removeOutput | ForEach-Object { "$_" })
        $report['removeExitCode'] = $removeCode
        if ($removeCode -notin @(0,3010)) { throw "PnP removal failed ($removeCode)." }
        $report['performed'] = $true
        $scanOutput = & "$env:SystemRoot\System32\pnputil.exe" /scan-devices 2>&1
        $scanCode = $LASTEXITCODE
        $report['scanOutput'] = @($scanOutput | ForEach-Object { "$_" })
        $report['scanExitCode'] = $scanCode
        if ($scanCode -notin @(0,3010)) { throw "PnP rescan failed ($scanCode). Reconnect the FM-1 USB cable." }
        $report['restartRequestedByWindows'] = ($removeCode -eq 3010 -or $scanCode -eq 3010)
        $until = (Get-Date).AddSeconds(15)
        do {
            Start-Sleep -Milliseconds 500
            $after = Get-PnpDevice -PresentOnly -InstanceId $target.InstanceId -ErrorAction SilentlyContinue
        } until ($after -or (Get-Date) -gt $until)
        $report['newFriendlyName'] = if ($after) { $after.FriendlyName } else { $null }
        $report['deviceReturned'] = [bool]$after
        $report['adapterNameMatches'] = [bool]($after -and $after.FriendlyName -ceq 'Emerald YM FM-1')
        if (-not $after) { throw 'Audio interface has not returned; reconnect the USB cable. No firmware was changed.' }
    } catch {
        $report['error'] = $_.Exception.Message
        throw
    } finally {
        $report['finishedUtc'] = [DateTime]::UtcNow.ToString('o')
        $report | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $ReportPath -Encoding utf8
    }
}
$report | ConvertTo-Json -Depth 5
