param(
    [Parameter(Mandatory=$true)]
    [ValidateSet('prod','test')]
    [string]$Profile,

    [string]$ConfigPath = 'Config/DefaultGame.ini'
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path -LiteralPath $ConfigPath)) {
    throw "Config file not found: $ConfigPath"
}

$content = Get-Content -LiteralPath $ConfigPath -Raw -Encoding UTF8
$sectionHeader = '[/Script/SpellRise.SpellRiseGameModeBase]'

$sectionPattern = '(?ms)^\[/Script/SpellRise\.SpellRiseGameModeBase\]\r?\n(?<body>.*?)(?=^\[|\z)'
$match = [regex]::Match($content, $sectionPattern)
if (-not $match.Success) {
    throw "Section not found: $sectionHeader"
}

$body = $match.Groups['body'].Value

switch ($Profile) {
    'prod' {
        $requireSteam = 'true'
        $allowFallback = 'false'
    }
    'test' {
        $requireSteam = 'false'
        $allowFallback = 'true'
    }
}

if ($body -match '(?m)^bRequireSteamAuthOnDedicatedServer=') {
    $body = [regex]::Replace($body, '(?m)^bRequireSteamAuthOnDedicatedServer=.*$', "bRequireSteamAuthOnDedicatedServer=$requireSteam")
} else {
    $body = "bRequireSteamAuthOnDedicatedServer=$requireSteam`r`n" + $body
}

if ($body -match '(?m)^bAllowDevOfflineIdFallback=') {
    $body = [regex]::Replace($body, '(?m)^bAllowDevOfflineIdFallback=.*$', "bAllowDevOfflineIdFallback=$allowFallback")
} else {
    $body = "bAllowDevOfflineIdFallback=$allowFallback`r`n" + $body
}

$newSection = "$sectionHeader`r`n$body"
$newContent = $content.Substring(0, $match.Index) + $newSection + $content.Substring($match.Index + $match.Length)
Set-Content -LiteralPath $ConfigPath -Value $newContent -Encoding UTF8

Write-Host "Applied Steam auth profile: $Profile"
Write-Host "bRequireSteamAuthOnDedicatedServer=$requireSteam"
Write-Host "bAllowDevOfflineIdFallback=$allowFallback"
