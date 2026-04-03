param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$EditorCmdPath = "",
    [switch]$NoBuild,
    [switch]$NoSteam,
    [int]$PortBase = 17777,
    [int]$TestDurationSeconds = 55,
    [switch]$SkipLagLoss,
    [string]$TestPersistentId = "",
    [switch]$AllowHighInProgressBugLog,
    [switch]$DisableAutoNoSteamRetry
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Join-Path $PSScriptRoot ".."
$ProjectPath = (Resolve-Path (Join-Path $ProjectRoot "SpellRise.uproject")).Path
$SmokeScript = (Resolve-Path (Join-Path $PSScriptRoot "Run-Smoke-DS.ps1")).Path

if (!(Test-Path $ProjectPath)) {
    Write-Error "SpellRise.uproject nao encontrado em: $ProjectPath"
    exit 1
}

if (!(Test-Path $SmokeScript)) {
    Write-Error "Run-Smoke-DS.ps1 nao encontrado em: $SmokeScript"
    exit 1
}

if (-not $NoBuild.IsPresent) {
    $buildBat = "C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat"
    if (!(Test-Path $buildBat)) {
        Write-Error "Build.bat nao encontrado em: $buildBat"
        exit 1
    }

    Write-Host "[GATE] Build SpellRiseEditor Win64 Development..."
    & $buildBat SpellRiseEditor Win64 Development $ProjectPath -WaitMutex -NoHotReload
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build falhou com codigo $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

$shared = @(
    "-ExecutionPolicy", "Bypass",
    "-File", $SmokeScript,
    "-EditorPath", $EditorPath,
    "-TestDurationSeconds", $TestDurationSeconds,
    "-ServerExtraArgs", "-SRPersistenceAllowFileFallback",
    "-ClientExtraArgs", "-SRForceDeathOnBeginPlay",
    "-ReconnectClient1",
    "-ReconnectClient1AtSeconds", 20,
    "-ReconnectClient1DowntimeSeconds", 3,
    "-RequirePersistence"
)

if (![string]::IsNullOrWhiteSpace($EditorCmdPath)) {
    $shared += @("-EditorCmdPath", $EditorCmdPath)
}

if ($NoSteam.IsPresent) {
    $shared += "-NoSteam"
}
if (![string]::IsNullOrWhiteSpace($TestPersistentId)) {
    $shared += @("-TestPersistentId", $TestPersistentId)
}

function Invoke-SmokeScenario {
    param(
        [string]$Name,
        [bool]$WithLagLoss,
        [int]$Port,
        [int]$PktLag = 120,
        [int]$PktLoss = 1
    )

    $args = @($shared)
    $args += @("-Port", $Port)
    if ($WithLagLoss) {
        $args += @("-WithLagLoss", "-PktLag", $PktLag, "-PktLoss", $PktLoss)
    }

    Write-Host ("[GATE] Rodando cenario: {0}" -f $Name)
    & powershell @args
    $code = $LASTEXITCODE
    if ($code -eq 0) {
        return
    }

    $canRetryNoSteam = (-not $NoSteam.IsPresent) -and (-not $DisableAutoNoSteamRetry.IsPresent)
    if ($canRetryNoSteam) {
        $smokeRoot = Join-Path $ProjectRoot "Saved\Logs\SmokeAuto"
        if (Test-Path $smokeRoot) {
            $lastRunDir = Get-ChildItem -Path $smokeRoot -Directory | Sort-Object LastWriteTime -Descending | Select-Object -First 1
            if ($null -ne $lastRunDir) {
                $summaryPath = Join-Path $lastRunDir.FullName "Smoke_Summary.txt"
                $serverLogPath = Join-Path $lastRunDir.FullName "Smoke_DS_Server.stdout.log"
                $client1LogPath = Join-Path $lastRunDir.FullName "Smoke_DS_Client1.stdout.log"
                $client2LogPath = Join-Path $lastRunDir.FullName "Smoke_DS_Client2.stdout.log"

                $hasUniqueNetIdMismatch = $false
                foreach ($candidate in @($summaryPath, $serverLogPath, $client1LogPath, $client2LogPath)) {
                    if ((Test-Path $candidate) -and ((Select-String -Path $candidate -Pattern "incompatible_unique_net_id" -SimpleMatch -ErrorAction SilentlyContinue | Measure-Object).Count -gt 0)) {
                        $hasUniqueNetIdMismatch = $true
                        break
                    }
                }

                if ($hasUniqueNetIdMismatch) {
                    Write-Warning ("[GATE] Cenario '{0}' falhou por Steam/UniqueNetId mismatch. Reexecutando automaticamente com -NoSteam." -f $Name)
                    $retryArgs = @($args)
                    $retryArgs += "-NoSteam"
                    & powershell @retryArgs
                    $retryCode = $LASTEXITCODE
                    if ($retryCode -eq 0) {
                        Write-Host ("[GATE] Cenario '{0}' aprovado no retry -NoSteam." -f $Name)
                        return
                    }

                    Write-Error ("Cenario '{0}' falhou no retry -NoSteam com codigo {1}" -f $Name, $retryCode)
                    exit $retryCode
                }
            }
        }
    }

    Write-Error ("Cenario '{0}' falhou com codigo {1}" -f $Name, $code)
    exit $code
}

Invoke-SmokeScenario -Name "DS+2 reconnect (normal)" -WithLagLoss:$false -Port $PortBase

if (-not $SkipLagLoss.IsPresent) {
    Invoke-SmokeScenario -Name "DS+2 reconnect (lag/loss Perfil A: 120/1)" -WithLagLoss:$true -Port ($PortBase + 1) -PktLag 120 -PktLoss 1
    Invoke-SmokeScenario -Name "DS+2 reconnect (lag/loss Perfil B: 180/3)" -WithLagLoss:$true -Port ($PortBase + 2) -PktLag 180 -PktLoss 3
}

Write-Host "[GATE] PASS: todos os cenarios obrigatorios aprovados."

$bugLogPath = Join-Path $ProjectRoot "Fontes\BUG_LOG.md"
if ((Test-Path $bugLogPath) -and (-not $AllowHighInProgressBugLog.IsPresent)) {
    $bugLogText = Get-Content $bugLogPath -Raw
    $sections = $bugLogText -split "(?m)^### "
    $highInProgressCount = 0
    foreach ($section in $sections) {
        if ($section -match "Severity:\s*High" -and $section -match "Status:\s*In Progress") {
            $highInProgressCount += 1
        }
    }
    if ($highInProgressCount -gt 0) {
        Write-Error ("[GATE] Bloqueado: BUG_LOG possui {0} itens High em In Progress." -f $highInProgressCount)
        exit 3
    }
}

Write-Host "[GATE] PASS final: smoke + gate de BUG_LOG aprovados."
exit 0
