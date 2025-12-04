project(MusicVisualizer LANGUAGES CXX C)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 CONFIG REQUIRED) 
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

# statically link kissfft lib
add_library(kissfft_lib STATIC
    third_party/kiss_fft/kiss_fft.c
    third_party/kiss_fft/kiss_fftr.c
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
    src/geometry.cpp
    third_party/glad/src/glad.c
    ${IMGUI_SOURCES}
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

# link static lib into the main executable
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
