param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$EditorCmdPath = "",
    [string]$Map = "/Game/Maps/Stylized/Stylized",
    [int]$Port = 17777,
    [int]$ServerBootSeconds = 12,
    [int]$ClientJoinGapMs = 800,
    [int]$RoundDurationSeconds = 60,
    [string]$ClientCounts = "2,4,6,8",
    [switch]$WithLagLoss,
    [int]$PktLag = 120,
    [int]$PktLoss = 5,
    [string]$ServerExtraArgs = "",
    [string]$ClientExtraArgs = "",
    [string]$ClientExecCmds = "",
    [switch]$StopOnFirstUnstable
)

$ErrorActionPreference = "Stop"

function Get-MatchCount {
    param(
        [string]$Path,
        [string]$Pattern
    )

    if (!(Test-Path $Path)) {
        return 0
    }

    $matches = Select-String -Path $Path -Pattern $Pattern -ErrorAction SilentlyContinue
    if ($null -eq $matches) {
        return 0
    }

    return @($matches).Count
}

function Stop-IfRunning {
    param([System.Diagnostics.Process]$Process)

    if ($null -eq $Process) {
        return
    }

    try {
        if (!$Process.HasExited) {
            Stop-Process -Id $Process.Id -Force -ErrorAction Stop
        }
    }
    catch {
    }
}

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

if (!(Test-Path $ProjectPath)) {
    Write-Error "SpellRise.uproject nao encontrado em: $ProjectPath"
    exit 1
}

# Garante AppID Steam ao lado dos executaveis usados nos testes (inclusive -nosteam).
$steamAppIdValue = "480"
$steamAppIdPaths = @(
    (Join-Path (Split-Path $EditorPath -Parent) "steam_appid.txt"),
    (Join-Path (Split-Path $EditorCmdPath -Parent) "steam_appid.txt")
) | Select-Object -Unique

foreach ($steamAppIdPath in $steamAppIdPaths) {
    Set-Content -Path $steamAppIdPath -Value $steamAppIdValue -Encoding ascii
}

$parsedCounts = @()
foreach ($token in $ClientCounts.Split(',')) {
    $trimmed = $token.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
        continue
    }

    $value = 0
    if (![int]::TryParse($trimmed, [ref]$value)) {
        Write-Error "ClientCounts invalido: '$trimmed'"
        exit 1
    }

    if ($value -le 0) {
        Write-Error "ClientCounts deve conter apenas inteiros positivos. Recebido: '$value'"
        exit 1
    }

    $parsedCounts += $value
}

if ($parsedCounts.Count -eq 0) {
    Write-Error "ClientCounts vazio. Exemplo valido: 2,4,6,8"
    exit 1
}

$stamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$runDir = Join-Path $LogsRoot ("LoadNoSteam\{0}" -f $stamp)
New-Item -ItemType Directory -Path $runDir -Force | Out-Null
$runDir = (Resolve-Path $runDir).Path

$overallResults = @()
$overallUnstable = $false

foreach ($count in $parsedCounts) {
    $roundName = "Round_{0}Clients" -f $count
    $roundDir = Join-Path $runDir $roundName
    New-Item -ItemType Directory -Path $roundDir -Force | Out-Null

    $serverLog = Join-Path $roundDir "Server.stdout.log"
    $serverErr = Join-Path $roundDir "Server.stderr.log"

    $clientExecCommandList = @()
    if ($WithLagLoss.IsPresent) {
        $clientExecCommandList += ("Net PktLag={0}" -f $PktLag)
        $clientExecCommandList += ("Net PktLoss={0}" -f $PktLoss)
    }

    if (![string]::IsNullOrWhiteSpace($ClientExecCmds)) {
        $extraClientCommands = $ClientExecCmds.Split(';', [System.StringSplitOptions]::RemoveEmptyEntries)
        foreach ($extraCommand in $extraClientCommands) {
            $trimmedCommand = $extraCommand.Trim()
            if (![string]::IsNullOrWhiteSpace($trimmedCommand)) {
                $clientExecCommandList += $trimmedCommand
            }
        }
    }

    $clientExecArg = ""
    if ($clientExecCommandList.Count -gt 0) {
        $clientExecArg = (' -ExecCmds="{0}"' -f ($clientExecCommandList -join ";"))
    }

    $serverArgs = ('"{0}" {1} -server -unattended -nullrhi -nosplash -stdout -FullStdOutLogOutput -log -nosteam -Port={2} -NetDriverListenPort={2}' -f $ProjectPath, $Map, $Port)
    if (![string]::IsNullOrWhiteSpace($ServerExtraArgs)) {
        $serverArgs += (" " + $ServerExtraArgs.Trim())
    }

    $serverProc = $null
    $clientProcs = @()

    try {
        $serverProc = Start-Process -FilePath $EditorCmdPath -ArgumentList $serverArgs -PassThru -RedirectStandardOutput $serverLog -RedirectStandardError $serverErr
        Start-Sleep -Seconds $ServerBootSeconds

        for ($i = 1; $i -le $count; $i++) {
            $clientLog = Join-Path $roundDir ("Client_{0}.stdout.log" -f $i)
            $clientErr = Join-Path $roundDir ("Client_{0}.stderr.log" -f $i)

            $winX = (($i - 1) % 4) * 960
            $winY = [math]::Floor(($i - 1) / 4) * 540

            $clientArgs = ('"{0}" 127.0.0.1:{1} -game -unattended -nullrhi -nosplash -stdout -FullStdOutLogOutput -log -nosteam -windowed -ResX=960 -ResY=540 -WinX={2} -WinY={3}{4}' -f $ProjectPath, $Port, $winX, $winY, $clientExecArg)
            if (![string]::IsNullOrWhiteSpace($ClientExtraArgs)) {
                $clientArgs += (" " + $ClientExtraArgs.Trim())
            }

            $clientProc = Start-Process -FilePath $EditorCmdPath -ArgumentList $clientArgs -PassThru -RedirectStandardOutput $clientLog -RedirectStandardError $clientErr
            $clientProcs += [PSCustomObject]@{
                Index = $i
                Process = $clientProc
                Log = $clientLog
                Err = $clientErr
            }

            Start-Sleep -Milliseconds $ClientJoinGapMs
        }

        Start-Sleep -Seconds $RoundDurationSeconds
    }
    finally {
        foreach ($entry in $clientProcs) {
            Stop-IfRunning -Process $entry.Process
        }

        Stop-IfRunning -Process $serverProc
    }

    $serverJoinCount = Get-MatchCount -Path $serverLog -Pattern "Join succeeded"
    $serverAcceptCount = Get-MatchCount -Path $serverLog -Pattern "NotifyAcceptedConnection"

    $clientResults = @()
    $joinedClients = 0
    $timeoutSignals = 0
    $preLoginFailures = 0
    $networkFailures = 0
    $ensureSignals = 0
    $bitReaderOverflows = 0

    foreach ($entry in $clientProcs) {
        $joinCount = (Get-MatchCount -Path $entry.Log -Pattern "Welcomed by server") + (Get-MatchCount -Path $entry.Log -Pattern "Join succeeded")
        $timeoutCount = Get-MatchCount -Path $entry.Log -Pattern "ConnectionTimeout|Connection TIMED OUT"
        $preLoginCount = Get-MatchCount -Path $entry.Log -Pattern "PreLogin failure|incompatible_unique_net_id"
        $networkFailureCount = Get-MatchCount -Path $entry.Log -Pattern "NetworkFailure|ConnectionLost|PendingConnectionFailure"
        $ensureCount = Get-MatchCount -Path $entry.Log -Pattern "Handled ensure|Ensure condition failed|Critical error|Fatal error"
        $bitOverflowCount = Get-MatchCount -Path $entry.Log -Pattern "FBitReader::SetOverflowed|ReadFieldHeaderAndPayload: Error reading payload"

        if ($joinCount -gt 0) {
            $joinedClients++
        }

        $timeoutSignals += $timeoutCount
        $preLoginFailures += $preLoginCount
        $networkFailures += $networkFailureCount
        $ensureSignals += $ensureCount
        $bitReaderOverflows += $bitOverflowCount

        $clientResults += [PSCustomObject]@{
            ClientIndex = $entry.Index
            Log = $entry.Log
            Err = $entry.Err
            JoinSignals = $joinCount
            TimeoutSignals = $timeoutCount
            PreLoginFailures = $preLoginCount
            NetworkFailures = $networkFailureCount
            EnsureSignals = $ensureCount
            BitReaderOverflowSignals = $bitOverflowCount
        }
    }

    $lagLossApplied = $true
    if ($WithLagLoss.IsPresent) {
        $lagLossApplied = $true
        foreach ($entry in $clientProcs) {
            $lagApplied = (Get-MatchCount -Path $entry.Log -Pattern ("PktLag set to {0}" -f $PktLag)) -gt 0
            $lossApplied = (Get-MatchCount -Path $entry.Log -Pattern ("PktLoss set to {0}" -f $PktLoss)) -gt 0
            if (!($lagApplied -and $lossApplied)) {
                $lagLossApplied = $false
                break
            }
        }
    }

    $roundUnstable = $false
    if ($joinedClients -lt $count) { $roundUnstable = $true }
    if ($preLoginFailures -gt 0) { $roundUnstable = $true }
    if ($networkFailures -gt 0) { $roundUnstable = $true }
    if ($timeoutSignals -gt 0) { $roundUnstable = $true }
    if ($ensureSignals -gt 0) { $roundUnstable = $true }
    if ($bitReaderOverflows -gt 0) { $roundUnstable = $true }
    if ($WithLagLoss.IsPresent -and -not $lagLossApplied) { $roundUnstable = $true }

    $roundResult = [PSCustomObject]@{
        RoundName = $roundName
        RoundDir = $roundDir
        ClientsRequested = $count
        DurationSeconds = $RoundDurationSeconds
        WithLagLoss = $WithLagLoss.IsPresent
        PktLag = $PktLag
        PktLoss = $PktLoss
        LagLossApplied = $lagLossApplied
        ServerJoinSucceeded = $serverJoinCount
        ServerAcceptedConnections = $serverAcceptCount
        JoinedClients = $joinedClients
        TimeoutSignals = $timeoutSignals
        PreLoginFailures = $preLoginFailures
        NetworkFailures = $networkFailures
        EnsureSignals = $ensureSignals
        BitReaderOverflowSignals = $bitReaderOverflows
        Unstable = $roundUnstable
        ServerLog = $serverLog
        ServerErr = $serverErr
        ClientExecCmds = ($clientExecCommandList -join ";")
        Clients = $clientResults
    }

    $overallResults += $roundResult
    if ($roundUnstable) {
        $overallUnstable = $true
    }

    if ($roundUnstable -and $StopOnFirstUnstable.IsPresent) {
        break
    }
}

$summaryJsonPath = Join-Path $runDir "LoadNoSteam_Summary.json"
$summaryTxtPath = Join-Path $runDir "LoadNoSteam_Summary.txt"

$summaryObject = [PSCustomObject]@{
    RunDir = $runDir
    Timestamp = (Get-Date).ToString("o")
    ProjectPath = $ProjectPath
    EditorPath = $EditorPath
    EditorCmdPath = $EditorCmdPath
    Map = $Map
    Port = $Port
    ServerBootSeconds = $ServerBootSeconds
    ClientJoinGapMs = $ClientJoinGapMs
    RoundDurationSeconds = $RoundDurationSeconds
    ClientCounts = $parsedCounts
    WithLagLoss = $WithLagLoss.IsPresent
    PktLag = $PktLag
    PktLoss = $PktLoss
    SteamAppIdPaths = $steamAppIdPaths
    OverallUnstable = $overallUnstable
    Rounds = $overallResults
}

$summaryObject | ConvertTo-Json -Depth 8 | Set-Content -Path $summaryJsonPath -Encoding UTF8

$txt = @()
$txt += ("RunDir={0}" -f $runDir)
$txt += ("ProjectPath={0}" -f $ProjectPath)
$txt += ("EditorCmdPath={0}" -f $EditorCmdPath)
$txt += ("Map={0} Port={1}" -f $Map, $Port)
$txt += ("ClientCounts={0}" -f ($parsedCounts -join ","))
$txt += ("WithLagLoss={0} PktLag={1} PktLoss={2}" -f $WithLagLoss.IsPresent, $PktLag, $PktLoss)
$txt += ("OverallUnstable={0}" -f $overallUnstable)
$txt += ""

foreach ($round in $overallResults) {
    $txt += ("[{0}]" -f $round.RoundName)
    $txt += ("RoundDir={0}" -f $round.RoundDir)
    $txt += ("ClientsRequested={0} JoinedClients={1}" -f $round.ClientsRequested, $round.JoinedClients)
    $txt += ("ServerJoinSucceeded={0} ServerAcceptedConnections={1}" -f $round.ServerJoinSucceeded, $round.ServerAcceptedConnections)
    $txt += ("TimeoutSignals={0} PreLoginFailures={1} NetworkFailures={2}" -f $round.TimeoutSignals, $round.PreLoginFailures, $round.NetworkFailures)
    $txt += ("EnsureSignals={0} BitReaderOverflowSignals={1}" -f $round.EnsureSignals, $round.BitReaderOverflowSignals)
    $txt += ("LagLossApplied={0} ClientExecCmds={1}" -f $round.LagLossApplied, $round.ClientExecCmds)
    $txt += ("Unstable={0}" -f $round.Unstable)
    $txt += ""
}

$txt | Set-Content -Path $summaryTxtPath -Encoding UTF8

Write-Host ("[LOAD-NOSTEAM] RunDir: {0}" -f $runDir)
Write-Host ("[LOAD-NOSTEAM] SummaryJson: {0}" -f $summaryJsonPath)
Write-Host ("[LOAD-NOSTEAM] SummaryTxt: {0}" -f $summaryTxtPath)
Write-Host ("[LOAD-NOSTEAM] OverallUnstable={0}" -f $overallUnstable)

if ($overallUnstable) {
    exit 2
}
