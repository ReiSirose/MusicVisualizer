#ifndef WINDOW_H
#define WINDOW_H
#define GLFW_INCLUDE_NONE
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
    GLFWwindow* getWindow() const {return m_window;}
    bool isFullscreen() const { return m_isFullscreen; }
    void setFullscreen(bool enable);
    int getWidth() const;
    int getHeight() const;
private:
    GLFWwindow*  m_window;
    static int s_windowCount;
    bool m_isFullscreen = false;

    int m_savedXPos = 0, m_savedYPos = 0;
    int m_savedWidth = 1280, m_savedHeight = 720;

    static void s_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void framebuffer_size_callback(int width, int height);

};



#endif