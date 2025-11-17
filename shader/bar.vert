#version 330 core

// Input: The 2D vertex position from your VBO
layout (location = 0) in vec2 aPos;

// Uniforms: Global variables you set from C++
uniform mat4 model;      // The matrix that scales/moves this *one* bar
uniform mat4 projection; // The matrix that puts it on the screen

void main()
{
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}