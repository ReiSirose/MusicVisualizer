#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "window.h"
#include "audio.h"
#include "geometry.h"
#include "shader.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int NFFT = 1024;
const int SAMPLE_RATE = 44100;
int main (){
    Audio audio ("../song/flower_thief.mp3", SAMPLE_RATE, NFFT);
    Window window ("flower thief", SCREEN_WIDTH, SCREEN_HEIGHT);
    Shader barShader("../shader/bar.vert", "../shader/bar.frag");

    float vertices[] = {
        // bottom triangle
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        // top triangle
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    Geometry barGeometry (vertices, sizeof(vertices));

    barShader.use();
    barShader.setVec3("barColor", 1.0f, 0.5f, 0.2f);

    float barColor[3] = {1.0f, 0.5f, 0.2f};
    float sensitivity = 200.0f;
    float smoothFactor = 0.250f;
    bool isFullscreen = false;
    std::vector<float> displayMagnitudes(NFFT / 2 + 1, 0.0f);
    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    float masterVolume = 1.0f;

    while(!window.shouldClose()){

        window.processInput();
        audio.Update();
        window.startFrame();

        int currentWidth = window.getWidth();
        int currentHeight = window.getHeight();
        glm::mat4 projection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight, -1.0f, 1.0f);
        barShader.use();
        barShader.setMat4("projection", projection);

        const std::vector<float>& magnitudes = audio.getFrequencyMagnitude();
        barShader.use();
        barShader.setVec3("barColor", barColor[0], barColor[1], barColor[2]);
        barGeometry.bind();

        int numBars = magnitudes.size() / 4;
        float barWidth = (float)currentWidth / numBars;

        for (int i = 0; i < numBars; ++i) {
            // Scale magnitude for screen
            float magnitude = log10(1.0f + magnitudes[i]) * sensitivity;
            
            displayMagnitudes[i] += (magnitude - displayMagnitudes[i]) * smoothFactor;

            float finalHeight = displayMagnitudes[i];
            if (finalHeight > SCREEN_HEIGHT) finalHeight = SCREEN_HEIGHT;
            if (finalHeight < 2.0f) finalHeight = 2.0f;
            
            // Calculate bar position
            float xPos = i * barWidth;
            float yPos = 0.0f;

            // Create a "model" matrix for this *one* bar
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
            model = glm::scale(model, glm::vec3(barWidth - 2.0f, finalHeight, 1.0f));

            barShader.setMat4("model", model);
            barGeometry.draw();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Visualizer Setting");

        ImGui::Text("Now playing: flower thief");
        ImGui::ColorEdit3("Bar Color", barColor);
        ImGui::SliderFloat("Height Scale", &sensitivity, 10.0f, 250.0f);
        ImGui::SliderFloat("Smoothing", &smoothFactor, 0.01f, 1.0f);

        if(ImGui::SliderFloat("Volume", &masterVolume, 0.0f, 1.0f)){
            audio.setVolume(masterVolume);
        }
        if (ImGui::Checkbox("Fullscreen", &isFullscreen)) {
            window.setFullscreen(isFullscreen);
        }
        ImGui::Text("Application frame rate %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.endFrame();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}