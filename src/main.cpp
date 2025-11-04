#include "window.h"
#include "audio.h"
#include "geometry.h"
int main (){
    Audio audio ("flower_thief.mp3", 44100, 1024);
    Window window ("flower thief", 1280, 720);
    float vertices [] = {1.0f, 0.3f, 0.7f};
    Geometry barGeometry (vertices, 3);

    while(!window.shouldClose()){
        window.startFrame();
        window.endFrame();
    }
}