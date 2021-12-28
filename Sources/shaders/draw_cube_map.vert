#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (binding = 6) uniform GLOBAL_SETTINGS{
    mat4 viewMatrix;
    mat4 inverseViewMatrix;
    mat4 projectionMatrix;
};
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projectionMatrix * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}