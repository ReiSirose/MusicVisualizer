#version 330 core
out vec4 FragColor;

// Uniforms: Global variables you set from C++
uniform vec3 barColor;

void main()
{
    // Set the pixel's color to the barColor, with 100% opacity
    FragColor = vec4(barColor, 1.0);
}