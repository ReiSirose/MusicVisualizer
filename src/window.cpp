#include <glad/glad.h> 
#include "window.h"
#include <iostream>

int Window::s_windowCount;

Window::Window(const char* title, unsigned int width, unsigned int height){
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    s_windowCount++;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (m_window == NULL) {
        --s_windowCount;
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }


    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, s_framebuffer_size_callback);
}


void Window::startFrame(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::endFrame(){
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Window::s_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->framebuffer_size_callback(width, height);
    }
}

void Window::framebuffer_size_callback( int width, int height){
    glViewport(0, 0, width, height);
}

bool  Window:: shouldClose(){
    return glfwWindowShouldClose(m_window);
}

void Window::processInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

void Window::setFullscreen(bool enable) {
    if (enable == m_isFullscreen) return; // No change needed

    if (enable) {
        // 1. Save current state so we can restore it later
        glfwGetWindowPos(m_window, &m_savedXPos, &m_savedYPos);
        glfwGetWindowSize(m_window, &m_savedWidth, &m_savedHeight);

        // 2. Get the primary monitor and its video mode (resolution)
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // 3. Switch! (Passing 'monitor' makes it fullscreen)
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        
    } else {
        // --- SWITCH TO WINDOWED ---
        
        // Restore to old position and size (Passing 'nullptr' makes it windowed)
        glfwSetWindowMonitor(m_window, nullptr, m_savedXPos, m_savedYPos, m_savedWidth, m_savedHeight, 0);
    }

    m_isFullscreen = enable;
    
    // Force VSync on again (sometimes switching modes resets it)
    glfwSwapInterval(1);
}

int Window::getWidth() const {
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return w;
}

int Window::getHeight() const {
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return h;
}

Window::~Window(){
    glfwDestroyWindow(m_window);
    s_windowCount--;
    if(s_windowCount == 0){
        glfwTerminate();
    }
}