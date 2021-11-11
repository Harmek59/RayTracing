#version 430

uniform mat4 projectionMatrix;
uniform mat4 customViewMatrixProjection;
uniform bool useCustomViewProjectionMatrix = false;
uniform int modelDataIndex;

layout (binding = 6) uniform GLOBAL_SETTINGS{
    mat4 viewMatrix;
    mat4 inverseViewMatrix;
    mat4 lightSpaceMatrix;  //TODO zaimplementowac proste oswietlenie za pomoca tej macierz (na razie kierunek swiatla nie jest na podstawie macierzy)
};

struct Triangle{
    uint vertexIndex0;
    uint vertexIndex1;
    uint vertexIndex2;
    uint material;
    uint normalIndex0;
    uint normalIndex1;
    uint normalIndex2;
    uint offset;
};

struct Vertex{
    vec3 position;
    uint offset;
};
uniform uint numberOfTrianglesFromMesh = 0;
layout (std430, binding = 8) readonly buffer TRIANGLE
{
    Triangle triangles[];
};

layout (std430, binding = 9) readonly buffer VERTEX
{
    Vertex vertices[];
};
struct NormalsFromMesh{
    vec3 normal;
    uint offset;
};
layout (std430, binding = 7) readonly buffer NORMAL
{
    NormalsFromMesh normals[];
};

struct ModelData{
    mat4 positionMatrix;
    mat4 rotationMatrix;
    mat4 scaleMatrix;
//    bool whetherToDraw;
    vec4 whetherToDraw;
    ivec4 brginEndTriangles;
    vec4 offset;
    vec4 offset2;
};
layout (std430, binding = 13) readonly buffer MODELDATA{
    ModelData modelData[];
};

flat out int discardTriangle;
flat out vec3 normal;

void main(){
    uint triangleId = gl_VertexID/3 + modelData[modelDataIndex].brginEndTriangles.x;
    uint vertexId = int(mod(gl_VertexID, 3));


    Triangle trgl = triangles[triangleId];
    Vertex vert;
    NormalsFromMesh norm;
    if (vertexId == 0){
        vert = vertices[trgl.vertexIndex0];
        norm = normals[trgl.normalIndex0];
    } else if (vertexId == 1){
        vert = vertices[trgl.vertexIndex1];
        norm = normals[trgl.normalIndex1];
    } else {
        vert = vertices[trgl.vertexIndex2];
        norm = normals[trgl.normalIndex2];
    }

    mat4 model = modelData[modelDataIndex].positionMatrix * modelData[modelDataIndex].rotationMatrix * modelData[modelDataIndex].scaleMatrix;
    discardTriangle = int(modelData[modelDataIndex].whetherToDraw[0]);

    normal = norm.normal;

    mat4 viewProjMat;
    if(useCustomViewProjectionMatrix) viewProjMat = customViewMatrixProjection;
    else viewProjMat = projectionMatrix * viewMatrix;
    gl_Position = viewProjMat * model * vec4(vert.position, 1.0f);

}

