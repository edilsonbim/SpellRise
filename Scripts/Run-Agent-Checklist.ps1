param(
    [ValidateSet("build","gas","net","input","smoke","all")]
    [string]$TaskType = "all",
    [ValidateSet("checklist","run")]
    [string]$Mode = "checklist",
    [switch]$NoSteam,
    [switch]$SkipLagLoss,
    [switch]$CopyPrompt
)

$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$AgentsRoot = Join-Path $ProjectRoot "Fontes\Agentes"
$WorkflowScript = Join-Path $PSScriptRoot "SpellRise-Workflow.ps1"

$AgentMap = @{
    build = @{
        Name = "BuildGuardian"
        File = "BuildGuardian.md"
        Focus = "Compilacao Unreal Source e causa raiz de erro."
    }
    gas = @{
        Name = "GASAuthorityGuard"
        File = "GASAuthorityGuard.md"
        Focus = "Authority GAS, commit/validation/prediction."
    }
    net = @{
        Name = "ReplicationSentinel"
        File = "ReplicationSentinel.md"
        Focus = "RPC, OnRep, ownership e seguranca de rede."
    }
    input = @{
        Name = "InputContextArchitect"
        File = "InputContextArchitect.md"
        Focus = "IMC/IA, prioridades e gates de consumo."
    }
    smoke = @{
        Name = "SmokeDSVerifier"
        File = "SmokeDSVerifier.md"
        Focus = "Smoke DS+2, reconnect e lag/loss."
    }
}

function Get-QuickPromptFromTemplate {
    param(
        [Parameter(Mandatory = $true)]
        [string]$TemplatePath
    )

    $content = Get-Content -Raw -Path $TemplatePath
    $matches = [regex]::Matches($content, '```text\s*(.*?)\s*```', 'Singleline')
    if ($matches.Count -eq 0) {
        return ""
    }

    # O bloco final corresponde ao "Uso rapido (copiar e colar)".
    return $matches[$matches.Count - 1].Groups[1].Value.Trim()
}

function Show-Agent {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("build","gas","net","input","smoke")]
        [string]$Key
    )

    $agent = $AgentMap[$Key]
    $templatePath = Join-Path $AgentsRoot $agent.File

    if (!(Test-Path $templatePath)) {
        throw "Template nao encontrado: $templatePath"
    }

    $prompt = Get-QuickPromptFromTemplate -TemplatePath $templatePath

    Write-Host ("[Agent] Tipo: {0}" -f $Key)
    Write-Host ("[Agent] Nome: {0}" -f $agent.Name)
    Write-Host ("[Agent] Foco: {0}" -f $agent.Focus)
    Write-Host ("[Agent] Template: {0}" -f $templatePath)
    Write-Host ""
    Write-Host "[Agent] Prompt sugerido:"
    Write-Host "--------------------"
    Write-Host $prompt
    Write-Host "--------------------"
    Write-Host ""

    return $prompt
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
        Write-Host "[Agent] Prompt copiado para area de transferencia."
    }
    catch {
        Write-Warning "[Agent] Nao foi possivel copiar o prompt para a area de transferencia."
    }
}

function Invoke-AgentRun {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("build","gas","net","input","smoke","all")]
        [string]$Key
    )

    if (!(Test-Path $WorkflowScript)) {
        throw "Script de workflow nao encontrado: $WorkflowScript"
    }

    switch ($Key) {
        "build" {
            & powershell -ExecutionPolicy Bypass -File $WorkflowScript -Mode build
            if ($LASTEXITCODE -ne 0) { throw "Falha ao executar build (codigo $LASTEXITCODE)." }
        }
        "smoke" {
            $smokeArgs = @("-ExecutionPolicy", "Bypass", "-File", $WorkflowScript, "-Mode", "smoke")
            if ($NoSteam.IsPresent) { $smokeArgs += "-NoSteam" }
            if ($SkipLagLoss.IsPresent) { $smokeArgs += "-SkipLagLoss" }
            & powershell @smokeArgs
            if ($LASTEXITCODE -ne 0) { throw "Falha ao executar smoke (codigo $LASTEXITCODE)." }
        }
        "all" {
            & powershell -ExecutionPolicy Bypass -File $WorkflowScript -Mode build
            if ($LASTEXITCODE -ne 0) { throw "Falha ao executar build (codigo $LASTEXITCODE)." }

            $smokeArgs = @("-ExecutionPolicy", "Bypass", "-File", $WorkflowScript, "-Mode", "smoke")
            if ($NoSteam.IsPresent) { $smokeArgs += "-NoSteam" }
            if ($SkipLagLoss.IsPresent) { $smokeArgs += "-SkipLagLoss" }
            & powershell @smokeArgs
            if ($LASTEXITCODE -ne 0) { throw "Falha ao executar smoke (codigo $LASTEXITCODE)." }
        }
        default {
            $prompt = Show-Agent -Key $Key
            if ($CopyPrompt.IsPresent) {
                Copy-PromptToClipboard -Prompt $prompt
            }
            Write-Host ("[Agent] Task '{0}' nao possui execucao automatica. Prompt pronto para enviar ao agente." -f $Key)
        }
    }
}

if ($Mode -eq "checklist") {
    if ($TaskType -eq "all") {
        foreach ($key in @("build","gas","net","input","smoke")) {
            $prompt = Show-Agent -Key $key
            if ($CopyPrompt.IsPresent -and $key -eq "smoke") {
                # Evita sobrescrever clipboard varias vezes no modo all.
                Copy-PromptToClipboard -Prompt $prompt
            }
        }
    }
    else {
        $prompt = Show-Agent -Key $TaskType
        if ($CopyPrompt.IsPresent) {
            Copy-PromptToClipboard -Prompt $prompt
        }
    }

    Write-Host ("[Agent] Checklist pronto para task '{0}'." -f $TaskType)
}
else {
    Invoke-AgentRun -Key $TaskType

    if ($TaskType -in @("build","smoke","all")) {
        Write-Host ("[Agent] Execucao automatica concluida para task '{0}'." -f $TaskType)
    }
}
