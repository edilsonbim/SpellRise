param(
    [int]$Tail = 200,
    [string]$Contains = "",
    [switch]$SecurityOnly,
    [switch]$PersistenceOnly
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Join-Path $PSScriptRoot ".."
$AuditFile = Join-Path $ProjectRoot "Saved\SpellRise\Audit\audit_trail.ndjson"

if (!(Test-Path $AuditFile)) {
    Write-Error "Audit trail nao encontrado: $AuditFile"
    exit 1
}

$lines = Get-Content $AuditFile -ErrorAction Stop
if ($Tail -gt 0 -and $lines.Count -gt $Tail) {
    $lines = $lines[($lines.Count - $Tail)..($lines.Count - 1)]
}

if ($SecurityOnly.IsPresent) {
    $lines = $lines | Where-Object { $_ -match '"category":"Security"' }
}
if ($PersistenceOnly.IsPresent) {
    $lines = $lines | Where-Object { $_ -match '"category":"Persistence"' }
}
if (![string]::IsNullOrWhiteSpace($Contains)) {
    $escaped = [Regex]::Escape($Contains)
    $lines = $lines | Where-Object { $_ -match $escaped }
}

if ($null -eq $lines -or $lines.Count -eq 0) {
    Write-Host "[AUDIT] Nenhuma entrada encontrada com os filtros informados."
    exit 0
}

Write-Host ("[AUDIT] arquivo={0}" -f $AuditFile)
Write-Host ("[AUDIT] entradas={0}" -f $lines.Count)
$lines | ForEach-Object { Write-Host $_ }
exit 0
