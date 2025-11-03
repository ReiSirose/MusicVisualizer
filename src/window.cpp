#include <glad/glad.h> 
#include "window.h"
#include <iostream>

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
        std::cerr} << "Failed to initialize GLAD" << std::endl;
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


Window::~Window(){
    glfwDestroyWindow(m_window);
    s_windowCount--;
    if(s_windowCount == 0){
        glfwTerminate();
    }
}