#include "window.h"
#include "audio.h"

int main (){
    Audio audio ("flower_thief.mp3", 44100, 1024);
    Window window ("flower thief", 1280, 720);

    while(!window.shouldClose()){
        window.startFrame();
        window.endFrame();
    }
}