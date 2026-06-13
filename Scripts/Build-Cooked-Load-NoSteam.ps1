param(
    [string]$EngineRoot = "C:\UnrealSource\UnrealEngine",
    [string]$ProjectPath = "",
    [string]$ArchiveDirectory = "",
    [string]$Map = "/Game/Maps/Stylized/Stylized",
    [ValidateSet("Both", "Client", "Server")]
    [string]$TargetSet = "Both",
    [switch]$SkipBuild,
    [switch]$SkipCook,
    [switch]$NoPak
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Join-Path $PSScriptRoot ".."
if ([string]::IsNullOrWhiteSpace($ProjectPath)) {
    $ProjectPath = (Resolve-Path (Join-Path $ProjectRoot "SpellRise.uproject")).Path
}

if ([string]::IsNullOrWhiteSpace($ArchiveDirectory)) {
    $ArchiveDirectory = Join-Path $ProjectRoot "Saved\LoadTestBuild"
}

$runUat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
if (!(Test-Path $runUat)) {
    Write-Error "RunUAT.bat nao encontrado em: $runUat"
    exit 1
}

if (!(Test-Path $ProjectPath)) {
    Write-Error "SpellRise.uproject nao encontrado em: $ProjectPath"
    exit 1
}

function Invoke-BuildCookRun {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Label,
        [Parameter(Mandatory = $true)]
        [string[]]$ExtraArgs
    )

    $uatArgs = @(
        "BuildCookRun",
        "-project=`"$ProjectPath`"",
        "-noP4",
        "-cook",
        "-stage",
        "-archive",
        "-archivedirectory=`"$ArchiveDirectory`"",
        "-map=$Map",
        "-unattended",
        "-utf8output"
    )

    if (!$NoPak.IsPresent) {
        $uatArgs += "-pak"
    }

    if ($SkipBuild.IsPresent) {
        $uatArgs += "-skipbuild"
    }

    if ($SkipCook.IsPresent) {
        $uatArgs += "-skipcook"
    }

    $uatArgs += $ExtraArgs

    Write-Host "[COOK-LOAD] RunUAT: $runUat"
    Write-Host "[COOK-LOAD] ArchiveDirectory: $ArchiveDirectory"
    Write-Host "[COOK-LOAD] Target: $Label"
    Write-Host "[COOK-LOAD] Args: $($uatArgs -join ' ')"

    & $runUat @uatArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "BuildCookRun $Label falhou com exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

if ($TargetSet -eq "Both" -or $TargetSet -eq "Client") {
    Invoke-BuildCookRun -Label "Client" -ExtraArgs @(
        "-platform=Win64",
        "-clientconfig=Development",
        "-target=SpellRiseClient",
        "-client"
    )
}

if ($TargetSet -eq "Both" -or $TargetSet -eq "Server") {
    Invoke-BuildCookRun -Label "Server" -ExtraArgs @(
        "-serverplatform=Win64",
        "-serverconfig=Development",
        "-servertarget=SpellRiseServer",
        "-server",
        "-noclient"
    )
}

Write-Host "[COOK-LOAD] Completed"
