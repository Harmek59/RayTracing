#version 430 core

void main()
{
    const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.0, 1.0),
                            vec4(1.0, -1.0, 0.0, 1.0),
                            vec4(-1.0, 1.0, 0.0, 1.0),
                            vec4(1.0, 1.0, 0.0, 1.0));
    gl_Position = vec4(vertices[gl_VertexID]);
}