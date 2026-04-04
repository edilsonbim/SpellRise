param(
    [string]$PersistenceRoot = "",
    [int]$TargetSchemaVersion = 6,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($PersistenceRoot)) {
    $ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
    $PersistenceRoot = Join-Path $ProjectRoot "Saved\SpellRise\Persistence"
}

$CharacterDir = Join-Path $PersistenceRoot "Characters"
if (!(Test-Path $CharacterDir)) {
    Write-Error "[SaveMigration] Diretorio de personagens nao encontrado: $CharacterDir"
    exit 1
}

$Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$Files = Get-ChildItem -Path $CharacterDir -Filter *.json -File

$Scanned = 0
$Migrated = 0
$Skipped = 0
$Failed = 0

foreach ($File in $Files) {
    $Scanned++

    try {
        $Raw = Get-Content -Raw -Path $File.FullName
        $Json = $Raw | ConvertFrom-Json
    }
    catch {
        Write-Warning ("[SaveMigration] Falha ao parsear JSON: {0}" -f $File.FullName)
        $Failed++
        continue
    }

    $Snapshot = $null
    if ($Json.PSObject.Properties.Name -contains "snapshot") {
        $Snapshot = $Json.snapshot
    }
    else {
        $Snapshot = $Json
    }

    if ($null -eq $Snapshot) {
        Write-Warning ("[SaveMigration] Snapshot invalido: {0}" -f $File.FullName)
        $Failed++
        continue
    }

    $CurrentSchema = 0
    if ($Snapshot.PSObject.Properties.Name -contains "SchemaVersion") {
        $CurrentSchema = [int]$Snapshot.SchemaVersion
    }

    if ($CurrentSchema -ge $TargetSchemaVersion) {
        $Skipped++
        continue
    }

    $Snapshot.SchemaVersion = $TargetSchemaVersion

    if ($Snapshot.PSObject.Properties.Name -contains "ArchetypeValue") {
        $ClampedArchetype = [Math]::Min([Math]::Max([int]$Snapshot.ArchetypeValue, 0), 5)
        $Snapshot.ArchetypeValue = $ClampedArchetype
    }

    if ($DryRun.IsPresent) {
        Write-Host ("[SaveMigration][DryRun] {0}: Schema {1} -> {2}" -f $File.Name, $CurrentSchema, $TargetSchemaVersion)
        $Migrated++
        continue
    }

    $BackupPath = "{0}.bak.{1}" -f $File.FullName, $Timestamp
    Copy-Item -Path $File.FullName -Destination $BackupPath -Force

    $OutputJson = $Json | ConvertTo-Json -Depth 100
    Set-Content -Path $File.FullName -Value $OutputJson -Encoding UTF8

    Write-Host ("[SaveMigration] {0}: Schema {1} -> {2} (backup: {3})" -f $File.Name, $CurrentSchema, $TargetSchemaVersion, (Split-Path -Leaf $BackupPath))
    $Migrated++
}

Write-Host ""
Write-Host ("[SaveMigration] Resultado: Scanned={0} Migrated={1} Skipped={2} Failed={3}" -f $Scanned, $Migrated, $Skipped, $Failed)

if ($Failed -gt 0) {
    exit 2
}

exit 0
