# fix-png.ps1 - Recursively strip iCCP and metadata from all PNG files
# Usage: Run in your project root folder

$ErrorActionPreference = "Stop"

# Check if optipng is available
if (!(Get-Command optipng -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: optipng not found. Please install it and add to PATH." -ForegroundColor Red
    exit 1
}

Write-Host "Searching for .png files recursively..." -ForegroundColor Cyan

# Find all .png files (case-insensitive)
$pngFiles = Get-ChildItem -Path . -Recurse -File | Where-Object {
    $_.Extension -eq '.png' -or $_.Extension -eq '.PNG'
}

if ($null -eq $pngFiles -or $pngFiles.Count -eq 0) {
    Write-Host "No .png files found in current directory and subdirectories." -ForegroundColor Yellow
    exit 0
}

Write-Host "Found $($pngFiles.Count) PNG file(s). Processing..." -ForegroundColor Green

$processed = 0
foreach ($file in $pngFiles) {
    $relativePath = Resolve-Path -Relative $file.FullName
    Write-Host "Processing: $relativePath"
    
    & optipng -strip all -o7 -quiet $file.FullName
    
    if ($LASTEXITCODE -eq 0) {
        $processed++
    } else {
        Write-Host "Warning: Failed to process $relativePath" -ForegroundColor Yellow
    }
}

Write-Host "`nDone! Processed $processed out of $($pngFiles.Count) files." -ForegroundColor Green
Write-Host "Note: Original files were overwritten. Backup first next time if needed." -ForegroundColor Gray