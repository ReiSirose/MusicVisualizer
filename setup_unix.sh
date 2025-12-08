#!/bin/bash

echo "Set up Music Visualizer dependencies ..."

if [["$OSTYPE" == "darwin"*]]; then
    echo "Detected MacOS"
    if ! command -v brew &> /dev/null; then
    echo " Homebrew not found. Please install it first: https://brew.sh/"
    exit 1
    fi
    echo "📦 Installing CMake and GLFW via Homebrew..."
    brew update
    brew install cmake glfw
elif [[ -f /etc/debian_version ]]; then
    echo " Detected Debian/Ubuntu Linux"
    
    echo " Installing Build Tools, CMake, GLFW, and OpenGL..."
    sudo apt-get update
    sudo apt-get install -y build-essential cmake libglfw3-dev libgl1-mesa-dev xorg-dev
elif [[ -f /etc/arch-release ]]; then
    # --- Arch Linux ---
    echo " Detected Arch Linux"
    
    echo "  Installing CMake and GLFW..."
    sudo pacman -S --needed cmake glfw-x11 base-devel

elif [[ -f /etc/fedora-release ]]; then
    # --- Fedora ---
    echo " Detected Fedora"
    
    echo " Installing CMake and GLFW..."
    sudo dnf install cmake glfw-devel

else
    echo " OS not supported by this script."
    echo "Please manually install: CMake, GLFW3, and OpenGL development headers."
    exit 1
fi

echo "✅ Dependencies installed! You can now run cmake."