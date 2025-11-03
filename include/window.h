#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>

class Window {
public:

    Window(const char* title, unsigned int width, unsigned int height);
    ~Window();
    void processInput();
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void startFrame();
    void endFrame();
    bool  shouldClose();

private:
    GLFWwindow*  m_window;
    static int s_windowCount;
    static void s_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void framebuffer_size_callback(int width, int height);
};



#endif