param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
)

$ProjectPath = Join-Path $PSScriptRoot "..\SpellRise.uproject"
$ProjectPath = (Resolve-Path $ProjectPath).Path
$SteamAppIdPath = Join-Path (Split-Path $EditorPath -Parent) "steam_appid.txt"

if (!(Test-Path $EditorPath)) {
    Write-Error "UnrealEditor.exe nao encontrado em: $EditorPath"
    exit 1
}

Set-Content -Path $SteamAppIdPath -Value "480" -Encoding ascii
Write-Host "steam_appid.txt configurado em: $SteamAppIdPath"

Start-Process -FilePath $EditorPath -ArgumentList ('"{0}"' -f $ProjectPath)
