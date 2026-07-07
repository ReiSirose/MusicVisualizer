cmake_minimum_required(VERSION 3.10)
project(MusicVisualizer LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 CONFIG REQUIRED) 
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE PROJECT_HEADERS "include/*.h" "include/*.hpp")
file(GLOB_RECURSE IMGUI_HEADERS "third_party/imgui/*.h")
file(GLOB_RECURSE KISSFFT_HEADERS "third_party/kiss_fft/*.h")

# (Optional but recommended) Group them in Xcode folders
source_group("Headers/Project" FILES ${PROJECT_HEADERS})
source_group("Headers/ImGui" FILES ${IMGUI_HEADERS})
source_group("Headers/KissFFT" FILES ${KISSFFT_HEADERS})

# statically link kissfft lib
add_library(kissfft_lib STATIC
    third_party/kiss_fft/kiss_fft.c
    third_party/kiss_fft/kiss_fftr.c
    ${KISSFFT_HEADERS}
)
target_include_directories(kissfft_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/kiss_fft
)

# set all translation unit for imgui core and background into a variable
set(IMGUI_SOURCES 
    third_party/imgui/core/imgui_demo.cpp
    third_party/imgui/core/imgui_draw.cpp
    third_party/imgui/core/imgui_tables.cpp
    third_party/imgui/core/imgui_widgets.cpp
    third_party/imgui/core/imgui.cpp
    third_party/imgui/backend/imgui_impl_glfw.cpp
    third_party/imgui/backend/imgui_impl_opengl3.cpp
)

# add translation unit into the executable
add_executable(visualizer
    src/main.cpp
    src/shader.cpp
    src/window.cpp
    src/audio.cpp
    src/song.cpp
    src/geometry.cpp
    third_party/glad/src/glad.c
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
    ${PROJECT_HEADERS}
)

# include directory into the executable
target_include_directories(visualizer PRIVATE 
    # Your project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib 
    
    # Third-party includes
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/include
)

target_compile_options(visualizer PRIVATE
    # If the build type is Debug, use no optimization and include debug symbols
    $<$<CONFIG:Debug>:-O0 -g3>
    
    # If the build type is Release, use aggressive optimization and enable Fast Math
    $<$<CONFIG:Release>:-O3 -ffast-math>
    
    # If you want a specific "Size" optimized release mode
    $<$<CONFIG:MinSizeRel>:-Os>
)

# link static lib into the main executable
target_link_libraries(visualizer PRIVATE 
    # Your custom library
    kissfft_lib
    OpenGL::GL  
    glfw
    glm::glm
)

add_executable(downloader src/downloader.cpp)

set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/third_party/bin")

if(WIN32)
    set(YT_DLP_PATH "${TOOLS_DIR}/yt-dlp.exe")
    set(FFMPEG_PATH "${TOOLS_DIR}/ffmpeg.exe")
else()
    set(YT_DLP_PATH "${TOOLS_DIR}/yt-dlp")
    set(FFMPEG_PATH "${TOOLS_DIR}/ffmpeg")
endif()

# Pass paths to C++ as preprocessor definitions
add_definitions(-DYT_DLP_PATH="${YT_DLP_PATH}")
add_definitions(-DFFMPEG_PATH="${FFMPEG_PATH}")

target_compile_definitions(downloader PRIVATE 
    YT_DLP_PATH="${YT_DLP_PATH}"
    FFMPEG_PATH="${FFMPEG_PATH}"
)
if(APPLE)
    target_link_libraries(visualizer PRIVATE 
        m 
        pthread 
        "-framework CoreFoundation" 
        "-framework CoreAudio"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(LINUX)
    target_link_libraries(visualizer PRIVATE m pthread dlfcn)
elseif(WIN32)
    target_link_libraries(visualizer PRIVATE) 
endif()

set_target_properties(visualizer PROPERTIES 
    CXX_EXTENSIONS OFF
)
