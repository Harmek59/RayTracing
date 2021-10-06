#version 430

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 cameraPosition;

uniform int imageWidth = 1280;
uniform int imageHeight = 720;


layout (binding = 5) uniform SCENE_DATA{
    int recursionDepth;
    int numberOfLights;
    uint beginOfModelsAndGrids;
    uint endOfModelsAndGrids;
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
    uint textureCoordsIndex1;
    uint textureCoordsIndex2;
    uint textureCoordsIndex3;
    uint offset2;
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

out int discardTriangle;

out vec3 FragPos;
flat out vec3 Normal;
flat out uint materialID;

void main(){
    uint triangleId = gl_VertexID/3;
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
        norm = normals[trgl.normalIndex0];
    }
    int modelID = int(beginOfModelsAndGrids);
    for (int i = int(beginOfModelsAndGrids); i<int(endOfModelsAndGrids); i++){
        if (triangleId >= modelData[i].brginEndTriangles.y){
            modelID++;
        } else {
            break;
        }
    }
    mat4 model = modelData[modelID].positionMatrix * modelData[modelID].rotationMatrix * modelData[modelID].scaleMatrix;
    discardTriangle = int(modelData[modelID].whetherToDraw[0]);

    FragPos = vert.position;
    Normal = norm.normal;
    materialID = trgl.material;

    gl_Position = projectionMatrix * viewMatrix * model * vec4(vert.position, 1.0f);

}

