#pragma once

class FrameCounter{
public:

    float update(float currentFrameTime){
        frameCounter++;
        trueEveryOneSecond = false;
        if(currentFrameTime - lastFrameForFrameCounter >= 1.0f){
            lastFrameForFrameCounter = currentFrameTime;
            FPS = frameCounter;
            frameCounter = 0;
            trueEveryOneSecond = true;
        }
        float dt = currentFrameTime - prevFrameTime;
        prevFrameTime = currentFrameTime;
        return dt;
    }

    uint32_t getFPS(){
        return FPS;
    }

    bool trueEveryOneSecond = false;
private:
    float prevFrameTime = 0.0f;
    float lastFrameForFrameCounter = 0.0f;
    uint32_t frameCounter = 0;
    uint32_t FPS = 0; 
};