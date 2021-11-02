#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class GLSLShader {
public:
    GLSLShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
               const std::string &geometryShaderPath = "");

    GLSLShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
               const std::string &tessControlShaderPath,
               const std::string &tessEvaluationShaderPath, const std::string &geometryShaderPath = "");

    explicit GLSLShader(const std::string &computeShaderPath);

    GLSLShader(GLSLShader &&);

    GLSLShader &operator=(GLSLShader &&);

    //delete copy
    GLSLShader(const GLSLShader &) = delete;

    GLSLShader &operator=(const GLSLShader &) = delete;

    void remove();

    ~GLSLShader();

    void use() const;

    void unUse() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    std::string readShaderFromFile(const std::string &shaderPath);     //return shader code

    GLuint createAndCompileShader(const std::string &shaderCode, GLenum shaderType,
                                  const std::string &shaderName);    //return shader ID

    void createProgram();

    void attachShaderToProgram(GLuint shaderID, GLenum shaderType);    //deletes shader as well

    void linkProgram();

    GLuint programID = 0;
};