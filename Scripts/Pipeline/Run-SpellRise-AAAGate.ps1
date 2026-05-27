[CmdletBinding()]
param(
    [string]$ProjectRoot = 'C:\Users\biM\Documents\Unreal Projects\SpellRise',
    [string]$UnrealRoot = 'C:\UnrealSource\UnrealEngine',
    [ValidateSet('Auto', 'DocsOnly', 'Build', 'NetCritical', 'Persistence', 'Combat', 'Release')]
    [string]$GateMode = 'Auto',
    [string]$Configuration = 'Development',
    [string]$Platform = 'Win64',
    [string]$OnlineProfile = 'test',
    [int]$TestDurationSeconds = 60,
    [switch]$NoSteam,
    [switch]$SkipSmoke,
    [switch]$SkipLagLoss,
    [switch]$AllowHighInProgressBugLog
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\SpellRise.Pipeline.Common.ps1"

$uproject = Join-Path $ProjectRoot 'SpellRise.uproject'
$buildBat = Join-Path $UnrealRoot 'Engine\Build\BatchFiles\Build.bat'
$runDir = New-SpellRiseRunDirectory -ProjectRoot $ProjectRoot -PipelineName 'AAAOperationalGate'

$summary = [ordered]@{
    Pipeline = 'AAAOperationalGate'
    TimestampUtc = (Get-Date).ToUniversalTime().ToString('o')
    ProjectRoot = $ProjectRoot
    UnrealRoot = $UnrealRoot
    RequestedGateMode = $GateMode
    EffectiveGateMode = $GateMode
    Configuration = $Configuration
    Platform = $Platform
    OnlineProfile = $OnlineProfile
    RunDirectory = $runDir
    ChangedFiles = @()
    Steps = @()
    Findings = @()
    Status = 'PASS'
}

function Add-AAAFinding {
    param(
        [Parameter(Mandatory = $true)][string]$Severity,
        [Parameter(Mandatory = $true)][string]$Message
    )

    $summary.Findings += [ordered]@{
        Severity = $Severity
        Message = $Message
    }

    if ($Severity -eq 'ERROR') {
        $summary.Status = 'FAIL'
    }
}

function Add-AAAStep {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][int]$ExitCode,
        [string]$LogPath = '',
        [string]$Notes = ''
    )

    $summary.Steps += [ordered]@{
        Name = $Name
        ExitCode = $ExitCode
        Status = $(if ($ExitCode -eq 0) { 'PASS' } else { 'FAIL' })
        LogPath = $LogPath
        Notes = $Notes
    }

    if ($ExitCode -ne 0) {
        $summary.Status = 'FAIL'
    }
}

function Get-AAAChangedFiles {
    param([string]$Root)

    $gitDir = Join-Path $Root '.git'
    if (-not (Test-Path $gitDir)) {
        return @()
    }

    $files = @()
    Push-Location $Root
    try {
        $files += (& git diff --name-only HEAD 2>$null)
        $files += (& git ls-files --others --exclude-standard 2>$null)
    }
    finally {
        Pop-Location
    }

    return @($files | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Sort-Object -Unique)
}

function Resolve-AAAGateMode {
    param(
        [string]$RequestedMode,
        [string[]]$ChangedFiles
    )

    if ($RequestedMode -ne 'Auto') {
        return $RequestedMode
    }

    if ($ChangedFiles.Count -eq 0) {
        return 'Build'
    }

    $nonDocFiles = @($ChangedFiles | Where-Object {
        ($_ -notlike '*.md') -and
        ($_ -notlike 'Docs/*') -and
        ($_ -notlike 'Fontes/*') -and
        ($_ -notlike '.github/*')
    })

    if ($nonDocFiles.Count -eq 0) {
        return 'DocsOnly'
    }

    if ($ChangedFiles | Where-Object { $_ -match '(^|/)(Persistence|Inventory|Loot|Economy|Database|Steam|Online|Auth)(/|\.|$)' }) {
        return 'Persistence'
    }

    if ($ChangedFiles | Where-Object { $_ -match '(PlayerController|Ability|GameplayAbility|GameplayEffect|AttributeSet|ExecCalc|MMC|Projectile|ConstructionMode|Replication|RPC|Combat|Input)' }) {
        return 'NetCritical'
    }

    if ($ChangedFiles | Where-Object { $_ -match '^Content/GamePlayAbilitySystem/' }) {
        return 'NetCritical'
    }

    return 'Build'
}

function Invoke-AAAProcess {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][string]$FilePath,
        [Parameter(Mandatory = $true)][string[]]$Arguments,
        [Parameter(Mandatory = $true)][string]$LogName
    )

    $logPath = Join-Path $runDir $LogName
    $result = Invoke-SpellRiseProcess -FilePath $FilePath -Arguments $Arguments -LogPath $logPath -WorkingDirectory $ProjectRoot
    Add-AAAStep -Name $Name -ExitCode $result.ExitCode -LogPath $logPath
    return $result.ExitCode
}

function Test-AAARequiredPath {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Label
    )

    if (-not (Test-Path $Path)) {
        Add-AAAFinding -Severity 'ERROR' -Message "$Label ausente: $Path"
        return $false
    }

    return $true
}

function Test-AAALogsForForbiddenPattern {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Pattern,
        [Parameter(Mandatory = $true)][string]$Label
    )

    if (-not (Test-Path $Root)) {
        return
    }

    $matches = Get-ChildItem -Path $Root -File -Recurse -ErrorAction SilentlyContinue |
        Where-Object { $_.Extension -in @('.log', '.txt') } |
        Select-String -Pattern $Pattern -SimpleMatch -ErrorAction SilentlyContinue |
        Select-Object -First 1

    if ($null -ne $matches) {
        Add-AAAFinding -Severity 'ERROR' -Message "$Label encontrado em $($matches.Path):$($matches.LineNumber)"
    }
}

$requiredDocs = @(
    'Fontes\PROJECT_STATE.md',
    'AGENTS.md',
    'Fontes\ARCHITECTURE.md',
    'Fontes\ATTRIBUTE_MATRIX.md',
    'Fontes\MULTIPLAYER_RULES.md',
    'Fontes\SECURITY_MODEL.md',
    'Fontes\ENGINEERING_PLAYBOOK.md'
)

foreach ($doc in $requiredDocs) {
    [void](Test-AAARequiredPath -Path (Join-Path $ProjectRoot $doc) -Label "Documento fonte")
}

[void](Test-AAARequiredPath -Path $uproject -Label 'SpellRise.uproject')
[void](Test-AAARequiredPath -Path $UnrealRoot -Label 'Unreal Source')

$summary.ChangedFiles = @(Get-AAAChangedFiles -Root $ProjectRoot)
$summary.EffectiveGateMode = Resolve-AAAGateMode -RequestedMode $GateMode -ChangedFiles $summary.ChangedFiles

if ($summary.Status -eq 'FAIL') {
    Write-SpellRiseJsonFile -Path (Join-Path $runDir 'summary.json') -Object $summary
    Write-SpellRiseTextFile -Path (Join-Path $runDir 'summary.txt') -Content 'AAAOperationalGate: FAIL em preflight.'
    exit 1
}

if ($summary.EffectiveGateMode -eq 'DocsOnly') {
    Add-AAAStep -Name 'DocsOnly preflight' -ExitCode 0 -Notes 'Alteracao classificada como documentacao/instrucoes.'
}
else {
    [void](Test-AAARequiredPath -Path $buildBat -Label 'Build.bat da Unreal Source')
    if ($summary.Status -eq 'FAIL') {
        Write-SpellRiseJsonFile -Path (Join-Path $runDir 'summary.json') -Object $summary
        Write-SpellRiseTextFile -Path (Join-Path $runDir 'summary.txt') -Content 'AAAOperationalGate: FAIL em preflight de build.'
        exit 1
    }
}

switch ($summary.EffectiveGateMode) {
    'DocsOnly' {
        break
    }
    'Build' {
        $args = @('SpellRiseEditor', $Platform, $Configuration, ('"{0}"' -f $uproject), '-WaitMutex', '-NoHotReloadFromIDE')
        [void](Invoke-AAAProcess -Name 'Build SpellRiseEditor' -FilePath $buildBat -Arguments $args -LogName 'SpellRiseEditor-build.log')
        break
    }
    'NetCritical' {
        $args = @('SpellRiseEditor', $Platform, $Configuration, ('"{0}"' -f $uproject), '-WaitMutex', '-NoHotReloadFromIDE')
        [void](Invoke-AAAProcess -Name 'Build SpellRiseEditor' -FilePath $buildBat -Arguments $args -LogName 'SpellRiseEditor-build.log')

        if (-not $SkipSmoke.IsPresent -and $summary.Status -eq 'PASS') {
            $smokeScript = Join-Path $ProjectRoot 'Scripts\Run-Smoke-Gate.ps1'
            $smokeArgs = @('-ExecutionPolicy', 'Bypass', '-File', ('"{0}"' -f $smokeScript), '-NoBuild', '-TestDurationSeconds', "$TestDurationSeconds")
            if ($NoSteam.IsPresent) { $smokeArgs += '-NoSteam' }
            if ($SkipLagLoss.IsPresent) { $smokeArgs += '-SkipLagLoss' }
            if ($AllowHighInProgressBugLog.IsPresent) { $smokeArgs += '-AllowHighInProgressBugLog' }
            [void](Invoke-AAAProcess -Name 'SmokeGate DS+2' -FilePath 'powershell.exe' -Arguments $smokeArgs -LogName 'smoke-gate.log')
        }
        break
    }
    'Persistence' {
        $script = Join-Path $ProjectRoot 'Scripts\Pipeline\Run-SpellRise-PersistenceGate.ps1'
        $args = @('-ExecutionPolicy', 'Bypass', '-File', ('"{0}"' -f $script), '-TestDurationSeconds', "$TestDurationSeconds")
        if ($NoSteam.IsPresent) { $args += '-NoSteam' }
        if ($SkipLagLoss.IsPresent) { $args += '-SkipLagLoss' }
        [void](Invoke-AAAProcess -Name 'PersistenceGate' -FilePath 'powershell.exe' -Arguments $args -LogName 'persistence-gate.log')
        break
    }
    'Combat' {
        $script = Join-Path $ProjectRoot 'Scripts\Pipeline\Run-SpellRise-CombatLab.ps1'
        $args = @('-ExecutionPolicy', 'Bypass', '-File', ('"{0}"' -f $script), '-RunSmokeGate')
        if ($NoSteam.IsPresent) { $args += '-NoSteam' }
        [void](Invoke-AAAProcess -Name 'CombatLab' -FilePath 'powershell.exe' -Arguments $args -LogName 'combat-lab.log')
        break
    }
    'Release' {
        $script = Join-Path $ProjectRoot 'Scripts\Pipeline\Run-SpellRise-ReleaseGate.ps1'
        $args = @('-ExecutionPolicy', 'Bypass', '-File', ('"{0}"' -f $script), '-Configuration', $Configuration, '-Platform', $Platform, '-OnlineProfile', $OnlineProfile, '-TestDurationSeconds', "$TestDurationSeconds")
        if ($NoSteam.IsPresent) { $args += '-NoSteam' }
        if ($SkipLagLoss.IsPresent) { $args += '-SkipLagLoss' }
        [void](Invoke-AAAProcess -Name 'ReleaseGate' -FilePath 'powershell.exe' -Arguments $args -LogName 'release-gate.log')
        break
    }
}

Test-AAALogsForForbiddenPattern -Root $runDir -Pattern 'FBitReader::SetOverflowed' -Label 'Overflow de replicacao'
Test-AAALogsForForbiddenPattern -Root (Join-Path $ProjectRoot 'Saved\Logs\SmokeAuto') -Pattern 'FBitReader::SetOverflowed' -Label 'Overflow de replicacao em smoke'

$lines = @()
$lines += 'Pipeline: AAAOperationalGate'
$lines += "Status: $($summary.Status)"
$lines += "RequestedGateMode: $($summary.RequestedGateMode)"
$lines += "EffectiveGateMode: $($summary.EffectiveGateMode)"
$lines += "RunDirectory: $runDir"
$lines += ''
$lines += 'Steps:'
foreach ($step in $summary.Steps) {
    $lines += "- $($step.Name): $($step.Status)"
    if (-not [string]::IsNullOrWhiteSpace($step.LogPath)) { $lines += "  Log: $($step.LogPath)" }
    if (-not [string]::IsNullOrWhiteSpace($step.Notes)) { $lines += "  Notes: $($step.Notes)" }
}
$lines += ''
$lines += 'Findings:'
if ($summary.Findings.Count -eq 0) {
    $lines += '- none'
}
else {
    foreach ($finding in $summary.Findings) {
        $lines += "- [$($finding.Severity)] $($finding.Message)"
    }
}

Write-SpellRiseJsonFile -Path (Join-Path $runDir 'summary.json') -Object $summary
Write-SpellRiseTextFile -Path (Join-Path $runDir 'summary.txt') -Content ($lines -join [Environment]::NewLine)

if ($summary.Status -ne 'PASS') {
    exit 1
}

exit 0
