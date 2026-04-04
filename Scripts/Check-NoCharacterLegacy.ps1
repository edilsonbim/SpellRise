param(
    [string]$ProjectRoot = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($ProjectRoot)) {
    $ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}

$ScanRoot = Join-Path $ProjectRoot "Source\SpellRise"
if (!(Test-Path $ScanRoot)) {
    Write-Error "[PawnGate] Source/SpellRise nao encontrado: $ScanRoot"
    exit 1
}

$rg = Get-Command rg -ErrorAction SilentlyContinue
if (-not $rg) {
    Write-Error "[PawnGate] ripgrep (rg) nao encontrado no PATH."
    exit 1
}

$pattern = "ASpellRiseCharacterBase|SpellRiseCharacterBase|\bACharacter\b"
$args = @(
    "-n",
    "--glob", "!**/*.generated.h",
    "--glob", "!**/*.gen.cpp",
    $pattern,
    $ScanRoot
)

$matches = & rg @args
$exitCode = $LASTEXITCODE

if ($exitCode -eq 1) {
    Write-Host "[PawnGate] PASS: nenhum uso legado de Character encontrado em Source/SpellRise."
    exit 0
}

if ($exitCode -ne 0) {
    Write-Error "[PawnGate] Falha ao executar rg (codigo $exitCode)."
    exit $exitCode
}

Write-Host "[PawnGate] FAIL: referencias legadas detectadas:"
$matches | ForEach-Object { Write-Host $_ }
exit 2
