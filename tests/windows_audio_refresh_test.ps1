# Safety tests: only mocked PnP reads and -WhatIf; no physical device operations.
$ErrorActionPreference = 'Stop'
$global:eymRefreshTestIds = @('USB\VID_1209&PID_0001&MI_00\test')
$global:eymRefreshTestBus = 'Emerald YM FM-1'
$global:eymRefreshTestService = 'usbaudio'
$global:eymRefreshTestParent = 'USB\VID_1209&PID_0001\test'
function Get-PnpDevice {
    param($Class, [switch]$PresentOnly)
    foreach ($id in $global:eymRefreshTestIds) { [pscustomobject]@{ InstanceId=$id; FriendlyName='Felucca' } }
}
function Get-PnpDeviceProperty {
    param($InstanceId,$KeyName,$ErrorAction,[switch]$WhatIf)
    $values=@{
        DEVPKEY_Device_BusReportedDeviceDesc=$global:eymRefreshTestBus
        DEVPKEY_Device_Service=$global:eymRefreshTestService
        DEVPKEY_Device_Parent=$global:eymRefreshTestParent
        DEVPKEY_Device_DriverInfPath='wdma_usb.inf'
    }
    [pscustomobject]@{Data=$values[$KeyName]}
}
$helper=Join-Path $PSScriptRoot '..\tools\refresh-fm1-windows-audio.ps1'
$r = (& $helper -WhatIf | Out-String | ConvertFrom-Json)
if ($r.performed -or $r.registryEdited -or $r.driverPackageRemoved) { throw 'WhatIf changed state' }
$cases=@('none','ambiguous','unrelated','wrong-product','wrong-service','wrong-parent')
foreach ($case in $cases) {
    $global:eymRefreshTestIds=@('USB\VID_1209&PID_0001&MI_00\test')
    $global:eymRefreshTestBus='Emerald YM FM-1'; $global:eymRefreshTestService='usbaudio'; $global:eymRefreshTestParent='USB\VID_1209&PID_0001\test'
    switch ($case) {
        none { $global:eymRefreshTestIds=@() }
        ambiguous { $global:eymRefreshTestIds+= 'USB\VID_1209&PID_0001&MI_00\other' }
        unrelated { $global:eymRefreshTestIds=@('USB\VID_0582&PID_012A\Roland') }
        wrong-product { $global:eymRefreshTestBus='Felucca' }
        wrong-service { $global:eymRefreshTestService='other' }
        wrong-parent { $global:eymRefreshTestParent='USB\VID_0582&PID_012A\Roland' }
    }
    $rejected=$false
    try { & $helper -WhatIf | Out-Null } catch { $rejected=$true }
    if (-not $rejected) { throw "Unsafe selection accepted: $case" }
    Write-Output "PASS refusal: $case"
}
Write-Output 'PASS valid WhatIf and all selection guards; no physical devices accessed.'
