#version 430

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform int treeLayer;

struct AABB{
    vec3 begin;
    int modelIndex;// if -1 we did not "hit" model BB yet
    vec3 end;
    uint isValid;
};

layout (std430, binding = 15) readonly buffer BVH_TREE{
    AABB bvhTree[];
};


out vec3 color;
out vec3 cellSize;
out int skip;

vec3 colorsTable[10] = vec3[](
vec3(1.0, 0.0, 0.0),
vec3(0.0, 1.0, 0.0),
vec3(0.0, 0.0, 1.0),
vec3(1.0, 1.0, 0.0),
vec3(1.0, 0.0, 1.0),
vec3(0.0, 1.0, 1.0),
vec3(0.5, 0.5, 0.5),
vec3(1.0, 0.5, 0.0),
vec3(1.0, 0.0, 0.5),
vec3(0.0, 1.0, 0.5)
);

void main(){


    vec3 boxSize = bvhTree[gl_VertexID].end - bvhTree[gl_VertexID].begin;
    vec3 AABBcenter = bvhTree[gl_VertexID].begin;

    if (bvhTree[gl_VertexID].isValid == -1){
        skip = 1;
    } else {
        skip = 0;
    }

    int layer = 0;
    if (gl_VertexID > 0) layer++;
    if (gl_VertexID > 2) layer++;
    if (gl_VertexID > 6) layer++;
    if (gl_VertexID > 14) layer++;
    if (gl_VertexID > 30) layer++;
    if (gl_VertexID > 62) layer++;
    if (gl_VertexID > 126) layer++;
    if (gl_VertexID > 254) layer++;

    color = colorsTable[layer % 10];

    if (treeLayer != layer && treeLayer != -1){
        skip =1;
    }
    cellSize = boxSize;
    //    cellSize = vec3(50.f);

    gl_Position = vec4(AABBcenter, 1.0);
    //    gl_Position = vec4(1.0);
}

