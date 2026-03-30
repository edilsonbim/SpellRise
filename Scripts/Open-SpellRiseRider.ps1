param(
    [string]$RiderPath = "C:\Program Files\JetBrains\JetBrains Rider 2025.3.1\bin\rider64.exe",
    [switch]$FixLock
)

$ProjectPath = Join-Path $PSScriptRoot "..\SpellRise.sln"
$ProjectPath = (Resolve-Path $ProjectPath).Path

$JetBrainsLocal = Join-Path $env:LOCALAPPDATA "JetBrains\Rider2025.3"
$PortPath = Join-Path $JetBrainsLocal ".port"

if (!(Test-Path $RiderPath)) {
    Write-Error "Rider nao encontrado em: $RiderPath"
    exit 1
}

if ($FixLock -and (Test-Path $PortPath)) {
    try {
        Remove-Item -Force $PortPath -ErrorAction Stop
        Write-Host "Lock de porta removido: $PortPath"
    } catch {
        Write-Warning "Nao foi possivel remover $PortPath. Feche processos Rider/JetBrains e tente novamente."
    }
}

Start-Process -FilePath $RiderPath -ArgumentList ('"{0}"' -f $ProjectPath)

