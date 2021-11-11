#pragma once

#include "DisplayModes/DisplayModeInterface.h"
#include "Core.h"

class DisplayModePointShadowCubeMap : public DisplayModeInterface {
public:
    DisplayModePointShadowCubeMap() {
        setUp();
    }
    virtual void reloadShaders() override{
        loadShaders();
    }


    virtual void draw(const Scene& scene) override {
        drawCubeMapShader->use();

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        glm::mat4 view = glm::mat4(glm::mat3(Core::getCamera().getViewMatrix())); // remove translation from the view matrix
        drawCubeMapShader->setMat4("view", view);
        drawCubeMapShader->setMat4("projection", Core::getCamera().getProjectionMatrix());
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

private:

    virtual void setUp() override {
        loadShaders();

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }


    virtual void loadShaders() override {
        drawCubeMapShader = std::make_unique<GLSLShader>(drawCubeMapShaderPath_vert, drawCubeMapShaderPath_frag);
    }

    std::string drawCubeMapShaderPath_vert = "../Resources/shaders/draw_cube_map.vert";
    std::string drawCubeMapShaderPath_frag = "../Resources/shaders/draw_cube_map.frag";

    std::unique_ptr<GLSLShader> drawCubeMapShader;

    unsigned int skyboxVAO, skyboxVBO; //TODO DELETE

    static constexpr float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
};