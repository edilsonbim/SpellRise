[CmdletBinding()]
param(
    [string]$ProjectRoot = 'C:\Users\biM\Documents\Unreal Projects\SpellRise',
    [string]$UnrealRoot = 'C:\UnrealSource\UnrealEngine',
    [string]$Configuration = 'Development',
    [string]$Platform = 'Win64',
    [string]$OnlineProfile = 'test',
    [string]$ClientArchiveDir = '',
    [string]$ServerArchiveDir = '',
    [int]$TestDurationSeconds = 60,
    [switch]$NoBuild,
    [switch]$NoPackage,
    [switch]$NoParity,
    [switch]$NoSmoke,
    [switch]$NoSteam,
    [switch]$SkipLagLoss
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\SpellRise.Pipeline.Common.ps1"

$uproject = Join-Path $ProjectRoot 'SpellRise.uproject'
$buildBat = Join-Path $UnrealRoot 'Engine\Build\BatchFiles\Build.bat'
$runUatBat = Join-Path $UnrealRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$runDir = New-SpellRiseRunDirectory -ProjectRoot $ProjectRoot -PipelineName 'ReleaseGate'

if ([string]::IsNullOrWhiteSpace($ClientArchiveDir)) {
    $ClientArchiveDir = Join-Path $ProjectRoot 'Saved\Builds\Client'
}
if ([string]::IsNullOrWhiteSpace($ServerArchiveDir)) {
    $ServerArchiveDir = Join-Path $ProjectRoot 'Saved\Builds\Server'
}

$summary = [ordered]@{
    Pipeline = 'ReleaseGate'
    TimestampUtc = (Get-Date).ToUniversalTime().ToString('o')
    ProjectRoot = $ProjectRoot
    UnrealRoot = $UnrealRoot
    Configuration = $Configuration
    Platform = $Platform
    OnlineProfile = $OnlineProfile
    RunDirectory = $runDir
    Build = @()
    Package = @()
    Parity = $null
    Smoke = $null
    Status = 'PASS'
}

function Add-StepResult {
    param(
        [string]$Area,
        [string]$Name,
        [int]$ExitCode,
        [string]$LogPath
    )

    $entry = [ordered]@{
        Name = $Name
        ExitCode = $ExitCode
        LogPath = $LogPath
        Status = if ($ExitCode -eq 0) { 'PASS' } else { 'FAIL' }
    }

    $summary[$Area] += $entry
    if ($ExitCode -ne 0) {
        $summary.Status = 'FAIL'
    }
}

if (-not $NoBuild) {
    foreach ($target in @('SpellRiseEditor', 'SpellRiseClient', 'SpellRiseServer')) {
        $logPath = Join-Path $runDir "$target-build.log"
        $args = @(
            $target,
            $Platform,
            $Configuration,
            "`\"$uproject`\"",
            '-WaitMutex',
            '-NoHotReloadFromIDE'
        )

        $result = Invoke-SpellRiseProcess -FilePath $buildBat -Arguments $args -LogPath $logPath -WorkingDirectory $ProjectRoot
        Add-StepResult -Area 'Build' -Name $target -ExitCode $result.ExitCode -LogPath $logPath
    }
}

if (-not $NoPackage) {
    $clientLog = Join-Path $runDir 'SpellRiseClient-package.log'
    $clientArgs = @(
        'BuildCookRun',
        "-project=`\"$uproject`\"",
        '-noP4',
        '-client',
        "-clientconfig=$Configuration",
        "-platform=$Platform",
        '-build',
        '-cook',
        '-stage',
        '-pak',
        '-archive',
        "-archivedirectory=`\"$ClientArchiveDir`\""
    )
    $clientResult = Invoke-SpellRiseProcess -FilePath $runUatBat -Arguments $clientArgs -LogPath $clientLog -WorkingDirectory $ProjectRoot
    Add-StepResult -Area 'Package' -Name 'SpellRiseClient' -ExitCode $clientResult.ExitCode -LogPath $clientLog

    $serverLog = Join-Path $runDir 'SpellRiseServer-package.log'
    $serverArgs = @(
        'BuildCookRun',
        "-project=`\"$uproject`\"",
        '-noP4',
        '-server',
        "-serverconfig=$Configuration",
        "-platform=$Platform",
        '-build',
        '-cook',
        '-stage',
        '-pak',
        '-archive',
        "-archivedirectory=`\"$ServerArchiveDir`\""
    )
    $serverResult = Invoke-SpellRiseProcess -FilePath $runUatBat -Arguments $serverArgs -LogPath $serverLog -WorkingDirectory $ProjectRoot
    Add-StepResult -Area 'Package' -Name 'SpellRiseServer' -ExitCode $serverResult.ExitCode -LogPath $serverLog
}

$clientMeta = Join-Path $runDir 'SpellRiseClient.buildmeta.json'
$serverMeta = Join-Path $runDir 'SpellRiseServer.buildmeta.json'
Write-SpellRiseJsonFile -Path $clientMeta -Object (Get-SpellRiseBuildMetadata -ProjectRoot $ProjectRoot -TargetName 'SpellRiseClient' -Configuration $Configuration -Platform $Platform -OnlineProfile $OnlineProfile)
Write-SpellRiseJsonFile -Path $serverMeta -Object (Get-SpellRiseBuildMetadata -ProjectRoot $ProjectRoot -TargetName 'SpellRiseServer' -Configuration $Configuration -Platform $Platform -OnlineProfile $OnlineProfile)

if (-not $NoParity) {
    $parityScript = Join-Path $PSScriptRoot 'Test-SpellRise-PackageParity.ps1'
    $parityLog = Join-Path $runDir 'package-parity.log'
    $parityResult = & $parityScript `
        -ProjectRoot $ProjectRoot `
        -ClientPackageDir $ClientArchiveDir `
        -ServerPackageDir $ServerArchiveDir `
        -ClientMetadataPath $clientMeta `
        -ServerMetadataPath $serverMeta `
        -OutputLogPath $parityLog

    $summary.Parity = $parityResult
    if ($parityResult.Status -ne 'PASS') {
        $summary.Status = 'FAIL'
    }
}

if (-not $NoSmoke) {
    $smokeScript = Join-Path $ProjectRoot 'Scripts\Run-Smoke-Gate.ps1'
    if (-not (Test-Path $smokeScript)) {
        $summary.Smoke = @{ Status = 'MANUAL_REQUIRED'; Reason = "Script ausente: $smokeScript" }
        if ($summary.Status -eq 'PASS') { $summary.Status = 'MANUAL_REQUIRED' }
    }
    else {
        $smokeLog = Join-Path $runDir 'smoke-gate.log'
        $smokeArgs = @('-NoBuild', '-TestDurationSeconds', "$TestDurationSeconds")
        if ($NoSteam) { $smokeArgs += '-NoSteam' }
        if ($SkipLagLoss) { $smokeArgs += '-SkipLagLoss' }

        $smokeResult = Invoke-SpellRiseProcess -FilePath 'powershell.exe' -Arguments @(
            '-ExecutionPolicy', 'Bypass', '-File', "`\"$smokeScript`\""
        ) + $smokeArgs -LogPath $smokeLog -WorkingDirectory $ProjectRoot

        $summary.Smoke = @{ ExitCode = $smokeResult.ExitCode; LogPath = $smokeLog; Status = if ($smokeResult.ExitCode -eq 0) { 'PASS' } else { 'FAIL' } }
        if ($smokeResult.ExitCode -ne 0) { $summary.Status = 'FAIL' }
    }
}

Write-SpellRiseJsonFile -Path (Join-Path $runDir 'summary.json') -Object $summary

$lines = @()
$lines += 'Pipeline: ReleaseGate'
$lines += "Status: $($summary.Status)"
$lines += "RunDirectory: $runDir"
$lines += ''
$lines += 'Build:'
foreach ($entry in $summary.Build) { $lines += "- $($entry.Name): $($entry.Status) ($($entry.LogPath))" }
$lines += ''
$lines += 'Package:'
foreach ($entry in $summary.Package) { $lines += "- $($entry.Name): $($entry.Status) ($($entry.LogPath))" }
$lines += ''
if ($null -ne $summary.Parity) {
    $lines += "PackageParity: $($summary.Parity.Status)"
    if ($summary.Parity.Reasons) { foreach ($reason in $summary.Parity.Reasons) { $lines += "- $reason" } }
}
if ($null -ne $summary.Smoke) {
    $lines += ''
    $lines += "Smoke: $($summary.Smoke.Status)"
    if ($summary.Smoke.LogPath) { $lines += "- Log: $($summary.Smoke.LogPath)" }
    if ($summary.Smoke.Reason) { $lines += "- $($summary.Smoke.Reason)" }
}
Write-SpellRiseTextFile -Path (Join-Path $runDir 'summary.txt') -Content ($lines -join [Environment]::NewLine)

if ($summary.Status -eq 'FAIL') { exit 1 }
if ($summary.Status -eq 'MANUAL_REQUIRED') { exit 2 }
exit 0
