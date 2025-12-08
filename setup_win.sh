Write-Host "🎵 Setting up MusicVisualizer Dependencies for Windows..." -ForegroundColor Cyan

# 1. Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host " CMake not found! Please install it from https://cmake.org/download/" -ForegroundColor Red
    exit
}

$vcpkgPath = "$PSScriptRoot\vcpkg"

if (-not (Test-Path $vcpkgPath)) {
    Write-Host " Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git $vcpkgPath
    
    Write-Host " Bootstrapping vcpkg..."
    Set-Location $vcpkgPath
    .\bootstrap-vcpkg.bat
}

Write-Host " Installing GLFW3 via vcpkg..."
cd $vcpkgPath
.\vcpkg install glfw3:x64-windows

Write-Host " Integrating vcpkg globally..."
.\vcpkg integrate install

Write-Host " Done! When running CMake, use the following toolchain file if it doesn't pick it up automatically:" -ForegroundColor Green
Write-Host "-DCMAKE_TOOLCHAIN_FILE=$vcpkgPath/scripts/buildsystems/vcpkg.cmake" -ForegroundColor Yellow