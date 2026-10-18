<#
.SYNOPSIS
    Removes solid background from logos, icons, and UI assets with zero halos.

.DESCRIPTION
    Wrapper for remove_background.py.

.PARAMETER ImagePath
    Path or filename of the image (e.g. "Assets/Logo/logo 3.png" or just "logo 3.png").

.PARAMETER Crop
    Also save an optically balanced cropped version for UI usage.

.PARAMETER Embed
    Embed the resulting logo into Source/UI/FloweretLogoData.h.

.EXAMPLE
    .\remove_background.ps1 "Assets\Logo\logo 3.png" -Crop -Embed
#>

param (
    [Parameter(Position=0, Mandatory=$false)]
    [string]$ImagePath,

    [switch]$Crop,
    [switch]$Embed,
    [string]$BgColor
)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$pyScript = Join-Path $scriptDir "remove_background.py"

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Host "[!] Error: 'python' was not found in PATH." -ForegroundColor Red
    exit 1
}

$pyArgs = @($pyScript)

if ($ImagePath) {
    $pyArgs += $ImagePath
    if ($Crop) { $pyArgs += "--crop" }
    if ($Embed) { $pyArgs += "--embed" }
    if ($BgColor) { $pyArgs += @("--bg", $BgColor) }
}

python @pyArgs
