param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$Map = "/Game/Maps/Stylized/Stylized"
)

$ProjectPath = Join-Path $PSScriptRoot "..\SpellRise.uproject"
$ProjectPath = (Resolve-Path $ProjectPath).Path

if (!(Test-Path $EditorPath)) {
    Write-Error "UnrealEditor.exe nao encontrado em: $EditorPath"
    exit 1
}

# Executa o jogo via Unreal Source (sem UI do editor)
Start-Process -FilePath $EditorPath -ArgumentList @(
    ('"{0}"' -f $ProjectPath),
    $Map,
    "-game",
    "-log"
)
