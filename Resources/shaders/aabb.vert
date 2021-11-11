#version 430

uniform mat4 projectionMatrix;
uniform int modelDataIndex;

layout (binding = 6) uniform GLOBAL_SETTINGS{
    mat4 viewMatrix;
};

struct GridData{
    vec3 gridBegin;
    uint cellsBeginIndex;
    vec3 gridEnd;
    uint cellsEndIndex;
    ivec3 gridResolution;
    int offset1;
    vec3 cellSize;
    int offset2;
};
layout (std430, binding = 12) readonly buffer GRIDSDATA
{
    GridData gridData[];
};


struct ModelData{
    mat4 positionMatrix;
    mat4 rotationMatrix;
    mat4 scaleMatrix;
//    bool whetherToDraw;
    vec4 whetherToDraw;
    ivec4 brginEndTriangles;
    ivec4 brginEndGrids;
    vec4 offset2;
};
layout (std430, binding = 13) readonly buffer MODELDATA{
    ModelData modelData[];
};



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

int modI(int v1, int v2) {
    float a = float(v1);
    float b = float(v2);
    float m=a-floor((a+0.5)/b)*b;
    return int(floor(m+0.5));
}

out vec3 color;
out vec3 cellSize;
out int skip;
out mat4 modelViewProjectionMatrix;

void main(){

    uint gridIndex = gl_VertexID + modelData[modelDataIndex].brginEndGrids.x;

    color = colorsTable[modI(gl_VertexID, 10)];   //TODO zrob modI w tree.vert

    mat4 modelMatrix = modelData[modelDataIndex].positionMatrix * modelData[modelDataIndex].rotationMatrix * modelData[modelDataIndex].scaleMatrix;

    cellSize =  (gridData[gridIndex].gridEnd - gridData[gridIndex].gridBegin);

    if (modelData[modelDataIndex].whetherToDraw[0] == 0){   //TODO to chyba trzeba poprawic, bo rysuje gdy whetherToDraw jest rowne 0
        skip = 1;
    } else {
        skip = 0;
    }
    skip = 0;

    modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    gl_Position = vec4(gridData[gridIndex].gridBegin, 1.0);

//    gl_Position = vec4(1.0);
//    cellSize = vec3(3.0);
}

