#version 430 core
uniform mat4 lightSpaceMatrix;

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

void main()
{
    uint triangleId = gl_VertexID/3;
    uint vertexId = int(mod(gl_VertexID, 3));

    uint trianglesSkipped = 0;
    uint localTriangleID = 0;
    int modelID = int(beginOfModelsAndGrids);
    for (int i = int(beginOfModelsAndGrids); i<int(endOfModelsAndGrids); i++){
        uint trianglesInModel = modelData[i].brginEndTriangles.y - modelData[i].brginEndTriangles.x;
        trianglesSkipped += trianglesInModel;
        if (triangleId >= trianglesSkipped){
            modelID++;
        } else {
            localTriangleID = triangleId - trianglesSkipped + trianglesInModel;
            break;
        }
    }

    //    Triangle trgl = triangles[triangleId + modelData[beginOfModelsAndGrids].brginEndTriangles.x];
    Triangle trgl = triangles[localTriangleID + modelData[modelID].brginEndTriangles.x];
    Vertex vert;
    if (vertexId == 0){
        vert = vertices[trgl.vertexIndex0];
    } else if (vertexId == 1){
        vert = vertices[trgl.vertexIndex1];
    } else {
        vert = vertices[trgl.vertexIndex2];
    }
    mat4 model = modelData[modelID].positionMatrix * modelData[modelID].rotationMatrix * modelData[modelID].scaleMatrix;
    discardTriangle = int(modelData[modelID].whetherToDraw[0]);

    gl_Position = lightSpaceMatrix * model * vec4(vert.position, 1.0);
}