#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"
#include "audio.h"
#include "geometry.h"
#include "shader.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int NFFT = 1024;
const int SAMPLE_RATE = 44100;
int main (){
    Audio audio ("flower_thief.mp3", SAMPLE_RATE, NFFT);
    Window window ("flower thief", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    Shader barShader("../shader/bar.vert", "../shader/bar.frag");
    float vertices[] = {
        // positions
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };
    Geometry barGeometry (vertices, sizeof(vertices));

    barShader.use();
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);
    barShader.setMat4("projection", projection);
    barShader.setVec3("barColor", 1.0f, 0.5f, 0.2f); // Set to orange


    while(!window.shouldClose()){
        window.processInput();

        audio.Update();

        window.startFrame();

        const std::vector<float>& magnitudes = audio.getFrequencyMagnitude();
    

        barGeometry.bind();
        int numBars = magnitudes.size() / 4;
        float barWidth = (float)SCREEN_WIDTH / numBars;

        for (int i = 0; i < numBars; ++i) {
            // Scale magnitude for screen
            float magnitude = log10(1.0f + magnitudes[i]) * 50.0f;
            if(magnitude < 2.0f) magnitude = 2.0f;
            if (magnitude > SCREEN_HEIGHT) magnitude = SCREEN_HEIGHT; // Clamp
            
            // Calculate bar position
            float xPos = i * barWidth;
            float yPos = 0.0f; // At the bottom

            // Create a "model" matrix for this *one* bar
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
            model = glm::scale(model, glm::vec3(barWidth - 1.0f, magnitude, 1.0f));

            barShader.setMat4("model", model);

            barGeometry.draw();
        }
        window.endFrame();
    }
    return 0;
}