#pragma once

#include "DisplayModeInterface.h"

class DisplayModePerformanceMeasure : public DisplayModeInterface {
public:
    DisplayModePerformanceMeasure() {
        setUp();
    }

    virtual void setUp() override {
        loadShaders();

        //////////////////////////

        glGenBuffers(1, &atomicsBuffer);
        // bind the buffer and define its initial storage capacity
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 3, NULL, GL_DYNAMIC_DRAW);
        // unbind the buffer
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
        // declare a pointer to hold the values in the buffer

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        // map the buffer, userCounters will point to the buffers data
        userCounters = (GLuint *) glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                                   0,
                                                   sizeof(GLuint) * 3,
                                                   GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT |
                                                   GL_MAP_UNSYNCHRONIZED_BIT
        );
        // set the memory to zeros, resetting the values in the buffer
        memset(userCounters, 0, sizeof(GLuint) * 3);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                         3,
                         atomicsBuffer);
        // unmap the buffer
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
        //////////////////////////
    }


    virtual void loadShaders() override {
        rayTracingMeasurePerformanceShader = std::make_unique<GLSLShader>(rayTracingMeasurePerformanceShaderPath);
    }

    virtual void draw(const Scene& scene) override {

        glClearColor(0.4f, 0.4f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ////////////
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        // again we map the buffer to userCounters, but this time for read-only access
        userCounters = (GLuint *) glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                                   0,
                                                   sizeof(GLuint) * 3,
                                                   GL_MAP_READ_BIT
        );
        // copy the values to other variables because...
        numberOfTriangleTests = userCounters[0];
        numberOfCellTests = userCounters[1];
        numberOfBoxTests = userCounters[2];
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        // map the buffer, userCounters will point to the buffers data
        userCounters = (GLuint *) glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                                   0,
                                                   sizeof(GLuint) * 3,
                                                   GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT |
                                                   GL_MAP_UNSYNCHRONIZED_BIT
        );
        // set the memory to zeros, resetting the values in the buffer
        memset(userCounters, 0, sizeof(GLuint) * 3);
        // unmap the buffer
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);


        ImGui::Text("Triangles test: %d", numberOfTriangleTests / (1280 * 720));
        ImGui::Text("Cells test: %d", numberOfCellTests / (1280 * 720));
        ImGui::Text("Box test: %d", numberOfBoxTests / (1280 * 720));
        ImGui::Text("");
        std::string buttonText = "mode " + std::to_string((MODE + 1) % 2);
        if (ImGui::Button(buttonText.c_str())) {
            MODE = (MODE + 1) % 2;
            numberOfTriangleTestsSlider = numberOfTriangleTests / (1280 * 720);
            numberOfCellTestsSlider = numberOfCellTests / (1280 * 720);
            numberOfBoxTestsSlider = numberOfBoxTests / (1280 * 720);
        }
        if (MODE == 1) {
            ImGui::SliderInt("numberOfTriangleTests", &numberOfTriangleTestsSlider, 0, 10000);
            ImGui::SliderInt("numberOfCellTests", &numberOfCellTestsSlider, 0, 10000);
            ImGui::SliderInt("numberOfBoxTests", &numberOfBoxTestsSlider, 0, 10000);

        }
        ///////

        rayTracingMeasurePerformanceShader->use();
        rayTracingMeasurePerformanceShader->setInt("numberOfTriangleTestsToDo", numberOfTriangleTestsSlider);
        rayTracingMeasurePerformanceShader->setInt("numberOfCellTestsToDo", numberOfCellTestsSlider);
        rayTracingMeasurePerformanceShader->setInt("numberOfBoxTestsToDo", numberOfBoxTestsSlider);
        rayTracingMeasurePerformanceShader->setInt("MODE", MODE);
        rayTracingMeasurePerformanceShader->setMat4("viewMatrix", glm::inverse(
                CoreEngine::getCamera().getViewMatrix()));  //we need to invert view matrix, because everything inside view matrix is inverted (position -> -positon itp.)
        rayTracingMeasurePerformanceShader->setVec3("cameraPosition", CoreEngine::getCamera().getPosition());

        glBindImageTexture(0, out_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glDispatchCompute(80, 45, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

private:

    int MODE = 0;

    uint32_t numberOfTriangleTests = 0;
    uint32_t numberOfCellTests = 0;
    uint32_t numberOfBoxTests = 0;
    GLuint *userCounters;
    GLuint atomicsBuffer;

    int numberOfTriangleTestsSlider = 0;
    int numberOfCellTestsSlider = 0;
    int numberOfBoxTestsSlider = 0;

    Texture2D out_tex{1280, 720, GL_RGBA32F, GL_RGBA, GL_FLOAT};

    std::string rayTracingMeasurePerformanceShaderPath = "../Resources/shaders/raytracer/ray_tracing_measure_performance.comp";

    std::unique_ptr<GLSLShader> rayTracingMeasurePerformanceShader;
};