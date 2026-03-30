param(
    [string]$EditorPath = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe",
    [string]$Map = "/Game/Maps/Stylized/Stylized",
    [string]$PersistentSeed = "77777777",
    [string]$PgConn = "",
    [string]$PsqlPath = ""
)

$ErrorActionPreference = "Stop"

$ProjectPath = Join-Path $PSScriptRoot "..\SpellRise.uproject"
$ProjectPath = (Resolve-Path $ProjectPath).Path

if (!(Test-Path $EditorPath)) {
    Write-Error "UnrealEditor.exe nao encontrado em: $EditorPath"
    exit 1
}

if ([string]::IsNullOrWhiteSpace($PgConn)) {
    $PgConn = $env:SR_PG_CONN
}

if ([string]::IsNullOrWhiteSpace($PsqlPath)) {
    $PsqlPath = $env:SR_PSQL_PATH
}

if ([string]::IsNullOrWhiteSpace($PgConn)) {
    Write-Error "SR_PG_CONN nao definido. Defina a variavel de ambiente (ver Database/README.md)."
    exit 1
}

if ([string]::IsNullOrWhiteSpace($PsqlPath)) {
    Write-Error "SR_PSQL_PATH nao definido. Defina a variavel de ambiente (ver Database/README.md)."
    exit 1
}

if (!(Test-Path $PsqlPath)) {
    Write-Error "psql.exe nao encontrado em: $PsqlPath"
    exit 1
}

$env:SR_PG_CONN = $PgConn
$env:SR_PSQL_PATH = $PsqlPath

$args = @(
    ('"{0}"' -f $ProjectPath),
    $Map,
    "-game",
    "-log",
    "-nosteam",
    "-SRPersistenceProvider=postgres",
    ('-SRPsqlPath="{0}"' -f $PsqlPath),
    ('-SRPgConn="{0}"' -f $PgConn),
    ('-SRTestPersistentId={0}' -f $PersistentSeed)
)

Write-Host "[SpellRise] Iniciando com PostgreSQL + NoSteam + TestPersistentId..."
Write-Host ("[SpellRise] SR_PSQL_PATH={0}" -f $PsqlPath)
Write-Host "[SpellRise] SR_PG_CONN=<redacted>"
Write-Host ("[SpellRise] SRTestPersistentId={0}" -f $PersistentSeed)

Start-Process -FilePath $EditorPath -ArgumentList $args

exit 0
