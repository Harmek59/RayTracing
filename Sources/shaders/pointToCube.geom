#version 430

layout (points) in;
layout (Line_strip, max_vertices = 16) out;


in vec3 cellSize[];
in vec3 color[];
in int skip[];
in mat4 modelViewProjectionMatrix[];

out vec3 col;

void main() {

    if (skip[0] == 0){
        vec4 position = gl_in[0].gl_Position;

        col = color[0];

        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(0.0, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, 0.0, cellSize[0].z, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, 0.0, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, cellSize[0].y, 0.0, 0.0));
        EmitVertex();
        gl_Position = modelViewProjectionMatrix[0] * (position + vec4(cellSize[0].x, cellSize[0].y, cellSize[0].z, 0.0));
        EmitVertex();

        EndPrimitive();
    }
}
