# 🎵 C++ Music Visualizer

A real-time, synchronized audio visualizer built from scratch using C++ and OpenGL.

## 📝 Description

This project renders a frequency spectrum analysis of audio files. It uses the Fast Fourier Transform (FFT) to convert audio data into frequency magnitudes, which are then rendered as 2D bars using custom OpenGL shaders.

The core achievement of this project is to make visualization for my favorite songs

## 🛠️ Tech Stack

* **Language:** C++17
* **Graphics:** OpenGL (loaded via [GLAD](https://glad.dav1d.de/))
* **Windowing:** [GLFW](https://www.glfw.org/)
* **Math:** [GLM](https://github.com/g-truc/glm)
* **Audio:** [miniaudio](https://miniaud.io/) (Playback & Decoding)
* **Analysis:** [kissfft](https://github.com/mborgerding/kissfft) (Frequency transformation)

## 🗺️ Roadmap

The current focus is evolving the project from a static visualizer into an interactive application using **Dear ImGui**.

- [ ] **Level 4: UI Integration**
    - Integrate the ImGui library.
    - Create a basic overlay window.
    - Implement a Master Volume slider to control `miniaudio` output.

- [ ] **Level 5: Visual Customization**
    - Add UI controls to modify the visualizer in real-time.
    - **Color Picker:** Change the RGB color of the bars.
    - **Sensitivity:** Adjust the magnitude scaling factor (height of bars).
    - **Smoothing:** Adjust the decay rate of the bars.

- [ ] **Level 6: Playback Control (The Jukebox)**
    - Implement a "Load-All" playlist system.
    - **Controls:** Add Play, Pause, and Stop buttons.
    - **Seeking:** Add a progress slider to scrub through the song (utilizing the in-memory PCM buffer).
    - **Playlist:** Allow switching between different loaded audio files.


### Prerequisites
Ensure you have a C++ compiler (GCC/Clang/MSVC) and CMake installed.

### Build Instructions

```bash
# 1. Clone the repository
git clone https://github.com/ReiSirose/MusicVisualizer.git

cd MusicVisualizer

# 2. Create build directory
mkdir build
cd build

# 3. Configure and Build
cmake ..
make

# 4. Run
./MusicVisualizer