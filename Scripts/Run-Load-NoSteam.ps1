param(
    [string]$DesktopBuildRoot = "$env:USERPROFILE\Desktop\SpellRise_TestBuild",
    [string]$ServerExePath = "",
    [string]$ClientExePath = "",
    [string]$ServerAddress = "127.0.0.1",
    [int]$Port = 7777,
    [int[]]$PlayerSteps = @(2,4,6,8),
    [int]$StepDurationSeconds = 90,
    [int]$ClientLaunchGapMs = 900,
    [switch]$WithLagLoss,
    [int]$PktLag = 80,
    [int]$PktLoss = 2,
    [switch]$KeepTestProfile
)

$ErrorActionPreference = 'Stop'

$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$setProfileScript = Join-Path $projectRoot 'Scripts\Set-SteamAuthProfile.ps1'
$reportRoot = Join-Path $projectRoot 'Saved\Logs\Load_NoSteam'
$runStamp = Get-Date -Format 'yyyy-MM-dd_HH-mm-ss'
$runDir = Join-Path $reportRoot $runStamp
New-Item -ItemType Directory -Path $runDir -Force | Out-Null

$serverExe = if ([string]::IsNullOrWhiteSpace($ServerExePath)) {
    Join-Path $DesktopBuildRoot 'Server\WindowsServer\SpellRise\Binaries\Win64\SpellRiseServer.exe'
} else {
    $ServerExePath
}
$clientExe = if ([string]::IsNullOrWhiteSpace($ClientExePath)) {
    Join-Path $DesktopBuildRoot 'Client\WindowsClient\SpellRise\Binaries\Win64\SpellRise.exe'
} else {
    $ClientExePath
}

if (!(Test-Path $serverExe)) { throw "Server exe nao encontrado: $serverExe" }
if (!(Test-Path $clientExe)) { throw "Client exe nao encontrado: $clientExe" }
if (!(Test-Path $setProfileScript)) { throw "Script de perfil nao encontrado: $setProfileScript" }

$allResults = @()
$serverProc = $null
$serverSamples = @()

try {
    & $setProfileScript -Profile test

    $serverLog = Join-Path $runDir 'server.stdout.log'
    $serverErr = Join-Path $runDir 'server.stderr.log'
    $serverArgs = "-log -nosteam -Port=$Port -NetDriverListenPort=$Port"
    $serverProc = Start-Process -FilePath $serverExe -ArgumentList $serverArgs -PassThru -RedirectStandardOutput $serverLog -RedirectStandardError $serverErr

    Start-Sleep -Seconds 10

    foreach ($players in $PlayerSteps) {
        $stepDir = Join-Path $runDir ("step_{0}p" -f $players)
        New-Item -ItemType Directory -Path $stepDir -Force | Out-Null

        $clients = @()
        for ($i = 1; $i -le $players; $i++) {
            $out = Join-Path $stepDir ("client${i}.stdout.log")
            $err = Join-Path $stepDir ("client${i}.stderr.log")
            $x = ((($i - 1) % 4) * 480)
            $y = ([int](($i - 1) / 4) * 290)
            $args = "$ServerAddress`:$Port -log -nosteam -windowed -ResX=470 -ResY=260 -WinX=$x -WinY=$y"
            if ($WithLagLoss.IsPresent) {
                $args += (' -ExecCmds=\"Net PktLag={0};Net PktLoss={1}\"' -f $PktLag, $PktLoss)
            }
            $proc = Start-Process -FilePath $clientExe -ArgumentList $args -PassThru -RedirectStandardOutput $out -RedirectStandardError $err
            $clients += [PSCustomObject]@{ Index = $i; Id = $proc.Id; Out = $out; Err = $err }
            Start-Sleep -Milliseconds $ClientLaunchGapMs
        }

        $startedAt = Get-Date
        while (((Get-Date) - $startedAt).TotalSeconds -lt $StepDurationSeconds) {
            if ($serverProc -and -not $serverProc.HasExited) {
                try {
                    $sp = Get-Process -Id $serverProc.Id -ErrorAction Stop
                    $serverSamples += [PSCustomObject]@{
                        Step = $players
                        Time = (Get-Date).ToString('o')
                        CPU = $sp.CPU
                        WS_MB = [math]::Round($sp.WorkingSet64 / 1MB, 2)
                        PM_MB = [math]::Round($sp.PrivateMemorySize64 / 1MB, 2)
                    }
                } catch {}
            }
            Start-Sleep -Seconds 1
        }

        $stepClientRows = @()
        foreach ($client in $clients) {
            $alive = $false
            try { $alive = -not (Get-Process -Id $client.Id -ErrorAction Stop).HasExited } catch { $alive = $false }
            if ($alive) { Stop-Process -Id $client.Id -Force -ErrorAction SilentlyContinue }

            $join = (Select-String -Path $client.Out -Pattern 'Join succeeded|Welcomed by server|Login request' -ErrorAction SilentlyContinue | Measure-Object).Count
            $timeout = (Select-String -Path $client.Out -Pattern 'ConnectionTimeout|ConnectionLost|NetworkFailure' -ErrorAction SilentlyContinue | Measure-Object).Count
            $preLogin = (Select-String -Path $client.Out -Pattern 'PreLogin failure|incompatible_unique_net_id' -ErrorAction SilentlyContinue | Measure-Object).Count
            $bitOverflow = (Select-String -Path $client.Out -Pattern 'FBitReader::SetOverflowed|ReadFieldHeaderAndPayload: Error reading payload' -ErrorAction SilentlyContinue | Measure-Object).Count
            $ensure = (Select-String -Path $client.Out -Pattern 'Handled ensure|Fatal error|Critical error' -ErrorAction SilentlyContinue | Measure-Object).Count

            $stepClientRows += [PSCustomObject]@{
                Client = $client.Index
                JoinSignals = $join
                TimeoutSignals = $timeout
                PreLoginSignals = $preLogin
                BitReaderOverflowSignals = $bitOverflow
                EnsureOrFatalSignals = $ensure
                Log = $client.Out
            }
        }

        $connected = ($stepClientRows | Where-Object { $_.JoinSignals -gt 0 }).Count
        $bitTotal = ($stepClientRows | Measure-Object -Property BitReaderOverflowSignals -Sum).Sum
        $ensureTotal = ($stepClientRows | Measure-Object -Property EnsureOrFatalSignals -Sum).Sum
        $timeoutTotal = ($stepClientRows | Measure-Object -Property TimeoutSignals -Sum).Sum
        $unstable = ($connected -lt $players) -or ($bitTotal -gt 0) -or ($ensureTotal -gt 0) -or ($timeoutTotal -gt 0)

        $allResults += [PSCustomObject]@{
            Players = $players
            DurationSeconds = $StepDurationSeconds
            ConnectedClients = $connected
            BitReaderOverflowSignals = $bitTotal
            EnsureOrFatalSignals = $ensureTotal
            TimeoutSignals = $timeoutTotal
            Unstable = $unstable
            StepDir = $stepDir
            ClientRows = $stepClientRows
        }

        if ($unstable) {
            break
        }
    }
}
finally {
    if ($serverProc) {
        try {
            if (-not $serverProc.HasExited) {
                Stop-Process -Id $serverProc.Id -Force -ErrorAction Stop
            }
        } catch {}
    }

    if (-not $KeepTestProfile.IsPresent) {
        try { & $setProfileScript -Profile prod } catch {}
    }
}

$serverCpuDelta = 0.0
$serverWSAvg = 0.0
$serverPMAvg = 0.0
if ($serverSamples.Count -gt 1) {
    $firstCpu = [double]$serverSamples[0].CPU
    $lastCpu = [double]$serverSamples[$serverSamples.Count - 1].CPU
    $serverCpuDelta = [math]::Round($lastCpu - $firstCpu, 3)
    $serverWSAvg = [math]::Round((($serverSamples | Measure-Object -Property WS_MB -Average).Average), 2)
    $serverPMAvg = [math]::Round((($serverSamples | Measure-Object -Property PM_MB -Average).Average), 2)
}

$summary = [PSCustomObject]@{
    RunDir = $runDir
    ServerExe = $serverExe
    ClientExe = $clientExe
    ServerAddress = "$ServerAddress`:$Port"
    WithLagLoss = $WithLagLoss.IsPresent
    PktLag = $PktLag
    PktLoss = $PktLoss
    Steps = $allResults
    ServerPerf = [PSCustomObject]@{
        SampleCount = $serverSamples.Count
        CpuDeltaSeconds = $serverCpuDelta
        AvgWorkingSetMB = $serverWSAvg
        AvgPrivateMemoryMB = $serverPMAvg
    }
}

$summaryJson = Join-Path $runDir 'Load_NoSteam_Summary.json'
$summaryTxt = Join-Path $runDir 'Load_NoSteam_Summary.txt'
$perfCsv = Join-Path $runDir 'Load_NoSteam_ServerPerf.csv'

$summary | ConvertTo-Json -Depth 8 | Set-Content -Path $summaryJson -Encoding UTF8

$txt = @()
$txt += "RunDir=$runDir"
$txt += "Server=$ServerAddress`:$Port"
$txt += "WithLagLoss=$($WithLagLoss.IsPresent) Lag=$PktLag Loss=$PktLoss"
$txt += "ServerPerf.SampleCount=$($summary.ServerPerf.SampleCount)"
$txt += "ServerPerf.CpuDeltaSeconds=$($summary.ServerPerf.CpuDeltaSeconds)"
$txt += "ServerPerf.AvgWorkingSetMB=$($summary.ServerPerf.AvgWorkingSetMB)"
$txt += "ServerPerf.AvgPrivateMemoryMB=$($summary.ServerPerf.AvgPrivateMemoryMB)"
foreach ($step in $allResults) {
    $txt += "Step.Players=$($step.Players) Connected=$($step.ConnectedClients) BitOverflow=$($step.BitReaderOverflowSignals) EnsureOrFatal=$($step.EnsureOrFatalSignals) Timeout=$($step.TimeoutSignals) Unstable=$($step.Unstable)"
}
$txt | Set-Content -Path $summaryTxt -Encoding UTF8

if ($serverSamples.Count -gt 0) {
    $serverSamples | Export-Csv -Path $perfCsv -NoTypeInformation -Encoding UTF8
}

Write-Host "[LoadNoSteam] SummaryJson: $summaryJson"
Write-Host "[LoadNoSteam] SummaryTxt:  $summaryTxt"
Write-Host "[LoadNoSteam] PerfCsv:     $perfCsv"
