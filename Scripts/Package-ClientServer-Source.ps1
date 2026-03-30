param(
    [string]$EngineRoot = "C:\UnrealSource\UnrealEngine",
    [string]$DesktopOutputRoot = "$env:USERPROFILE\Desktop\SpellRise_TestBuild",
    [string]$Map = "/Game/Maps/Stylized/Stylized",
    [ValidateSet("Development","Shipping")]
    [string]$Config = "Development",
    [int]$SteamAppId = 480,
    [switch]$CleanOutput
)

$ErrorActionPreference = "Stop"

$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$projectPath = (Resolve-Path (Join-Path $projectRoot "SpellRise.uproject")).Path
$runUat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"

if (!(Test-Path $runUat)) {
    throw "RunUAT.bat nao encontrado em: $runUat"
}

function Resolve-SteamApi64Source {
    param(
        [string]$ProjectRoot,
        [string]$EngineRoot
    )

    $candidates = @(
        (Join-Path $ProjectRoot "Build\SteamRedist\Win64\steam_api64.dll")
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    $steamworksRoot = Join-Path $EngineRoot "Engine\Binaries\ThirdParty\Steamworks"
    if (Test-Path $steamworksRoot) {
        $latest = Get-ChildItem -Path $steamworksRoot -Directory -Filter "Steamv*" |
            Sort-Object Name -Descending |
            Select-Object -First 1
        if ($null -ne $latest) {
            $fromEngine = Join-Path $latest.FullName "Win64\steam_api64.dll"
            if (Test-Path $fromEngine) {
                return $fromEngine
            }
        }
    }

    return $null
}

function Resolve-SteamClientRedistributablesDir {
    param(
        [string]$ProjectRoot
    )

    $programFilesX86 = ${env:ProgramFiles(x86)}
    $candidates = @(
        (Join-Path $ProjectRoot "Build\SteamRedist\Win64"),
        (Join-Path $programFilesX86 "Steam"),
        "C:\Program Files (x86)\Steam"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path (Join-Path $candidate "steamclient64.dll")) {
            return $candidate
        }
    }

    return $null
}

function Stage-SteamRuntime {
    param(
        [string]$TargetBinDir,
        [string]$SteamApi64Source,
        [string]$SteamRedistributablesDir,
        [int]$SteamAppId
    )

    if (!(Test-Path $TargetBinDir)) {
        throw "Diretorio alvo nao encontrado para staging Steam: $TargetBinDir"
    }

    if ([string]::IsNullOrWhiteSpace($SteamApi64Source) -or !(Test-Path $SteamApi64Source)) {
        throw "steam_api64.dll nao encontrada. Coloque em Build\\SteamRedist\\Win64 ou valide EngineRoot."
    }

    if ([string]::IsNullOrWhiteSpace($SteamRedistributablesDir) -or !(Test-Path $SteamRedistributablesDir)) {
        throw "Diretorio de redistribuiveis Steam nao encontrado. Instale Steam/SteamCMD ou preencha Build\\SteamRedist\\Win64."
    }

    Copy-Item -LiteralPath $SteamApi64Source -Destination (Join-Path $TargetBinDir "steam_api64.dll") -Force

    $required = @("steamclient64.dll", "tier0_s64.dll", "vstdlib_s64.dll")
    foreach ($dll in $required) {
        $sourcePath = Join-Path $SteamRedistributablesDir $dll
        if (!(Test-Path $sourcePath)) {
            throw "DLL Steam obrigatoria ausente: $sourcePath"
        }
        Copy-Item -LiteralPath $sourcePath -Destination (Join-Path $TargetBinDir $dll) -Force
    }

    Set-Content -LiteralPath (Join-Path $TargetBinDir "steam_appid.txt") -Value ([string]$SteamAppId) -Encoding ASCII
}

$clientOut = Join-Path $DesktopOutputRoot "Client"
$serverOut = Join-Path $DesktopOutputRoot "Server"

$steamApi64Source = Resolve-SteamApi64Source -ProjectRoot $projectRoot -EngineRoot $EngineRoot
$steamRedistributablesDir = Resolve-SteamClientRedistributablesDir -ProjectRoot $projectRoot

if ($CleanOutput.IsPresent) {
    if (Test-Path $clientOut) { Remove-Item -LiteralPath $clientOut -Recurse -Force }
    if (Test-Path $serverOut) { Remove-Item -LiteralPath $serverOut -Recurse -Force }
}

New-Item -ItemType Directory -Path $clientOut -Force | Out-Null
New-Item -ItemType Directory -Path $serverOut -Force | Out-Null

Write-Host "[Package] Client Win64 ($Config)..."
& $runUat BuildCookRun `
    -project="$projectPath" `
    -noP4 `
    -utf8output `
    -target=SpellRiseClient `
    -platform=Win64 `
    ("-clientconfig={0}" -f $Config) `
    -build `
    -cook `
    -stage `
    -pak `
    -archive `
    -archivedirectory="$clientOut" `
    ("-map={0}" -f $Map)
if ($LASTEXITCODE -ne 0) {
    throw "Package do client falhou com codigo $LASTEXITCODE"
}

Write-Host "[Package] Server Win64 ($Config)..."
& $runUat BuildCookRun `
    -project="$projectPath" `
    -noP4 `
    -utf8output `
    -target=SpellRiseServer `
    -server `
    -serverplatform=Win64 `
    ("-serverconfig={0}" -f $Config) `
    -noclient `
    -build `
    -cook `
    -stage `
    -pak `
    -archive `
    -archivedirectory="$serverOut" `
    ("-map={0}" -f $Map)
if ($LASTEXITCODE -ne 0) {
    throw "Package do server falhou com codigo $LASTEXITCODE"
}

$appIdText = [string]$SteamAppId
$serverAppIdCandidates = @(
    (Join-Path $serverOut "WindowsServer\steam_appid.txt"),
    (Join-Path $serverOut "WindowsServer\SpellRise\Binaries\Win64\steam_appid.txt")
)

foreach ($candidate in $serverAppIdCandidates) {
    $parent = Split-Path -Parent $candidate
    if (Test-Path $parent) {
        Set-Content -LiteralPath $candidate -Value $appIdText -Encoding ASCII
    }
}

$clientAppIdCandidates = @(
    (Join-Path $clientOut "WindowsClient\steam_appid.txt"),
    (Join-Path $clientOut "WindowsClient\SpellRise\Binaries\Win64\steam_appid.txt")
)

foreach ($candidate in $clientAppIdCandidates) {
    $parent = Split-Path -Parent $candidate
    if (Test-Path $parent) {
        Set-Content -LiteralPath $candidate -Value $appIdText -Encoding ASCII
    }
}

$serverBinDir = Join-Path $serverOut "WindowsServer\SpellRise\Binaries\Win64"
if (Test-Path $serverBinDir) {
    Stage-SteamRuntime -TargetBinDir $serverBinDir -SteamApi64Source $steamApi64Source -SteamRedistributablesDir $steamRedistributablesDir -SteamAppId $SteamAppId
}

$clientBinDir = Join-Path $clientOut "WindowsClient\SpellRise\Binaries\Win64"
if (Test-Path $clientBinDir) {
    Stage-SteamRuntime -TargetBinDir $clientBinDir -SteamApi64Source $steamApi64Source -SteamRedistributablesDir $steamRedistributablesDir -SteamAppId $SteamAppId
}

Write-Host "[Package] Concluido com sucesso."
Write-Host "[Package] Client: $clientOut"
Write-Host "[Package] Server: $serverOut"
