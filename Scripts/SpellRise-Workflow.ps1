param(
    [ValidateSet("build","editor","smoke","all")]
    [string]$Mode = "all",
    [switch]$NoSteam,
    [switch]$SkipLagLoss
)

$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$ProjectPath = (Resolve-Path (Join-Path $ProjectRoot "SpellRise.uproject")).Path
$BuildBat = "C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat"
$EditorExe = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
$SmokeGateScript = (Resolve-Path (Join-Path $PSScriptRoot "Run-Smoke-Gate.ps1")).Path
$LegacyGateScript = (Resolve-Path (Join-Path $PSScriptRoot "Check-NoCharacterLegacy.ps1")).Path

function Invoke-SpellRiseBuild {
    Write-Host "[Workflow] Gate anti-legado Character/Pawn..."
    & powershell -ExecutionPolicy Bypass -File $LegacyGateScript -ProjectRoot $ProjectRoot
    if ($LASTEXITCODE -ne 0) {
        throw "Gate anti-legado falhou com codigo $LASTEXITCODE"
    }

    if (!(Test-Path $BuildBat)) {
        throw "Build.bat nao encontrado em: $BuildBat"
    }

    Write-Host "[Workflow] Fechando UnrealEditor (se estiver aberto)..."
    Get-Process UnrealEditor -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 1

    Write-Host "[Workflow] Build SpellRiseEditor Win64 Development (Unreal Source)..."
    & $BuildBat SpellRiseEditor Win64 Development $ProjectPath -WaitMutex -NoHotReloadFromIDE
    if ($LASTEXITCODE -ne 0) {
        throw "Build falhou com codigo $LASTEXITCODE"
    }
}

function Invoke-SpellRiseEditor {
    if (!(Test-Path $EditorExe)) {
        throw "UnrealEditor.exe nao encontrado em: $EditorExe"
    }

    Write-Host "[Workflow] Abrindo editor..."
    Start-Process -FilePath $EditorExe -ArgumentList ('"{0}"' -f $ProjectPath) | Out-Null
}

function Invoke-SpellRiseSmoke {
    $smokeArgs = @(
        "-ExecutionPolicy", "Bypass",
        "-File", $SmokeGateScript
    )

    if ($NoSteam.IsPresent) {
        $smokeArgs += "-NoSteam"
    }
    if ($SkipLagLoss.IsPresent) {
        $smokeArgs += "-SkipLagLoss"
    }

    Write-Host "[Workflow] Rodando smoke gate DS+2..."
    & powershell @smokeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Smoke Gate falhou com codigo $LASTEXITCODE"
    }
}

switch ($Mode) {
    "build" {
        Invoke-SpellRiseBuild
    }
    "editor" {
        Invoke-SpellRiseEditor
    }
    "smoke" {
        Invoke-SpellRiseSmoke
    }
    "all" {
        Invoke-SpellRiseBuild
        Invoke-SpellRiseEditor
        Invoke-SpellRiseSmoke
    }
}

Write-Host ("[Workflow] OK ({0})" -f $Mode)
