param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('prod','test')]
    [string]$Profile,

    [string]$ConfigPath = ""
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = Join-Path $PSScriptRoot '..\Config\DefaultGame.ini'
}

$resolvedConfigPath = (Resolve-Path $ConfigPath).Path
$lines = Get-Content -Path $resolvedConfigPath

$sectionHeader = '[/Script/SpellRise.SpellRiseGameModeBase]'
$sectionStart = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i].Trim() -eq $sectionHeader) {
        $sectionStart = $i
        break
    }
}

if ($sectionStart -lt 0) {
    throw "Secao nao encontrada: $sectionHeader"
}

$sectionEnd = $lines.Count - 1
for ($i = $sectionStart + 1; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match '^\s*\[') {
        $sectionEnd = $i - 1
        break
    }
}

$requireSteam = if ($Profile -eq 'prod') { 'true' } else { 'false' }
$allowFallback = if ($Profile -eq 'prod') { 'false' } else { 'true' }

$updated = @()
$updated += $lines[0..$sectionStart]

$body = @()
if ($sectionEnd -ge ($sectionStart + 1)) {
    $body = $lines[($sectionStart + 1)..$sectionEnd]
}

$foundRequire = $false
$foundFallback = $false

for ($i = 0; $i -lt $body.Count; $i++) {
    $entry = $body[$i]
    if ($entry -match '^\s*bRequireSteamAuthOnDedicatedServer\s*=') {
        $body[$i] = "bRequireSteamAuthOnDedicatedServer=$requireSteam"
        $foundRequire = $true
        continue
    }

    if ($entry -match '^\s*bAllowDevOfflineIdFallback\s*=') {
        $body[$i] = "bAllowDevOfflineIdFallback=$allowFallback"
        $foundFallback = $true
        continue
    }
}

if (-not $foundRequire) {
    $body += "bRequireSteamAuthOnDedicatedServer=$requireSteam"
}

if (-not $foundFallback) {
    $body += "bAllowDevOfflineIdFallback=$allowFallback"
}

$updated += $body

if ($sectionEnd -lt ($lines.Count - 1)) {
    $updated += $lines[($sectionEnd + 1)..($lines.Count - 1)]
}

Set-Content -Path $resolvedConfigPath -Value $updated -Encoding UTF8
Write-Host "[SteamAuthProfile] Profile=$Profile"
Write-Host "[SteamAuthProfile] bRequireSteamAuthOnDedicatedServer=$requireSteam"
Write-Host "[SteamAuthProfile] bAllowDevOfflineIdFallback=$allowFallback"
