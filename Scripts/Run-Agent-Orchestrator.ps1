param(
    [ValidateSet("checklist","run")]
    [string]$Mode = "checklist",
    [string[]]$Stages = @("build","gas","net","input","smoke"),
    [switch]$NoSteam,
    [switch]$SkipLagLoss,
    [switch]$CopyPrompt,
    [switch]$FailFast,
    [switch]$ManualBlocksRun
)

$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$AgentsRoot = Join-Path $ProjectRoot "Fontes\Agentes"
$WorkflowScript = Join-Path $PSScriptRoot "SpellRise-Workflow.ps1"
$ReportRoot = Join-Path $ProjectRoot "Saved\Logs\AgentOrchestrator"

if (!(Test-Path $ReportRoot)) {
    New-Item -ItemType Directory -Path $ReportRoot -Force | Out-Null
}

if ($Stages.Count -eq 0) {
    throw "Parametro -Stages vazio. Informe ao menos um stage."
}

$NormalizedStages = @()
foreach ($rawStage in $Stages) {
    if ([string]::IsNullOrWhiteSpace($rawStage)) {
        continue
    }

    $parts = $rawStage.Split(",", [System.StringSplitOptions]::RemoveEmptyEntries)
    foreach ($part in $parts) {
        $candidate = $part.Trim().ToLowerInvariant()
        if (![string]::IsNullOrWhiteSpace($candidate)) {
            $NormalizedStages += $candidate
        }
    }
}

if ($NormalizedStages.Count -eq 0) {
    throw "Nenhum stage valido foi informado em -Stages."
}

$Stages = $NormalizedStages

$ValidStages = @("build","gas","net","input","smoke")
foreach ($stage in $Stages) {
    if ($ValidStages -notcontains $stage) {
        throw "Stage invalido: '$stage'. Valores aceitos: $($ValidStages -join ', ')"
    }
}

if (!$PSBoundParameters.ContainsKey("FailFast")) {
    $FailFast = $true
}

$AgentMap = @{
    build = @{
        Name = "BuildGuardian"
        File = "BuildGuardian.md"
        Focus = "Compilacao Unreal Source e causa raiz de erro."
        ExecutionType = "automated"
    }
    gas = @{
        Name = "GASAuthorityGuard"
        File = "GASAuthorityGuard.md"
        Focus = "Authority GAS, commit/validation/prediction."
        ExecutionType = "manual"
    }
    net = @{
        Name = "ReplicationSentinel"
        File = "ReplicationSentinel.md"
        Focus = "RPC, OnRep, ownership e seguranca de rede."
        ExecutionType = "manual"
    }
    input = @{
        Name = "InputContextArchitect"
        File = "InputContextArchitect.md"
        Focus = "IMC/IA, prioridades e gates de consumo."
        ExecutionType = "manual"
    }
    smoke = @{
        Name = "SmokeDSVerifier"
        File = "SmokeDSVerifier.md"
        Focus = "Smoke DS+2, reconnect e lag/loss."
        ExecutionType = "automated"
    }
}

function Get-QuickPromptFromTemplate {
    param(
        [Parameter(Mandatory = $true)]
        [string]$TemplatePath
    )

    if (!(Test-Path $TemplatePath)) {
        throw "Template nao encontrado: $TemplatePath"
    }

    $content = Get-Content -Raw -Path $TemplatePath
    $matches = [regex]::Matches($content, '```text\s*(.*?)\s*```', 'Singleline')
    if ($matches.Count -eq 0) {
        return ""
    }

    return $matches[$matches.Count - 1].Groups[1].Value.Trim()
}

function Copy-PromptToClipboard {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Prompt
    )

    if ([string]::IsNullOrWhiteSpace($Prompt)) {
        return
    }

    try {
        Set-Clipboard -Value $Prompt
        Write-Host "[Orchestrator] Prompt copiado para area de transferencia."
    }
    catch {
        Write-Warning "[Orchestrator] Falha ao copiar prompt para area de transferencia."
    }
}

function Invoke-AutomatedStage {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Stage
    )

    if (!(Test-Path $WorkflowScript)) {
        throw "Workflow script nao encontrado: $WorkflowScript"
    }

    switch ($Stage) {
        "build" {
            & powershell -ExecutionPolicy Bypass -File $WorkflowScript -Mode build
            if ($LASTEXITCODE -ne 0) { throw "Build falhou (codigo $LASTEXITCODE)." }
        }
        "smoke" {
            $smokeArgs = @("-ExecutionPolicy", "Bypass", "-File", $WorkflowScript, "-Mode", "smoke")
            if ($NoSteam.IsPresent) { $smokeArgs += "-NoSteam" }
            if ($SkipLagLoss.IsPresent) { $smokeArgs += "-SkipLagLoss" }

            & powershell @smokeArgs
            if ($LASTEXITCODE -ne 0) { throw "Smoke falhou (codigo $LASTEXITCODE)." }
        }
        default {
            throw "Stage automatizado nao suportado: $Stage"
        }
    }
}

function New-ManualTaskFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RunId,
        [Parameter(Mandatory = $true)]
        [string]$Stage,
        [Parameter(Mandatory = $true)]
        [string]$AgentName,
        [Parameter(Mandatory = $true)]
        [string]$Prompt,
        [Parameter(Mandatory = $true)]
        [string]$OutputDir
    )

    $taskPath = Join-Path $OutputDir ("{0}-{1}-task.md" -f $RunId, $Stage)

    $content = @(
        "# Agent Task - $Stage"
        ""
        "- RunId: $RunId"
        "- Agent: $AgentName"
        "- Stage: $Stage"
        "- Status: manual-required"
        ""
        "## Prompt"
        '```text'
        $Prompt
        '```'
        ""
        "## Mandatory Checks"
        "- Authority/prediction/RPC/OnRep risk review"
        "- Dedicated Server viability (no UI dependency)"
        "- GAS split: activation/commit/validation/GE/cue/cosmetic/end"
        "- Network budget declaration (NetUpdateFrequency, max RPC/s, max payload bytes, max target-data rate)"
        "- Validation matrix: Standalone, Listen, DS+2, DS+2 lag/loss"
    )

    Set-Content -Path $taskPath -Value $content -Encoding UTF8
    return $taskPath
}

$runStart = Get-Date
$runId = $runStart.ToString("yyyyMMdd-HHmmss")
$outputDir = Join-Path $ReportRoot $runId
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null

$runSummary = [ordered]@{
    RunId = $runId
    Mode = $Mode
    StartAt = $runStart.ToString("s")
    EndAt = $null
    DurationSeconds = 0
    ProjectRoot = $ProjectRoot
    StagesRequested = $Stages
    FailFast = [bool]$FailFast
    ManualBlocksRun = [bool]$ManualBlocksRun
    NetworkBudgetChecklist = @(
        @{ Item = "NetUpdateFrequency por ator relevante"; Mandatory = $true; Status = "required" },
        @{ Item = "Max RPC por segundo por jogador"; Mandatory = $true; Status = "required" },
        @{ Item = "Max payload de RPC (bytes)"; Mandatory = $true; Status = "required" },
        @{ Item = "Max taxa de envio de target data"; Mandatory = $true; Status = "required" }
    )
    ValidationMatrix = @(
        @{ Item = "Standalone"; Mandatory = $true; Status = "required" },
        @{ Item = "Listen Server"; Mandatory = $true; Status = "required" },
        @{ Item = "Dedicated Server 2+ clients"; Mandatory = $true; Status = "required" },
        @{ Item = "Dedicated Server 2+ clients with lag/loss"; Mandatory = $true; Status = "required" }
    )
    Stages = @()
    OverallStatus = "ok"
}

foreach ($stage in $Stages) {
    $stageCfg = $AgentMap[$stage]
    $templatePath = Join-Path $AgentsRoot $stageCfg.File
    $prompt = Get-QuickPromptFromTemplate -TemplatePath $templatePath

    $stageStart = Get-Date
    $stageRecord = [ordered]@{
        Stage = $stage
        Agent = $stageCfg.Name
        Focus = $stageCfg.Focus
        Template = $templatePath
        ExecutionType = $stageCfg.ExecutionType
        StartAt = $stageStart.ToString("s")
        EndAt = $null
        DurationSeconds = 0
        Status = "pending"
        Notes = ""
        PromptFile = $null
    }

    Write-Host ("[Orchestrator] Stage '{0}' via {1} ({2})" -f $stage, $stageCfg.Name, $stageCfg.ExecutionType)

    try {
        if ($Mode -eq "checklist") {
            Write-Host "[Orchestrator] Prompt sugerido:"
            Write-Host "--------------------"
            Write-Host $prompt
            Write-Host "--------------------"
            if ($CopyPrompt.IsPresent) {
                Copy-PromptToClipboard -Prompt $prompt
            }
            $stageRecord.Status = "checklist-ready"
            $stageRecord.Notes = "Prompt carregado do template."
        }
        else {
            if ($stageCfg.ExecutionType -eq "automated") {
                Invoke-AutomatedStage -Stage $stage
                $stageRecord.Status = "ok"
                $stageRecord.Notes = "Execucao automatica concluida."
            }
            else {
                $taskFile = New-ManualTaskFile -RunId $runId -Stage $stage -AgentName $stageCfg.Name -Prompt $prompt -OutputDir $outputDir
                $stageRecord.PromptFile = $taskFile
                $stageRecord.Status = "manual-required"
                $stageRecord.Notes = "Stage exige revisao manual com o template do agente."

                if ($ManualBlocksRun.IsPresent) {
                    throw "Stage manual bloqueado por -ManualBlocksRun."
                }
            }
        }
    }
    catch {
        $stageRecord.Status = "failed"
        $stageRecord.Notes = $_.Exception.Message
        $runSummary.OverallStatus = "failed"

        if ($FailFast.IsPresent) {
            $stageEnd = Get-Date
            $stageRecord.EndAt = $stageEnd.ToString("s")
            $stageRecord.DurationSeconds = [math]::Round(($stageEnd - $stageStart).TotalSeconds, 2)
            $runSummary.Stages += $stageRecord
            break
        }
    }

    if ($stageRecord.Status -eq "manual-required" -and $runSummary.OverallStatus -ne "failed") {
        $runSummary.OverallStatus = "manual-required"
    }

    $stageEnd = Get-Date
    $stageRecord.EndAt = $stageEnd.ToString("s")
    $stageRecord.DurationSeconds = [math]::Round(($stageEnd - $stageStart).TotalSeconds, 2)
    $runSummary.Stages += $stageRecord
}

$runEnd = Get-Date
$runSummary.EndAt = $runEnd.ToString("s")
$runSummary.DurationSeconds = [math]::Round(($runEnd - $runStart).TotalSeconds, 2)

$summaryJsonPath = Join-Path $outputDir ("{0}-summary.json" -f $runId)
$summaryTxtPath = Join-Path $outputDir ("{0}-summary.txt" -f $runId)

$runSummary | ConvertTo-Json -Depth 8 | Set-Content -Path $summaryJsonPath -Encoding UTF8

$txt = @()
$txt += "RunId: $($runSummary.RunId)"
$txt += "Mode: $($runSummary.Mode)"
$txt += "OverallStatus: $($runSummary.OverallStatus)"
$txt += "StartAt: $($runSummary.StartAt)"
$txt += "EndAt: $($runSummary.EndAt)"
$txt += "DurationSeconds: $($runSummary.DurationSeconds)"
$txt += ""
$txt += "Stages:"
foreach ($s in $runSummary.Stages) {
    $txt += ("- {0}: {1} ({2}s) [{3}]" -f $s.Stage, $s.Status, $s.DurationSeconds, $s.Agent)
    if (![string]::IsNullOrWhiteSpace($s.Notes)) {
        $txt += ("  Notes: {0}" -f $s.Notes)
    }
    if (![string]::IsNullOrWhiteSpace($s.PromptFile)) {
        $txt += ("  PromptFile: {0}" -f $s.PromptFile)
    }
}
$txt += ""
$txt += "Network Budget Checklist (mandatory):"
foreach ($item in $runSummary.NetworkBudgetChecklist) {
    $txt += ("- {0}" -f $item.Item)
}
$txt += ""
$txt += "Validation Matrix (mandatory):"
foreach ($item in $runSummary.ValidationMatrix) {
    $txt += ("- {0}" -f $item.Item)
}

Set-Content -Path $summaryTxtPath -Value $txt -Encoding UTF8

Write-Host ""
Write-Host ("[Orchestrator] Run finalizado com status: {0}" -f $runSummary.OverallStatus)
Write-Host ("[Orchestrator] Resumo JSON: {0}" -f $summaryJsonPath)
Write-Host ("[Orchestrator] Resumo TXT: {0}" -f $summaryTxtPath)

if ($runSummary.OverallStatus -eq "failed") {
    exit 1
}

if ($runSummary.OverallStatus -eq "manual-required") {
    exit 2
}

exit 0
