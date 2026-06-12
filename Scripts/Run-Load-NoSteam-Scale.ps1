param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$EditorCmdPath = "",
    [switch]$UseProjectBinaries,
    [string]$ServerBinaryPath = "",
    [string]$ClientBinaryPath = "",
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
    [int]$InitialConnectTimeoutSeconds = 60,
    [int]$ConnectionTimeoutSeconds = 60,
    [switch]$WithInsightsTrace,
    [string]$TraceChannels = "cpu,frame,bookmark,log,net",
    [switch]$WithPerfStats,
    [switch]$EnableGameplayCameras,
    [int]$LoadTestMaxPlayers = 100,
    [int]$MaxLocalClientsWithoutExplicitOverride = 12,
    [int]$ProcessExitGraceSeconds = 5,
    [switch]$AllowHighLocalClientCount,
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

    $processId = $Process.Id
    try {
        if (!$Process.HasExited) {
            Stop-Process -Id $processId -Force -ErrorAction SilentlyContinue
            if ($Process.WaitForExit($ProcessExitGraceSeconds * 1000)) {
                return
            }
        }
    }
    catch {
    }

    try {
        $taskKill = Join-Path $env:SystemRoot "System32\taskkill.exe"
        if (Test-Path $taskKill) {
            $taskKillProcess = Start-Process -FilePath $taskKill -ArgumentList @("/PID", $processId, "/T", "/F") -PassThru -WindowStyle Hidden
            [void]$taskKillProcess.WaitForExit(5000)
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

if ([string]::IsNullOrWhiteSpace($ServerBinaryPath)) {
    $ServerBinaryPath = Join-Path $ProjectRoot "Binaries\Win64\SpellRiseServer.exe"
}

if ([string]::IsNullOrWhiteSpace($ClientBinaryPath)) {
    $ClientBinaryPath = Join-Path $ProjectRoot "Binaries\Win64\SpellRiseClient.exe"
}

if (!$UseProjectBinaries.IsPresent -and !(Test-Path $EditorCmdPath)) {
    Write-Error "UnrealEditor-Cmd.exe nao encontrado em: $EditorCmdPath"
    exit 1
}

if ($UseProjectBinaries.IsPresent -and !(Test-Path $ServerBinaryPath)) {
    Write-Error "SpellRiseServer.exe nao encontrado em: $ServerBinaryPath"
    exit 1
}

if ($UseProjectBinaries.IsPresent -and !(Test-Path $ClientBinaryPath)) {
    Write-Error "SpellRiseClient.exe nao encontrado em: $ClientBinaryPath"
    exit 1
}

if ($UseProjectBinaries.IsPresent) {
    $serverCookedContent = Join-Path (Split-Path $ServerBinaryPath -Parent) "..\Content\Paks"
    $clientCookedContent = Join-Path (Split-Path $ClientBinaryPath -Parent) "..\Content\Paks"
    if (!(Test-Path $serverCookedContent) -or !(Test-Path $clientCookedContent)) {
        Write-Warning "UseProjectBinaries requer build staged/cooked. Binarios soltos em Binaries\\Win64 podem crashar carregando assets uncooked. Use Scripts\\Build-Cooked-Load-NoSteam.ps1 e passe os caminhos staged."
    }
}

if (!(Test-Path $ProjectPath)) {
    Write-Error "SpellRise.uproject nao encontrado em: $ProjectPath"
    exit 1
}

# Garante AppID Steam ao lado dos executaveis usados nos testes (inclusive -nosteam).
$steamAppIdValue = "480"
$steamAppIdPaths = @(
    (Join-Path (Split-Path $EditorPath -Parent) "steam_appid.txt"),
    (Join-Path (Split-Path $EditorCmdPath -Parent) "steam_appid.txt"),
    (Join-Path (Split-Path $ServerBinaryPath -Parent) "steam_appid.txt"),
    (Join-Path (Split-Path $ClientBinaryPath -Parent) "steam_appid.txt")
) | Select-Object -Unique

foreach ($steamAppIdPath in $steamAppIdPaths) {
    Set-Content -Path $steamAppIdPath -Value $steamAppIdValue -Encoding ascii
}

$parsedCounts = @()
foreach ($token in ([regex]::Split($ClientCounts, '[,\s]+'))) {
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

$maxRequestedClients = ($parsedCounts | Measure-Object -Maximum).Maximum
if ($maxRequestedClients -gt $MaxLocalClientsWithoutExplicitOverride -and !$AllowHighLocalClientCount.IsPresent) {
    Write-Error ("ClientCounts solicita {0} clientes locais. Limite seguro padrao={1}. Use -AllowHighLocalClientCount apenas em maquina dedicada/CI ou apos fechar clientes pesados." -f $maxRequestedClients, $MaxLocalClientsWithoutExplicitOverride)
    exit 1
}

$stamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$runDir = Join-Path $LogsRoot ("LoadNoSteam\{0}" -f $stamp)
New-Item -ItemType Directory -Path $runDir -Force | Out-Null
$runDir = (Resolve-Path $runDir).Path

$overallResults = @()
$overallUnstable = $false
$netTimeoutArgs = (' -ini:Engine:[/Script/OnlineSubsystemUtils.IpNetDriver]:InitialConnectTimeout={0}.0 -ini:Engine:[/Script/OnlineSubsystemUtils.IpNetDriver]:ConnectionTimeout={1}.0' -f $InitialConnectTimeoutSeconds, $ConnectionTimeoutSeconds)

foreach ($count in $parsedCounts) {
    $roundName = "Round_{0}Clients" -f $count
    $roundDir = Join-Path $runDir $roundName
    New-Item -ItemType Directory -Path $roundDir -Force | Out-Null

    $serverLog = Join-Path $roundDir "Server.stdout.log"
    $serverErr = Join-Path $roundDir "Server.stderr.log"
    $serverTrace = Join-Path $roundDir "Server.utrace"
    $serverMaxPlayers = [math]::Max($LoadTestMaxPlayers, $count)
    $serverMapUrl = $Map
    if ($serverMapUrl.Contains("?")) {
        $serverMapUrl += ("?MaxPlayers={0}" -f $serverMaxPlayers)
    }
    else {
        $serverMapUrl += ("?MaxPlayers={0}" -f $serverMaxPlayers)
    }

    $clientExecCommandList = @()
    if (!$EnableGameplayCameras.IsPresent) {
        $clientExecCommandList += "DDCVar.NewGameplayCameraSystem.Enable 0"
    }

    if ($WithPerfStats.IsPresent) {
        $clientExecCommandList += "stat net"
        $clientExecCommandList += "stat game"
        $clientExecCommandList += "stat abilitysystem"
    }

    if ($WithLagLoss.IsPresent) {
        $clientExecCommandList += ("NetEmulation.PktLag {0}" -f $PktLag)
        $clientExecCommandList += ("NetEmulation.PktLoss {0}" -f $PktLoss)
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

    $clientLoadCameraArgs = ""
    if (!$EnableGameplayCameras.IsPresent) {
        # Clientes sinteticos usam -unattended/-nullrhi; Gameplay Cameras e apresentacao local
        # e pode gerar ensure/hitch sem validar autoridade, rede ou desempenho do Dedicated Server.
        $clientLoadCameraArgs = " -DDCVar.NewGameplayCameraSystem.Enable=0 -SpellRiseSkipLocalHUDFlow"
    }

    $clientNetworkEmulationArgs = ""
    if ($WithLagLoss.IsPresent) {
        $clientNetworkEmulationArgs = (" PktLag={0} PktLoss={1}" -f $PktLag, $PktLoss)
    }

    $serverExe = $EditorCmdPath
    $serverArgs = ('"{0}" {1} -server -unattended -nullrhi -nosound -nosplash -stdout -FullStdOutLogOutput -log -nosteam -Port={2} -NetDriverListenPort={2}{3}' -f $ProjectPath, $serverMapUrl, $Port, $netTimeoutArgs)
    if ($UseProjectBinaries.IsPresent) {
        $serverExe = $ServerBinaryPath
        $serverArgs = ('{0} -server -unattended -nullrhi -nosound -nosplash -stdout -FullStdOutLogOutput -log -nosteam -Port={1} -NetDriverListenPort={1}{2}' -f $serverMapUrl, $Port, $netTimeoutArgs)
    }
    if ($WithInsightsTrace.IsPresent) {
        $serverArgs += (' -trace="{0}" -tracefile="{1}"' -f $TraceChannels, $serverTrace)
    }
    if ($WithPerfStats.IsPresent) {
        $serverArgs += ' -ExecCmds="stat net;stat game;stat abilitysystem"'
    }
    if (![string]::IsNullOrWhiteSpace($ServerExtraArgs)) {
        $serverArgs += (" " + $ServerExtraArgs.Trim())
    }

    $serverProc = $null
    $clientProcs = @()

    try {
        $serverProc = Start-Process -FilePath $serverExe -ArgumentList $serverArgs -PassThru -RedirectStandardOutput $serverLog -RedirectStandardError $serverErr
        Start-Sleep -Seconds $ServerBootSeconds

        for ($i = 1; $i -le $count; $i++) {
            $clientLog = Join-Path $roundDir ("Client_{0}.stdout.log" -f $i)
            $clientErr = Join-Path $roundDir ("Client_{0}.stderr.log" -f $i)

            $winX = (($i - 1) % 4) * 960
            $winY = [math]::Floor(($i - 1) / 4) * 540

            $clientExe = $EditorCmdPath
            $clientConnectUrl = ('127.0.0.1:{0}?InitialConnectTimeout={1}.0?ConnectionTimeout={2}.0' -f $Port, $InitialConnectTimeoutSeconds, $ConnectionTimeoutSeconds)
            $clientArgs = ('"{0}" {1} -game -unattended -nullrhi -nosound -nosplash -stdout -FullStdOutLogOutput -log -nosteam -windowed -ResX=960 -ResY=540 -WinX={2} -WinY={3}{4}{5}{6}{7}' -f $ProjectPath, $clientConnectUrl, $winX, $winY, $clientExecArg, $clientLoadCameraArgs, $clientNetworkEmulationArgs, $netTimeoutArgs)
            if ($UseProjectBinaries.IsPresent) {
                $clientExe = $ClientBinaryPath
                $clientArgs = ('{0} -game -unattended -nullrhi -nosound -nosplash -stdout -FullStdOutLogOutput -log -nosteam -windowed -ResX=960 -ResY=540 -WinX={1} -WinY={2}{3}{4}{5}{6}' -f $clientConnectUrl, $winX, $winY, $clientExecArg, $clientLoadCameraArgs, $clientNetworkEmulationArgs, $netTimeoutArgs)
            }
            if (![string]::IsNullOrWhiteSpace($ClientExtraArgs)) {
                $clientArgs += (" " + $ClientExtraArgs.Trim())
            }

            $clientProc = Start-Process -FilePath $clientExe -ArgumentList $clientArgs -PassThru -RedirectStandardOutput $clientLog -RedirectStandardError $clientErr
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
        $timeoutCount = Get-MatchCount -Path $entry.Log -Pattern "UNetConnection::Tick: Connection TIMED OUT|FailureType = ConnectionTimeout|Network Failure: .*ConnectionTimeout"
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

    $lagLossApplied = $false
    if ($WithLagLoss.IsPresent) {
        $lagLossApplied = $true
        foreach ($entry in $clientProcs) {
            $lagApplied =
                (Get-MatchCount -Path $entry.Log -Pattern ("PktLag={0}" -f $PktLag)) -gt 0 -or
                (Get-MatchCount -Path $entry.Log -Pattern ("NetEmulation\.PktLag {0}" -f $PktLag)) -gt 0
            $lossApplied =
                (Get-MatchCount -Path $entry.Log -Pattern ("PktLoss={0}" -f $PktLoss)) -gt 0 -or
                (Get-MatchCount -Path $entry.Log -Pattern ("NetEmulation\.PktLoss {0}" -f $PktLoss)) -gt 0
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
        ServerTrace = $(if ($WithInsightsTrace.IsPresent) { $serverTrace } else { "" })
        ServerMaxPlayers = $serverMaxPlayers
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
    UseProjectBinaries = $UseProjectBinaries.IsPresent
    ServerBinaryPath = $ServerBinaryPath
    ClientBinaryPath = $ClientBinaryPath
    Map = $Map
    Port = $Port
    ServerBootSeconds = $ServerBootSeconds
    ClientJoinGapMs = $ClientJoinGapMs
    RoundDurationSeconds = $RoundDurationSeconds
    InitialConnectTimeoutSeconds = $InitialConnectTimeoutSeconds
    ConnectionTimeoutSeconds = $ConnectionTimeoutSeconds
    ClientCounts = $parsedCounts
    WithLagLoss = $WithLagLoss.IsPresent
    PktLag = $PktLag
    PktLoss = $PktLoss
    WithInsightsTrace = $WithInsightsTrace.IsPresent
    TraceChannels = $TraceChannels
    WithPerfStats = $WithPerfStats.IsPresent
    EnableGameplayCameras = $EnableGameplayCameras.IsPresent
    LoadTestMaxPlayers = $LoadTestMaxPlayers
    MaxLocalClientsWithoutExplicitOverride = $MaxLocalClientsWithoutExplicitOverride
    ProcessExitGraceSeconds = $ProcessExitGraceSeconds
    AllowHighLocalClientCount = $AllowHighLocalClientCount.IsPresent
    SteamAppIdPaths = $steamAppIdPaths
    OverallUnstable = $overallUnstable
    Rounds = $overallResults
}

$summaryObject | ConvertTo-Json -Depth 8 | Set-Content -Path $summaryJsonPath -Encoding UTF8

$txt = @()
$txt += ("RunDir={0}" -f $runDir)
$txt += ("ProjectPath={0}" -f $ProjectPath)
$txt += ("EditorCmdPath={0}" -f $EditorCmdPath)
$txt += ("UseProjectBinaries={0} ServerBinaryPath={1} ClientBinaryPath={2}" -f $UseProjectBinaries.IsPresent, $ServerBinaryPath, $ClientBinaryPath)
$txt += ("Map={0} Port={1}" -f $Map, $Port)
$txt += ("ClientCounts={0}" -f ($parsedCounts -join ","))
$txt += ("InitialConnectTimeoutSeconds={0} ConnectionTimeoutSeconds={1}" -f $InitialConnectTimeoutSeconds, $ConnectionTimeoutSeconds)
$txt += ("WithLagLoss={0} PktLag={1} PktLoss={2}" -f $WithLagLoss.IsPresent, $PktLag, $PktLoss)
$txt += ("WithInsightsTrace={0} TraceChannels={1}" -f $WithInsightsTrace.IsPresent, $TraceChannels)
$txt += ("WithPerfStats={0}" -f $WithPerfStats.IsPresent)
$txt += ("EnableGameplayCameras={0}" -f $EnableGameplayCameras.IsPresent)
$txt += ("LoadTestMaxPlayers={0}" -f $LoadTestMaxPlayers)
$txt += ("MaxLocalClientsWithoutExplicitOverride={0} ProcessExitGraceSeconds={1} AllowHighLocalClientCount={2}" -f $MaxLocalClientsWithoutExplicitOverride, $ProcessExitGraceSeconds, $AllowHighLocalClientCount.IsPresent)
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
    if (![string]::IsNullOrWhiteSpace($round.ServerTrace)) {
        $txt += ("ServerTrace={0}" -f $round.ServerTrace)
    }
    $txt += ("ServerMaxPlayers={0}" -f $round.ServerMaxPlayers)
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
