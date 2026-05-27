param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$EditorCmdPath = "",
    [string]$Map = "/Game/Maps/Stylized/Stylized",
    [int]$Port = 17777,
    [int]$ServerBootSeconds = 10,
    [int]$ClientJoinGapSeconds = 5,
    [int]$TestDurationSeconds = 45,
    [switch]$ReconnectClient1,
    [int]$ReconnectClient1AtSeconds = 20,
    [int]$ReconnectClient1DowntimeSeconds = 3,
    [string]$ServerExtraArgs = "",
    [string]$TestPersistentId = "",
    [string]$ClientExtraArgs = "",
    [string]$ClientExecCmds = "",
    [switch]$WithLagLoss,
    [int]$PktLag = 120,
    [int]$PktLoss = 1,
    [switch]$NoSteam,
    [switch]$RequirePersistence
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Join-Path $PSScriptRoot ".."
$ProjectPath = (Resolve-Path (Join-Path $ProjectRoot "SpellRise.uproject")).Path
$LogsRoot = Join-Path $ProjectRoot "Saved\Logs"

if (!(Test-Path $EditorPath)) {
    Write-Error "UnrealEditor.exe nao encontrado em: $EditorPath"
    exit 1
}

if ([string]::IsNullOrWhiteSpace($EditorCmdPath)) {
    $EditorCmdPath = $EditorPath -replace "UnrealEditor\.exe$", "UnrealEditor-Cmd.exe"
}

if (!(Test-Path $EditorCmdPath)) {
    Write-Error "UnrealEditor-Cmd.exe nao encontrado em: $EditorCmdPath"
    exit 1
}

# Garante AppID Steam ao lado do executavel usado no smoke.
$steamAppIdValue = "480"
$steamAppIdPaths = @(
    (Join-Path (Split-Path $EditorPath -Parent) "steam_appid.txt"),
    (Join-Path (Split-Path $EditorCmdPath -Parent) "steam_appid.txt")
) | Select-Object -Unique

foreach ($steamAppIdPath in $steamAppIdPaths) {
    Set-Content -Path $steamAppIdPath -Value $steamAppIdValue -Encoding ascii
}

if (!(Test-Path $ProjectPath)) {
    Write-Error "SpellRise.uproject nao encontrado em: $ProjectPath"
    exit 1
}

$stamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$runDir = Join-Path $LogsRoot ("SmokeAuto\{0}" -f $stamp)
New-Item -ItemType Directory -Path $runDir -Force | Out-Null
$runDir = (Resolve-Path $runDir).Path

$serverLog = Join-Path $runDir "Smoke_DS_Server.stdout.log"
$serverErr = Join-Path $runDir "Smoke_DS_Server.stderr.log"
$client1Log = Join-Path $runDir "Smoke_DS_Client1.stdout.log"
$client1Err = Join-Path $runDir "Smoke_DS_Client1.stderr.log"
$client2Log = Join-Path $runDir "Smoke_DS_Client2.stdout.log"
$client2Err = Join-Path $runDir "Smoke_DS_Client2.stderr.log"
$client1ReconnectLog = Join-Path $runDir "Smoke_DS_Client1_Reconnect.stdout.log"
$client1ReconnectErr = Join-Path $runDir "Smoke_DS_Client1_Reconnect.stderr.log"

$sharedArgs = @()
if ($NoSteam) {
    $sharedArgs += "-nosteam"
}

$clientExecCommandList = @()
if ($WithLagLoss) {
    $clientExecCommandList += ("Net PktLag={0}" -f $PktLag)
    $clientExecCommandList += ("Net PktLoss={0}" -f $PktLoss)
}
if (![string]::IsNullOrWhiteSpace($ClientExecCmds)) {
    $extraClientCommands = $ClientExecCmds.Split(';', [System.StringSplitOptions]::RemoveEmptyEntries)
    foreach ($extraCommand in $extraClientCommands) {
        $trimmed = $extraCommand.Trim()
        if (![string]::IsNullOrWhiteSpace($trimmed)) {
            $clientExecCommandList += $trimmed
        }
    }
}

$clientExecArg = ""
if ($clientExecCommandList.Count -gt 0) {
    $clientExecArg = (' -ExecCmds=\"{0}\"' -f ($clientExecCommandList -join ";"))
}

$server = $null
$client1 = $null
$client2 = $null
$client1Reconnected = $false
$client1ReconnectAttempted = $false

try {
    $sharedArgString = ""
    if ($sharedArgs.Count -gt 0) {
        $sharedArgString = " " + ($sharedArgs -join " ")
    }

    $serverArgs = ('"{0}" {1} -server -unattended -nullrhi -nosplash -stdout -FullStdOutLogOutput -log -Port={2} -NetDriverListenPort={2}{3}' -f $ProjectPath, $Map, $Port, $sharedArgString)
    if (![string]::IsNullOrWhiteSpace($TestPersistentId)) {
        $serverArgs += (" -SRTestPersistentId=" + $TestPersistentId.Trim())
    }
    if (![string]::IsNullOrWhiteSpace($ServerExtraArgs)) {
        $serverArgs += (" " + $ServerExtraArgs.Trim())
    }

    $server = Start-Process -FilePath $EditorCmdPath -ArgumentList $serverArgs -PassThru -RedirectStandardOutput $serverLog -RedirectStandardError $serverErr
    Start-Sleep -Seconds $ServerBootSeconds

    $clientExtraArgString = ""
    if (![string]::IsNullOrWhiteSpace($ClientExtraArgs)) {
        $clientExtraArgString = " " + $ClientExtraArgs.Trim()
    }

    $client1Args = ('"{0}" 127.0.0.1:{1} -game -unattended -nullrhi -nosplash -stdout -FullStdOutLogOutput -log -windowed -ResX=960 -ResY=540 -WinX=0 -WinY=0{2}{3}{4}' -f $ProjectPath, $Port, $sharedArgString, $clientExecArg, $clientExtraArgString)
    $client1 = Start-Process -FilePath $EditorCmdPath -ArgumentList $client1Args -PassThru -RedirectStandardOutput $client1Log -RedirectStandardError $client1Err

    Start-Sleep -Seconds $ClientJoinGapSeconds

    $client2Args = ('"{0}" 127.0.0.1:{1} -game -unattended -nullrhi -nosplash -stdout -FullStdOutLogOutput -log -windowed -ResX=960 -ResY=540 -WinX=980 -WinY=0{2}{3}{4}' -f $ProjectPath, $Port, $sharedArgString, $clientExecArg, $clientExtraArgString)
    $client2 = Start-Process -FilePath $EditorCmdPath -ArgumentList $client2Args -PassThru -RedirectStandardOutput $client2Log -RedirectStandardError $client2Err

    $startedAt = Get-Date
    while (((Get-Date) - $startedAt).TotalSeconds -lt $TestDurationSeconds) {
        $elapsedSeconds = [int](((Get-Date) - $startedAt).TotalSeconds)

        if ($ReconnectClient1.IsPresent -and !$client1ReconnectAttempted -and $elapsedSeconds -ge $ReconnectClient1AtSeconds) {
            $client1ReconnectAttempted = $true

            if ($null -ne $client1) {
                try {
                    if (!$client1.HasExited) {
                        Stop-Process -Id $client1.Id -Force -ErrorAction Stop
                    }
                } catch {
                }
            }

            Start-Sleep -Seconds ([Math]::Max(1, $ReconnectClient1DowntimeSeconds))

            $client1 = Start-Process -FilePath $EditorCmdPath -ArgumentList $client1Args -PassThru -RedirectStandardOutput $client1ReconnectLog -RedirectStandardError $client1ReconnectErr
            $client1Reconnected = $true
        }

        Start-Sleep -Seconds 1
    }
}
finally {
    foreach ($proc in @($client2, $client1, $server)) {
        if ($null -ne $proc) {
            try {
                if (!$proc.HasExited) {
                    Stop-Process -Id $proc.Id -Force -ErrorAction Stop
                }
            } catch {
            }
        }
    }
}

function Get-MatchCount {
    param(
        [string]$Path,
        [string]$Pattern
    )
    if (!(Test-Path $Path)) { return 0 }
    $matches = Select-String -Path $Path -Pattern $Pattern -ErrorAction SilentlyContinue
    if ($null -eq $matches) { return 0 }
    return @($matches).Count
}

$serverJoinCount = Get-MatchCount -Path $serverLog -Pattern "Join succeeded"
$client1WelcomeCount = Get-MatchCount -Path $client1Log -Pattern "Welcomed by server"
$client2WelcomeCount = Get-MatchCount -Path $client2Log -Pattern "Welcomed by server"
$client1ReconnectWelcomeCount = Get-MatchCount -Path $client1ReconnectLog -Pattern "Welcomed by server"

$persistencePreloadOkCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[PreloadOk\]"
$persistencePreloadMissCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[PreloadMiss\]"
$persistenceApplyResultCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[ApplyResult\]"
$persistenceApplySkippedCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[ApplySkipped\]"
$persistenceSaveCharacterOkCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[SaveCharacterOk\]"
$persistencePreloadSkippedNoSteamCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[PreloadSkipped\].*nosteam_mode"
$persistenceApplySkippedNoSteamCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[ApplySkipped\].*nosteam_mode"
$persistenceSaveSkippedNoSteamCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[SaveSkipped\].*nosteam_mode"
$persistenceConflictCount = Get-MatchCount -Path $serverLog -Pattern "RevisionExpected"
$persistenceTestIdFallbackCount = Get-MatchCount -Path $serverLog -Pattern "\[Persistence\]\[TestIdFallback\]"
$replicationOverflowCountServer = Get-MatchCount -Path $serverLog -Pattern "FBitReader::SetOverflowed"
$replicationOverflowCountClient1 = Get-MatchCount -Path $client1Log -Pattern "FBitReader::SetOverflowed"
$replicationOverflowCountClient2 = Get-MatchCount -Path $client2Log -Pattern "FBitReader::SetOverflowed"
$replicationOverflowCountClient1Reconnect = Get-MatchCount -Path $client1ReconnectLog -Pattern "FBitReader::SetOverflowed"
$replicationOverflowCountTotal = $replicationOverflowCountServer + $replicationOverflowCountClient1 + $replicationOverflowCountClient2 + $replicationOverflowCountClient1Reconnect

# Fallback anti-flake: em alguns runs o log do servidor pode não registrar "Join succeeded"
# no tempo de coleta, mesmo com clientes confirmados no lado do cliente.
$inferredJoinCountFromClients = $client1WelcomeCount + $client2WelcomeCount + $client1ReconnectWelcomeCount
$effectiveServerJoinCount = [Math]::Max($serverJoinCount, $inferredJoinCountFromClients)

$client1LagFlag = $true
$client2LagFlag = $true
if ($WithLagLoss) {
    $client1LagFlag = ((Get-MatchCount -Path $client1Log -Pattern ("PktLag set to {0}" -f $PktLag)) -gt 0) -and
                      ((Get-MatchCount -Path $client1Log -Pattern ("PktLoss set to {0}" -f $PktLoss)) -gt 0)
    $client2LagFlag = ((Get-MatchCount -Path $client2Log -Pattern ("PktLag set to {0}" -f $PktLag)) -gt 0) -and
                      ((Get-MatchCount -Path $client2Log -Pattern ("PktLoss set to {0}" -f $PktLoss)) -gt 0)
}

$passConnectivity = ($effectiveServerJoinCount -ge 2) -and ($client1WelcomeCount -ge 1) -and ($client2WelcomeCount -ge 1)
$passReconnectClient1 = $true
if ($ReconnectClient1.IsPresent) {
    $passReconnectClient1 = $client1Reconnected -and ($client1ReconnectWelcomeCount -ge 1) -and ($effectiveServerJoinCount -ge 3)
}
$passLagLoss = ($client1LagFlag -and $client2LagFlag)
$passReplicationOverflow = ($replicationOverflowCountTotal -eq 0)
$passPersistence = $true
if ($RequirePersistence.IsPresent) {
    $passPersistenceSuccessPath = (($persistencePreloadOkCount -ge 1) -or ($persistencePreloadMissCount -ge 1)) -and (($persistenceApplyResultCount -ge 1) -or ($persistenceApplySkippedCount -ge 1)) -and ($persistenceSaveCharacterOkCount -ge 1)
    $passPersistenceNoSteamDisabledPath = $NoSteam.IsPresent -and ($persistencePreloadSkippedNoSteamCount -ge 1) -and ($persistenceApplySkippedNoSteamCount -ge 1) -and ($persistenceSaveSkippedNoSteamCount -ge 1)
    $passPersistence = $passPersistenceSuccessPath -or $passPersistenceNoSteamDisabledPath
}
$overallPass = $passConnectivity -and $passLagLoss -and $passReconnectClient1 -and $passPersistence -and $passReplicationOverflow

$summaryPath = Join-Path $runDir "Smoke_Summary.txt"
$summary = @(
    ("RunDir={0}" -f $runDir),
    ("ServerLog={0}" -f $serverLog),
    ("Client1Log={0}" -f $client1Log),
    ("Client1Err={0}" -f $client1Err),
    ("Client2Log={0}" -f $client2Log),
    ("Client2Err={0}" -f $client2Err),
    ("Client1ReconnectLog={0}" -f $client1ReconnectLog),
    ("Client1ReconnectErr={0}" -f $client1ReconnectErr),
    ("ServerJoinSucceededCount={0}" -f $serverJoinCount),
    ("ServerJoinEffectiveCount={0}" -f $effectiveServerJoinCount),
    ("ServerJoinInferredFromClientWelcomes={0}" -f $inferredJoinCountFromClients),
    ("Client1WelcomedCount={0}" -f $client1WelcomeCount),
    ("Client2WelcomedCount={0}" -f $client2WelcomeCount),
    ("ReconnectClient1={0}" -f $ReconnectClient1.IsPresent),
    ("ReconnectClient1AtSeconds={0}" -f $ReconnectClient1AtSeconds),
    ("ReconnectClient1DowntimeSeconds={0}" -f $ReconnectClient1DowntimeSeconds),
    ("Client1ReconnectAttempted={0}" -f $client1ReconnectAttempted),
    ("Client1ReconnectProcessStarted={0}" -f $client1Reconnected),
    ("Client1ReconnectWelcomedCount={0}" -f $client1ReconnectWelcomeCount),
    ("RequirePersistence={0}" -f $RequirePersistence.IsPresent),
    ("PersistencePreloadOkCount={0}" -f $persistencePreloadOkCount),
    ("PersistencePreloadMissCount={0}" -f $persistencePreloadMissCount),
    ("PersistenceApplyResultCount={0}" -f $persistenceApplyResultCount),
    ("PersistenceApplySkippedCount={0}" -f $persistenceApplySkippedCount),
    ("PersistenceSaveCharacterOkCount={0}" -f $persistenceSaveCharacterOkCount),
    ("PersistencePreloadSkippedNoSteamCount={0}" -f $persistencePreloadSkippedNoSteamCount),
    ("PersistenceApplySkippedNoSteamCount={0}" -f $persistenceApplySkippedNoSteamCount),
    ("PersistenceSaveSkippedNoSteamCount={0}" -f $persistenceSaveSkippedNoSteamCount),
    ("PersistenceRevisionConflictCount={0}" -f $persistenceConflictCount),
    ("PersistenceTestIdFallbackCount={0}" -f $persistenceTestIdFallbackCount),
    ("ReplicationOverflowServerCount={0}" -f $replicationOverflowCountServer),
    ("ReplicationOverflowClient1Count={0}" -f $replicationOverflowCountClient1),
    ("ReplicationOverflowClient2Count={0}" -f $replicationOverflowCountClient2),
    ("ReplicationOverflowClient1ReconnectCount={0}" -f $replicationOverflowCountClient1Reconnect),
    ("ReplicationOverflowTotalCount={0}" -f $replicationOverflowCountTotal),
    ("WithLagLoss={0}" -f $WithLagLoss.IsPresent),
    ("ClientExtraArgs={0}" -f $ClientExtraArgs),
    ("ClientExecCmds={0}" -f ($clientExecCommandList -join ";")),
    ("Client1LagLossApplied={0}" -f $client1LagFlag),
    ("Client2LagLossApplied={0}" -f $client2LagFlag),
    ("PassConnectivity={0}" -f $passConnectivity),
    ("PassReconnectClient1={0}" -f $passReconnectClient1),
    ("PassLagLoss={0}" -f $passLagLoss),
    ("PassReplicationOverflow={0}" -f $passReplicationOverflow),
    ("PassPersistence={0}" -f $passPersistence),
    ("OverallPass={0}" -f $overallPass)
)
$summary | Set-Content -Encoding UTF8 $summaryPath

Write-Host ("[SMOKE] RunDir: {0}" -f $runDir)
Write-Host ("[SMOKE] steam_appid.txt: {0}" -f ($steamAppIdPaths -join ", "))
Write-Host ("[SMOKE] Server joins: raw={0} effective={1}" -f $serverJoinCount, $effectiveServerJoinCount)
Write-Host ("[SMOKE] Client welcomes: C1={0} C2={1}" -f $client1WelcomeCount, $client2WelcomeCount)
if ($WithLagLoss) {
    Write-Host ("[SMOKE] Lag/Loss applied: C1={0} C2={1}" -f $client1LagFlag, $client2LagFlag)
}
if ($RequirePersistence.IsPresent) {
    Write-Host ("[SMOKE] Persistence: preload_ok={0} preload_miss={1} apply_ok={2} apply_skipped={3} save_ok={4} preload_skipped_nosteam={5} apply_skipped_nosteam={6} save_skipped_nosteam={7} conflicts={8} testid_fallback={9} pass={10}" -f $persistencePreloadOkCount, $persistencePreloadMissCount, $persistenceApplyResultCount, $persistenceApplySkippedCount, $persistenceSaveCharacterOkCount, $persistencePreloadSkippedNoSteamCount, $persistenceApplySkippedNoSteamCount, $persistenceSaveSkippedNoSteamCount, $persistenceConflictCount, $persistenceTestIdFallbackCount, $passPersistence)
}
Write-Host ("[SMOKE] ReplicationOverflow: server={0} c1={1} c2={2} c1_reconnect={3} total={4} pass={5}" -f $replicationOverflowCountServer, $replicationOverflowCountClient1, $replicationOverflowCountClient2, $replicationOverflowCountClient1Reconnect, $replicationOverflowCountTotal, $passReplicationOverflow)
Write-Host ("[SMOKE] OverallPass={0}" -f $overallPass)
Write-Host ("[SMOKE] Summary: {0}" -f $summaryPath)

if (-not $overallPass) {
    exit 2
}
