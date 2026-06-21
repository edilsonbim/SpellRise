[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectPath,

    [string]$EngineRoot = "C:\UnrealSource\UnrealEngine",

    [ValidateRange(1, 60)]
    [int]$PollSeconds = 2,

    [string]$Model = "gpt-5.4",

    [switch]$Once
)

$ErrorActionPreference = "Stop"

function Write-ConsumerLog {
    param(
        [string]$Message,
        [ValidateSet("INFO", "WARN", "ERROR")]
        [string]$Level = "INFO"
    )

    $line = "[{0}] [{1}] {2}" -f (Get-Date -Format "yyyy-MM-dd HH:mm:ss"), $Level, $Message
    Add-Content -LiteralPath $script:LogPath -Value $line -Encoding UTF8
}

function Write-JsonFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [object]$Value
    )

    $json = $Value | ConvertTo-Json -Depth 20
    $tempPath = "$Path.tmp"
    Set-Content -LiteralPath $tempPath -Value $json -Encoding UTF8
    Move-Item -Force -LiteralPath $tempPath -Destination $Path
}

function Get-BlueprintSnapshot {
    param(
        [Parameter(Mandatory = $true)]
        [string]$AssetPath,

        [Parameter(Mandatory = $true)]
        [string]$RequestId
    )

    if ([string]::IsNullOrWhiteSpace($AssetPath)) {
        return $null
    }

    $snapshotRequestPath = Join-Path $script:QueuePath "$RequestId.snapshot.request.json"
    $snapshotResponsePath = Join-Path $script:QueuePath "$RequestId.snapshot.response.json"
    Write-JsonFile -Path $snapshotRequestPath -Value ([ordered]@{
        version = 1
        operation = "DescribeBlueprint"
        asset = $AssetPath
    })

    $arguments = @(
        "`"$script:ResolvedProjectPath`""
        "-run=CodexBlueprintBridge"
        "-Input=`"$snapshotRequestPath`""
        "-Output=`"$snapshotResponsePath`""
        "-unattended"
        "-nop4"
        "-nosplash"
        "-nullrhi"
    )

    $process = Start-Process `
        -FilePath $script:UnrealEditorCmdPath `
        -ArgumentList $arguments `
        -Wait `
        -PassThru `
        -WindowStyle Hidden

    if ($process.ExitCode -ne 0 -or -not (Test-Path -LiteralPath $snapshotResponsePath)) {
        throw "DescribeBlueprint failed with exit code $($process.ExitCode)."
    }

    return Get-Content -Raw -LiteralPath $snapshotResponsePath | ConvertFrom-Json
}

function Invoke-CodexPrompt {
    param(
        [Parameter(Mandatory = $true)]
        [pscustomobject]$Request,

        [AllowNull()]
        [object]$Snapshot,

        [Parameter(Mandatory = $true)]
        [string]$RequestId
    )

    $codexOutputPath = Join-Path $script:QueuePath "$RequestId.codex-output.json"
    $snapshotJson = if ($null -eq $Snapshot) { "null" } else { $Snapshot | ConvertTo-Json -Depth 20 }
    $prompt = @"
Voce esta revisando um Blueprint do projeto SpellRise, Unreal Engine 5.7.

Responda em portugues do Brasil, de forma objetiva e orientada a producao.
Esta execucao e estritamente read-only:
- nao altere arquivos;
- nao altere assets;
- nao execute CompileBlueprint ou SaveBlueprint;
- nao proponha trust no cliente;
- respeite Dedicated Server e gameplay server-authoritative;
- UI e apenas apresentacao/intencao.

Asset solicitado: $($Request.asset)

Prompt do usuario:
$($Request.prompt)

Snapshot produzido pelo CodexBlueprintBridge:
$snapshotJson

Retorne:
- resumo;
- analise;
- riscos de authority, prediction, RPC, replicacao e OnRep quando aplicavel;
- proximos passos;
- proposedPatch deve ser null nesta versao read-only.
"@

    $arguments = @(
        "exec"
        "--ephemeral"
        "--model"
        $Model
        "--sandbox"
        "read-only"
        "--output-schema"
        $script:SchemaPath
        "--output-last-message"
        $codexOutputPath
        "--color"
        "never"
        "-C"
        $script:ProjectDirectory
        "-"
    )

    $previousNativePreference = $PSNativeCommandUseErrorActionPreference
    $PSNativeCommandUseErrorActionPreference = $false
    try {
        $prompt | & codex @arguments 2>> $script:LogPath | Out-Null
        $codexExitCode = $LASTEXITCODE
    }
    finally {
        $PSNativeCommandUseErrorActionPreference = $previousNativePreference
    }

    if ($codexExitCode -ne 0 -or -not (Test-Path -LiteralPath $codexOutputPath)) {
        throw "codex exec failed with exit code $codexExitCode."
    }

    return Get-Content -Raw -LiteralPath $codexOutputPath | ConvertFrom-Json
}

function Process-Request {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$RequestFile
    )

    $requestId = $RequestFile.BaseName -replace "\.request$", ""
    $processingPath = Join-Path $script:QueuePath "$requestId.processing.json"
    $responsePath = Join-Path $script:QueuePath "$requestId.response.json"
    $errorPath = Join-Path $script:QueuePath "$requestId.error.json"

    if ((Test-Path -LiteralPath $responsePath) -or (Test-Path -LiteralPath $processingPath)) {
        return
    }

    try {
        Move-Item -LiteralPath $RequestFile.FullName -Destination $processingPath
        $request = Get-Content -Raw -LiteralPath $processingPath | ConvertFrom-Json

        if ($request.version -ne 1 -or $request.kind -ne "CodexBlueprintPrompt") {
            throw "Unsupported prompt request contract."
        }

        if ($request.allowAssetMutation -ne $false) {
            throw "allowAssetMutation must remain false."
        }

        Write-ConsumerLog "Processing request $requestId for asset '$($request.asset)'."
        $snapshot = Get-BlueprintSnapshot -AssetPath $request.asset -RequestId $requestId
        $codexResult = Invoke-CodexPrompt -Request $request -Snapshot $snapshot -RequestId $requestId

        Write-JsonFile -Path $responsePath -Value ([ordered]@{
            version = 1
            requestId = $requestId
            status = "completed"
            completedUtc = [DateTime]::UtcNow.ToString("o")
            asset = $request.asset
            summary = $codexResult.summary
            response = $codexResult.response
            risks = @($codexResult.risks)
            recommendedNextSteps = @($codexResult.recommendedNextSteps)
            proposedPatch = $null
        })

        Remove-Item -LiteralPath $processingPath
        Write-ConsumerLog "Completed request $requestId."
    }
    catch {
        Write-JsonFile -Path $errorPath -Value ([ordered]@{
            version = 1
            requestId = $requestId
            status = "failed"
            failedUtc = [DateTime]::UtcNow.ToString("o")
            error = $_.Exception.Message
        })
        Remove-Item -Force -LiteralPath $processingPath -ErrorAction SilentlyContinue
        Write-ConsumerLog "Request $requestId failed: $($_.Exception.Message)" "ERROR"
    }
}

$script:ResolvedProjectPath = (Resolve-Path -LiteralPath $ProjectPath).Path
$script:ProjectDirectory = Split-Path -Parent $script:ResolvedProjectPath
$script:PluginRoot = Split-Path -Parent $PSScriptRoot
$script:SchemaPath = Join-Path $PSScriptRoot "prompt-response.schema.json"
$script:QueuePath = Join-Path $script:ProjectDirectory "Saved\CodexBlueprintBridge\PromptQueue"
$script:UnrealEditorCmdPath = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$script:LogPath = Join-Path $script:QueuePath "consumer.log"
$lockPath = Join-Path $script:QueuePath "consumer.lock"

if (-not (Test-Path -LiteralPath $script:SchemaPath)) {
    throw "Response schema not found: $script:SchemaPath"
}

if (-not (Test-Path -LiteralPath $script:UnrealEditorCmdPath)) {
    throw "UnrealEditor-Cmd.exe not found: $script:UnrealEditorCmdPath"
}

if (-not (Get-Command codex -ErrorAction SilentlyContinue)) {
    throw "Codex CLI is not available in PATH."
}

New-Item -ItemType Directory -Force -Path $script:QueuePath | Out-Null

$lockStream = $null
try {
    $lockStream = [System.IO.File]::Open(
        $lockPath,
        [System.IO.FileMode]::OpenOrCreate,
        [System.IO.FileAccess]::ReadWrite,
        [System.IO.FileShare]::None)

    Write-ConsumerLog "Consumer started. Project='$script:ResolvedProjectPath'."

    do {
        Get-ChildItem -LiteralPath $script:QueuePath -Filter "*.request.json" -File |
            Where-Object { $_.Name -notlike "*.snapshot.request.json" } |
            Sort-Object CreationTimeUtc |
            ForEach-Object { Process-Request -RequestFile $_ }

        if (-not $Once) {
            Start-Sleep -Seconds $PollSeconds
        }
    } while (-not $Once)
}
finally {
    if ($null -ne $lockStream) {
        $lockStream.Dispose()
    }
    Remove-Item -Force -LiteralPath $lockPath -ErrorAction SilentlyContinue
}
