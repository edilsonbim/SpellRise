[CmdletBinding()]
param(
    [string]$ProjectRoot = 'C:\Users\biM\Documents\Unreal Projects\SpellRise',
    [string]$UnrealRoot = 'C:\UnrealSource\UnrealEngine',
    [string]$Configuration = 'Development',
    [string]$Platform = 'Win64',
    [string]$OnlineProfile = 'test',
    [switch]$NoBuild,
    [switch]$NoPackage,
    [switch]$NoSmoke,
    [switch]$NoSteam,
    [switch]$SkipLagLoss,
    [int]$TestDurationSeconds = 60
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\SpellRise.Pipeline.Common.ps1"

$runDir = New-SpellRiseRunDirectory -ProjectRoot $ProjectRoot -PipelineName 'PersistenceGate'
$releaseGateScript = Join-Path $PSScriptRoot 'Run-SpellRise-ReleaseGate.ps1'
$gateLog = Join-Path $runDir 'release-gate.log'

$gateArgs = @(
    '-ProjectRoot', "`\"$ProjectRoot`\"",
    '-UnrealRoot', "`\"$UnrealRoot`\"",
    '-Configuration', $Configuration,
    '-Platform', $Platform,
    '-OnlineProfile', $OnlineProfile,
    '-TestDurationSeconds', "$TestDurationSeconds"
)
if ($NoBuild) { $gateArgs += '-NoBuild' }
if ($NoPackage) { $gateArgs += '-NoPackage' }
if ($NoSmoke) { $gateArgs += '-NoSmoke' }
if ($NoSteam) { $gateArgs += '-NoSteam' }
if ($SkipLagLoss) { $gateArgs += '-SkipLagLoss' }

$gateResult = Invoke-SpellRiseProcess -FilePath 'powershell.exe' -Arguments @(
    '-ExecutionPolicy', 'Bypass', '-File', "`\"$releaseGateScript`\""
) + $gateArgs -LogPath $gateLog -WorkingDirectory $ProjectRoot

$checklist = @()
$checklist += '# Persistence Gate Checklist'
$checklist += ''
$checklist += "RunDir: `$runDir`"
$checklist += ''
$checklist += '## Automated baseline'
$checklist += "- ReleaseGateExitCode: $($gateResult.ExitCode)"
$checklist += "- ReleaseGateLog: `$gateLog`"
$checklist += ''
$checklist += '## Manual persistence checks'
$checklist += '- [ ] login/sessão com identidade esperada do cenário'
$checklist += '- [ ] load inicial do personagem sem estado híbrido inválido'
$checklist += '- [ ] load inicial de inventário sem duplicação/perda'
$checklist += '- [ ] save após mudança de inventário'
$checklist += '- [ ] reconnect restaura estado determinístico'
$checklist += '- [ ] rollback seguro em falha parcial simulada'
$checklist += '- [ ] logs/telemetria de save/load presentes'
$checklist += '- [ ] caminho degradado explícito quando DB indisponível'
$checklist += ''
$checklist += '## Evidence'
$checklist += '- Build/package id:'
$checklist += '- Perfil online/auth:'
$checklist += '- Provider ativo (DB/file/degradado):'
$checklist += '- Logs:'
$checklist += '- Resultado final: PASS / PASS WITH RISK / FAIL'

Write-SpellRiseTextFile -Path (Join-Path $runDir 'persistence-gate-checklist.md') -Content ($checklist -join [Environment]::NewLine)

if ($gateResult.ExitCode -eq 1) { exit 1 }
if ($gateResult.ExitCode -eq 2) { exit 2 }
exit 0
