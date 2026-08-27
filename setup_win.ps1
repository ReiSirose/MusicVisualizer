Write-Host "Setting up MusicVisualizer dependencies for Windows..." -ForegroundColor Cyan

$ErrorActionPreference = "Stop"

$toolsPath = Join-Path $PSScriptRoot "third_party\bin"
$ytDlpPath = Join-Path $toolsPath "yt-dlp.exe"
$ffmpegPath = Join-Path $toolsPath "ffmpeg.exe"
$ffmpegArchive = Join-Path $env:TEMP "musicvisualizer-ffmpeg.zip"
$ffmpegExtractPath = Join-Path $env:TEMP "musicvisualizer-ffmpeg"

New-Item -ItemType Directory -Force -Path $toolsPath | Out-Null

Write-Host "Downloading Windows downloader tools..."
Invoke-WebRequest `
    -Uri "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe" `
    -OutFile $ytDlpPath

Invoke-WebRequest `
    -Uri "https://github.com/BtbN/FFmpeg-Builds/releases/latest/download/ffmpeg-master-latest-win64-gpl.zip" `
    -OutFile $ffmpegArchive

if (Test-Path $ffmpegExtractPath) {
    Remove-Item -Recurse -Force $ffmpegExtractPath
}
Expand-Archive -Path $ffmpegArchive -DestinationPath $ffmpegExtractPath
$ffmpegBinary = Get-ChildItem -Path $ffmpegExtractPath -Filter "ffmpeg.exe" -Recurse | Select-Object -First 1
if ($null -eq $ffmpegBinary) {
    throw "The downloaded FFmpeg archive did not contain ffmpeg.exe."
}
Copy-Item $ffmpegBinary.FullName $ffmpegPath -Force
Remove-Item $ffmpegArchive -Force
Remove-Item $ffmpegExtractPath -Recurse -Force

Write-Host "Windows downloader tools installed in $toolsPath" -ForegroundColor Green

# 1. Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "CMake not found! Please install it from https://cmake.org/download/" -ForegroundColor Red
    exit 1
}

$vcpkgPath = "$PSScriptRoot\vcpkg"

if (-not (Test-Path $vcpkgPath)) {
    Write-Host "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git $vcpkgPath

    Write-Host "Bootstrapping vcpkg..."
    Set-Location $vcpkgPath
    .\bootstrap-vcpkg.bat
}

Write-Host "Installing GLFW3 and GLM via vcpkg..."
cd $vcpkgPath
.\vcpkg install glfw3:x64-windows glm:x64-windows

Write-Host "Integrating vcpkg globally..."
.\vcpkg integrate install

Write-Host "Done! Configure CMake with this toolchain file if vcpkg is not detected automatically:" -ForegroundColor Green
Write-Host "-DCMAKE_TOOLCHAIN_FILE=$vcpkgPath/scripts/buildsystems/vcpkg.cmake" -ForegroundColor Yellow