project(MusicVisualizer LANGUAGES CXX C)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 CONFIG REQUIRED) 
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(kissfft_lib STATIC
    third_party/kiss_fft/kiss_fft.c
    third_party/kiss_fft/kiss_fftr.c
)

target_include_directories(kissfft_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/kiss_fft
)

add_executable(visualizer
    src/main.cpp
    src/shader.cpp
    src/window.cpp
    src/audio.cpp
    src/geometry.cpp
    third_party/glad/src/glad.c
)

target_include_directories(visualizer PRIVATE 
    # Your project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib 
    
    # Third-party includes
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
)

target_link_libraries(visualizer PRIVATE 
    # Your custom library
    kissfft_lib
    OpenGL::GL  
    glfw
    glm::glm       
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
