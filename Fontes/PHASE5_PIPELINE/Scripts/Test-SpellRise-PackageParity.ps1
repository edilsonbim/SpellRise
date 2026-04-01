[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectRoot,
    [Parameter(Mandatory = $true)][string]$ClientPackageDir,
    [Parameter(Mandatory = $true)][string]$ServerPackageDir,
    [Parameter(Mandatory = $true)][string]$ClientMetadataPath,
    [Parameter(Mandatory = $true)][string]$ServerMetadataPath,
    [string]$OutputLogPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\SpellRise.Pipeline.Common.ps1"

if ([string]::IsNullOrWhiteSpace($OutputLogPath)) {
    $OutputLogPath = Join-Path $ProjectRoot 'Saved\Logs\Automation\PackageParity\latest.log'
}

$reasons = New-Object System.Collections.Generic.List[string]

if (-not (Test-Path $ClientMetadataPath)) { $reasons.Add("Metadata do client ausente: $ClientMetadataPath") }
if (-not (Test-Path $ServerMetadataPath)) { $reasons.Add("Metadata do server ausente: $ServerMetadataPath") }
if (-not (Test-Path $ClientPackageDir)) { $reasons.Add("Diretório de package do client ausente: $ClientPackageDir") }
if (-not (Test-Path $ServerPackageDir)) { $reasons.Add("Diretório de package do server ausente: $ServerPackageDir") }

$clientMeta = if (Test-Path $ClientMetadataPath) { Get-Content $ClientMetadataPath -Raw | ConvertFrom-Json } else { $null }
$serverMeta = if (Test-Path $ServerMetadataPath) { Get-Content $ServerMetadataPath -Raw | ConvertFrom-Json } else { $null }

if ($null -ne $clientMeta -and $null -ne $serverMeta) {
    if ($clientMeta.Git.Commit -ne $serverMeta.Git.Commit) {
        $reasons.Add("Commit diferente entre client e server: $($clientMeta.Git.Commit) vs $($serverMeta.Git.Commit)")
    }
    if ($clientMeta.Configuration -ne $serverMeta.Configuration) {
        $reasons.Add('Configuration diferente entre client e server.')
    }
    if ($clientMeta.Platform -ne $serverMeta.Platform) {
        $reasons.Add('Platform diferente entre client e server.')
    }
    if ($clientMeta.OnlineProfile -ne $serverMeta.OnlineProfile) {
        $reasons.Add('OnlineProfile diferente entre client e server.')
    }
    if ($clientMeta.ConfigHashes.DefaultEngine -ne $serverMeta.ConfigHashes.DefaultEngine) {
        $reasons.Add('Hash de DefaultEngine.ini diferente entre client e server.')
    }
    if ($clientMeta.ConfigHashes.DefaultGame -ne $serverMeta.ConfigHashes.DefaultGame) {
        $reasons.Add('Hash de DefaultGame.ini diferente entre client e server.')
    }
}

$clientExe = Get-SpellRiseExecutableCandidate -Root $ClientPackageDir -NameLike '*Client*.exe'
$serverExe = Get-SpellRiseExecutableCandidate -Root $ServerPackageDir -NameLike '*Server*.exe'

if ([string]::IsNullOrWhiteSpace($clientExe)) {
    $reasons.Add("Executável do client não encontrado em: $ClientPackageDir")
}
if ([string]::IsNullOrWhiteSpace($serverExe)) {
    $reasons.Add("Executável do server não encontrado em: $ServerPackageDir")
}

$result = [ordered]@{
    Status = if ($reasons.Count -eq 0) { 'PASS' } else { 'FAIL' }
    ClientPackageDir = $ClientPackageDir
    ServerPackageDir = $ServerPackageDir
    ClientExecutable = $clientExe
    ServerExecutable = $serverExe
    Reasons = @($reasons)
}

$lines = @()
$lines += 'Package Parity'
$lines += "Status: $($result.Status)"
$lines += "ClientPackageDir: $ClientPackageDir"
$lines += "ServerPackageDir: $ServerPackageDir"
$lines += "ClientExecutable: $clientExe"
$lines += "ServerExecutable: $serverExe"
if ($reasons.Count -gt 0) {
    $lines += ''
    $lines += 'Reasons:'
    foreach ($reason in $reasons) {
        $lines += "- $reason"
    }
}

Write-SpellRiseTextFile -Path $OutputLogPath -Content ($lines -join [Environment]::NewLine)

return $result
