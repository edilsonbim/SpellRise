param(
    [string]$EngineRoot = "C:\UnrealSource\UnrealEngine",
    [string]$Configuration = "Development",
    [string]$ArchiveRoot = "$env:USERPROFILE\Desktop\SpellRise_TestBuild",
    [switch]$Clean,
    [switch]$Rebuild,
    [switch]$SkipClient,
    [switch]$SkipServer,
    [switch]$CreateLaunchBats
)

$ErrorActionPreference = "Stop"

$ProjectPath = Join-Path $PSScriptRoot "..\SpellRise.uproject"
$ProjectPath = (Resolve-Path $ProjectPath).Path
$ProjectRoot = Split-Path -Parent $ProjectPath

$RunUat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
if (!(Test-Path $RunUat)) {
    throw "RunUAT nao encontrado em: $RunUat"
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

$SteamAppId = 480
$SteamApi64Source = Resolve-SteamApi64Source -ProjectRoot $ProjectRoot -EngineRoot $EngineRoot
$SteamRedistributablesDir = Resolve-SteamClientRedistributablesDir -ProjectRoot $ProjectRoot

$ArchiveRoot = [System.IO.Path]::GetFullPath($ArchiveRoot)
$ClientArchive = Join-Path $ArchiveRoot "Client"
$ServerArchive = Join-Path $ArchiveRoot "Server"

New-Item -ItemType Directory -Force -Path $ArchiveRoot | Out-Null
if (-not $SkipClient) { New-Item -ItemType Directory -Force -Path $ClientArchive | Out-Null }
if (-not $SkipServer) { New-Item -ItemType Directory -Force -Path $ServerArchive | Out-Null }

$BaseArgs = @(
    "BuildCookRun",
    "-project=$ProjectPath",
    "-noP4",
    "-utf8output",
    "-build",
    "-cook",
    "-stage",
    "-pak",
    "-archive"
)

if ($Clean) { $BaseArgs += "-clean" }
if ($Rebuild) { $BaseArgs += "-rebuild" }

if (-not $SkipClient) {
    $ClientArgs = @() + $BaseArgs + @(
        "-platform=Win64",
        "-targetplatform=Win64",
        "-clientconfig=$Configuration",
        "-allmaps",
        "-prereqs",
        "-archivedirectory=$ClientArchive"
    )

    Write-Host ""
    Write-Host "[Package] Iniciando CLIENT..." -ForegroundColor Cyan
    & $RunUat @ClientArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Package do CLIENT falhou com codigo $LASTEXITCODE"
    }
}

if (-not $SkipServer) {
    $ServerArgs = @() + $BaseArgs + @(
        "-server",
        "-noclient",
        "-serverplatform=Win64",
        "-targetplatform=Win64",
        "-serverconfig=$Configuration",
        "-allmaps",
        "-archivedirectory=$ServerArchive"
    )

    Write-Host ""
    Write-Host "[Package] Iniciando SERVER..." -ForegroundColor Cyan
    & $RunUat @ServerArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Package do SERVER falhou com codigo $LASTEXITCODE"
    }
}

if ($CreateLaunchBats) {
    $ClientBin = Join-Path $ClientArchive "WindowsClient\SpellRise\Binaries\Win64"
    $ServerBin = Join-Path $ServerArchive "WindowsServer\SpellRise\Binaries\Win64"

    if (Test-Path $ClientBin) {
        $ClientSteam = @"
@echo off
setlocal
cd /d "%~dp0"
set SERVER_IP=56.124.17.172
set PORT=7777
start "SpellRise Client (Steam AWS)" "SpellRiseClient.exe" %SERVER_IP%:%PORT% -log
endlocal
"@
        Set-Content -LiteralPath (Join-Path $ClientBin "Start_Client_Steam_AWS.bat") -Value $ClientSteam -Encoding ASCII

        $ClientNoSteam = @"
@echo off
setlocal
cd /d "%~dp0"
set SERVER_IP=56.124.17.172
set PORT=7777
start "SpellRise Client (NoSteam AWS)" "SpellRiseClient.exe" %SERVER_IP%:%PORT% -log -nosteam
endlocal
"@
        Set-Content -LiteralPath (Join-Path $ClientBin "Start_Client_NoSteam_AWS.bat") -Value $ClientNoSteam -Encoding ASCII
    }

    if (Test-Path $ServerBin) {
        $ServerSteam = @"
@echo off
setlocal
cd /d "%~dp0"
set MAP=/Game/Maps/Stylized/Stylized?listen
set PORT=7777
start "SpellRise Server (Steam AWS)" "SpellRiseServer.exe" %MAP% -log -port=%PORT% -unattended -NoCrashDialog
endlocal
"@
        Set-Content -LiteralPath (Join-Path $ServerBin "Start_Server_Steam_AWS.bat") -Value $ServerSteam -Encoding ASCII

        $ServerNoSteam = @"
@echo off
setlocal
cd /d "%~dp0"
set MAP=/Game/Maps/Stylized/Stylized?listen
set PORT=7777
start "SpellRise Server (NoSteam AWS)" "SpellRiseServer.exe" %MAP% -log -nosteam -port=%PORT% -unattended -NoCrashDialog
endlocal
"@
        Set-Content -LiteralPath (Join-Path $ServerBin "Start_Server_NoSteam_AWS.bat") -Value $ServerNoSteam -Encoding ASCII
    }
}

if (-not $SkipServer) {
    $ServerBin = Join-Path $ServerArchive "WindowsServer\SpellRise\Binaries\Win64"
    if (Test-Path $ServerBin) {
        Stage-SteamRuntime -TargetBinDir $ServerBin -SteamApi64Source $SteamApi64Source -SteamRedistributablesDir $SteamRedistributablesDir -SteamAppId $SteamAppId
    }
}

if (-not $SkipClient) {
    $ClientBin = Join-Path $ClientArchive "WindowsClient\SpellRise\Binaries\Win64"
    if (Test-Path $ClientBin) {
        Stage-SteamRuntime -TargetBinDir $ClientBin -SteamApi64Source $SteamApi64Source -SteamRedistributablesDir $SteamRedistributablesDir -SteamAppId $SteamAppId
    }
}

Write-Host ""
Write-Host "[Package] Concluido com sucesso." -ForegroundColor Green
Write-Host "Client: $ClientArchive"
Write-Host "Server: $ServerArchive"
