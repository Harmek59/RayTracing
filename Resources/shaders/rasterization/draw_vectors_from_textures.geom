#version 430

layout (points) in;
//layout (line_strip, max_vertices = 2) out; // 5 FPS XD
layout (points, max_vertices = 2) out;

layout(rgba32f, binding = 0) uniform image2D img_position;
layout(rgba16f, binding = 1) uniform image2D img_direction;

in ivec2 imgCoord[];

flat out ivec2 pixel_coords;

uniform mat4 MVPmatrix;
uniform float vectorLength = 10.0;

void main() {   

    vec3 position = imageLoad(img_position, imgCoord[0]).xyz;
    vec3 direction = imageLoad(img_direction, imgCoord[0]).xyz;

    vec4 posWorldSpace = MVPmatrix * vec4(position, 1.0);
    posWorldSpace = MVPmatrix * vec4(position + direction * (vectorLength/10.), 1.0);
    gl_Position = posWorldSpace; 
    EmitVertex();

    EndPrimitive();

    posWorldSpace = MVPmatrix * vec4(position + direction * vectorLength, 1.0);
    gl_Position = posWorldSpace; 
    EmitVertex();

    EndPrimitive();

    pixel_coords = imgCoord[0];
} 
