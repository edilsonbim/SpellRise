[CmdletBinding()]
param(
    [string]$ProjectRoot = 'C:\Users\biM\Documents\Unreal Projects\SpellRise',
    [string[]]$ScenarioIds = @('CL-007', 'CL-008', 'CL-012'),
    [switch]$RunSmokeGate,
    [switch]$NoSteam,
    [switch]$SkipLagLoss,
    [int]$TestDurationSeconds = 60
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\SpellRise.Pipeline.Common.ps1"

$runDir = New-SpellRiseRunDirectory -ProjectRoot $ProjectRoot -PipelineName 'CombatLab'
$matrix = New-Object System.Collections.Generic.List[object]

$scenarioCatalog = @{
    'CL-001' = 'Formula sanity - melee'
    'CL-002' = 'Formula sanity - bow'
    'CL-003' = 'Formula sanity - spell'
    'CL-004' = 'TTK envelope - light armor'
    'CL-005' = 'TTK envelope - medium armor'
    'CL-006' = 'TTK envelope - heavy armor'
    'CL-007' = 'Projectile authority'
    'CL-008' = 'Predicted ability reconciliation'
    'CL-009' = 'Resource and cooldown integrity'
    'CL-010' = 'Fall damage'
    'CL-011' = 'Death, loot, respawn'
    'CL-012' = 'Owner vs remote presentation parity'
    'CL-013' = 'Reconnect after combat state change'
}

foreach ($scenarioId in $ScenarioIds) {
    $matrix.Add([ordered]@{
        Scenario = $scenarioId
        Name = if ($scenarioCatalog.ContainsKey($scenarioId)) { $scenarioCatalog[$scenarioId] } else { 'Unknown scenario' }
        Mode = 'Pending'
        Expected = 'Definir conforme COMBAT_LAB_TEST_MATRIX.md'
        Actual = ''
        Evidence = ''
        Status = 'PENDING'
    }) | Out-Null
}

$smokeSummary = $null
if ($RunSmokeGate) {
    $smokeScript = Join-Path $ProjectRoot 'Scripts\Run-Smoke-Gate.ps1'
    if (Test-Path $smokeScript) {
        $smokeLog = Join-Path $runDir 'combat-lab-smoke.log'
        $smokeArgs = @('-NoBuild', '-TestDurationSeconds', "$TestDurationSeconds")
        if ($NoSteam) { $smokeArgs += '-NoSteam' }
        if ($SkipLagLoss) { $smokeArgs += '-SkipLagLoss' }

        $smokeResult = Invoke-SpellRiseProcess -FilePath 'powershell.exe' -Arguments @(
            '-ExecutionPolicy', 'Bypass', '-File', "`\"$smokeScript`\""
        ) + $smokeArgs -LogPath $smokeLog -WorkingDirectory $ProjectRoot

        $smokeSummary = @{ Status = if ($smokeResult.ExitCode -eq 0) { 'PASS' } else { 'FAIL' }; ExitCode = $smokeResult.ExitCode; LogPath = $smokeLog }
    }
    else {
        $smokeSummary = @{ Status = 'MANUAL_REQUIRED'; ExitCode = 2; LogPath = '' }
    }
}

Write-SpellRiseJsonFile -Path (Join-Path $runDir 'combat-lab-matrix.json') -Object @{ TimestampUtc = (Get-Date).ToUniversalTime().ToString('o'); Scenarios = @($matrix); Smoke = $smokeSummary }

$csvLines = @('Scenario,Name,Mode,Expected,Actual,Evidence,Status')
foreach ($row in $matrix) { $csvLines += ('{0},{1},{2},{3},{4},{5},{6}' -f $row.Scenario, $row.Name, $row.Mode, $row.Expected, $row.Actual, $row.Evidence, $row.Status) }
Write-SpellRiseTextFile -Path (Join-Path $runDir 'combat-lab-matrix.csv') -Content ($csvLines -join [Environment]::NewLine)

$md = @()
$md += '# Combat Lab Run'
$md += ''
$md += "RunDir: `$runDir`"
$md += ''
$md += '| Scenario | Nome | Status | Evidence |'
$md += '|---|---|---|---|'
foreach ($row in $matrix) { $md += "| $($row.Scenario) | $($row.Name) | $($row.Status) | $($row.Evidence) |" }
if ($null -ne $smokeSummary) {
    $md += ''
    $md += "SmokeStatus: $($smokeSummary.Status)"
    if ($smokeSummary.LogPath) { $md += "SmokeLog: `$($smokeSummary.LogPath)`" }
}
Write-SpellRiseTextFile -Path (Join-Path $runDir 'combat-lab-run.md') -Content ($md -join [Environment]::NewLine)

if ($null -ne $smokeSummary -and $smokeSummary.Status -eq 'FAIL') { exit 1 }
if ($null -ne $smokeSummary -and $smokeSummary.Status -eq 'MANUAL_REQUIRED') { exit 2 }
exit 0
