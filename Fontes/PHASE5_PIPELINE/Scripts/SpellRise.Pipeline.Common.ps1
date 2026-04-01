Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function New-SpellRiseRunDirectory {
    param(
        [Parameter(Mandatory = $true)][string]$ProjectRoot,
        [Parameter(Mandatory = $true)][string]$PipelineName
    )

    $timestamp = Get-Date -Format 'yyyy-MM-dd_HH-mm-ss'
    $runRoot = Join-Path $ProjectRoot "Saved\Logs\Automation\$PipelineName\$timestamp"
    New-Item -ItemType Directory -Path $runRoot -Force | Out-Null
    return $runRoot
}

function Write-SpellRiseTextFile {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Content
    )

    $directory = Split-Path -Parent $Path
    if (-not [string]::IsNullOrWhiteSpace($directory)) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }

    Set-Content -Path $Path -Value $Content -Encoding UTF8
}

function Write-SpellRiseJsonFile {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)]$Object
    )

    $directory = Split-Path -Parent $Path
    if (-not [string]::IsNullOrWhiteSpace($directory)) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }

    $json = $Object | ConvertTo-Json -Depth 10
    Set-Content -Path $Path -Value $json -Encoding UTF8
}

function Invoke-SpellRiseProcess {
    param(
        [Parameter(Mandatory = $true)][string]$FilePath,
        [Parameter(Mandatory = $true)][string[]]$Arguments,
        [Parameter(Mandatory = $true)][string]$LogPath,
        [string]$WorkingDirectory = ''
    )

    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = $FilePath
    $psi.Arguments = [string]::Join(' ', $Arguments)
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.CreateNoWindow = $true
    if (-not [string]::IsNullOrWhiteSpace($WorkingDirectory)) {
        $psi.WorkingDirectory = $WorkingDirectory
    }

    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $psi

    $null = $process.Start()
    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()

    $log = @()
    $log += "COMMAND: $FilePath $([string]::Join(' ', $Arguments))"
    $log += "EXIT_CODE: $($process.ExitCode)"
    $log += "----- STDOUT -----"
    $log += $stdout
    $log += "----- STDERR -----"
    $log += $stderr
    Set-Content -Path $LogPath -Value $log -Encoding UTF8

    return @{
        ExitCode = $process.ExitCode
        StdOut = $stdout
        StdErr = $stderr
        LogPath = $LogPath
    }
}

function Get-SpellRiseGitMetadata {
    param(
        [Parameter(Mandatory = $true)][string]$ProjectRoot
    )

    $gitDir = Join-Path $ProjectRoot '.git'
    if (-not (Test-Path $gitDir)) {
        return @{ Branch = 'unknown'; Commit = 'unknown' }
    }

    $headPath = Join-Path $gitDir 'HEAD'
    if (-not (Test-Path $headPath)) {
        return @{ Branch = 'unknown'; Commit = 'unknown' }
    }

    $head = (Get-Content $headPath -ErrorAction SilentlyContinue | Select-Object -First 1)
    if ($head -like 'ref:*') {
        $ref = $head.Substring(5).Trim()
        $refPath = Join-Path $gitDir $ref.Replace('/', '\')
        $branch = Split-Path -Leaf $ref
        $commit = if (Test-Path $refPath) { (Get-Content $refPath | Select-Object -First 1).Trim() } else { 'unknown' }
        return @{ Branch = $branch; Commit = $commit }
    }

    return @{ Branch = 'detached'; Commit = $head.Trim() }
}

function Get-SpellRiseFileHashSafe {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        return ''
    }
    return (Get-FileHash -Path $Path -Algorithm SHA256).Hash
}

function Get-SpellRiseBuildMetadata {
    param(
        [Parameter(Mandatory = $true)][string]$ProjectRoot,
        [Parameter(Mandatory = $true)][string]$TargetName,
        [Parameter(Mandatory = $true)][string]$Configuration,
        [Parameter(Mandatory = $true)][string]$Platform,
        [Parameter(Mandatory = $true)][string]$OnlineProfile
    )

    $git = Get-SpellRiseGitMetadata -ProjectRoot $ProjectRoot
    $defaultEngine = Join-Path $ProjectRoot 'Config\DefaultEngine.ini'
    $defaultGame = Join-Path $ProjectRoot 'Config\DefaultGame.ini'

    return @{
        TimestampUtc = (Get-Date).ToUniversalTime().ToString('o')
        ProjectRoot = $ProjectRoot
        TargetName = $TargetName
        Configuration = $Configuration
        Platform = $Platform
        OnlineProfile = $OnlineProfile
        Git = $git
        ConfigHashes = @{
            DefaultEngine = (Get-SpellRiseFileHashSafe -Path $defaultEngine)
            DefaultGame = (Get-SpellRiseFileHashSafe -Path $defaultGame)
        }
    }
}

function Get-SpellRiseExecutableCandidate {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$NameLike
    )

    if (-not (Test-Path $Root)) {
        return ''
    }

    $candidate = Get-ChildItem -Path $Root -Filter '*.exe' -Recurse -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -like $NameLike } |
        Select-Object -First 1

    if ($null -eq $candidate) {
        return ''
    }

    return $candidate.FullName
}
