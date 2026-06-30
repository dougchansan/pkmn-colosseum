# Download and extract Felk's Dolphin scripting fork (scripting-preview4) into bin\.
# bin\ is gitignored; rerun this script to reinstall.
$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$binDir = Join-Path $root "bin"
$url = "https://github.com/Felk/dolphin/releases/download/scripting-preview4/dolphin-scripting-preview4-x64.7z"
$archive = Join-Path $root "dolphin-scripting-preview4-x64.7z"
$sevenZip = "C:\Program Files\7-Zip\7z.exe"

if (Test-Path (Join-Path $binDir "Dolphin.exe")) {
    Write-Host "Already installed at $binDir (delete bin\ to force reinstall)"
    exit 0
}

if (-not (Test-Path $sevenZip)) { throw "7-Zip not found at $sevenZip" }

Write-Host "Downloading $url ..."
Invoke-WebRequest -Uri $url -OutFile $archive

Write-Host "Extracting ..."
New-Item -ItemType Directory -Force $binDir | Out-Null
& $sevenZip x $archive "-o$binDir" -y | Out-Null
if ($LASTEXITCODE -ne 0) { throw "7z extraction failed ($LASTEXITCODE)" }
Remove-Item $archive

# The archive may contain a top-level folder; flatten so bin\Dolphin.exe exists.
if (-not (Test-Path (Join-Path $binDir "Dolphin.exe"))) {
    $inner = Get-ChildItem $binDir -Directory | Where-Object { Test-Path (Join-Path $_.FullName "Dolphin.exe") } | Select-Object -First 1
    if ($inner) {
        Get-ChildItem $inner.FullName | Move-Item -Destination $binDir
        Remove-Item $inner.FullName
    }
}

if (Test-Path (Join-Path $binDir "Dolphin.exe")) {
    Write-Host "Installed: $(Join-Path $binDir 'Dolphin.exe')"
} else {
    throw "Extraction finished but bin\Dolphin.exe not found - inspect $binDir"
}
