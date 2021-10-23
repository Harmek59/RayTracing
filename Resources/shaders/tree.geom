#version 430

layout (points) in;
layout (Line_strip, max_vertices = 16) out;


in vec3 cellSize[];
in vec3 color[];
in int skip[];

out vec3 col;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

    if (skip[0] == 0){
        vec4 position = gl_in[0].gl_Position;
        vec4 cSize = vec4(cellSize[0], 0.0);

        col = color[0];

        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(0.0, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = projectionMatrix * viewMatrix * (position + vec4(cellSize[0].x, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();

        EndPrimitive();
    }
}
