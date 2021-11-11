#version 430

uniform mat4 projectionMatrix;
uniform int modelDataIndex;
uniform int gridDataIndex;

layout (binding = 6) uniform GLOBAL_SETTINGS{
    mat4 viewMatrix;
};

struct Cell {
    uint beginOfTrianglesIndiciesArray;
    uint endOfTrianglesIndiciesArray;
};
layout (std430, binding = 11) readonly buffer CELLS
{
    Cell cells[];
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
    vec4 offset;
    vec4 offset2;
};
layout (std430, binding = 13) readonly buffer MODELDATA{
    ModelData modelData[];
};


ivec3 cellsIndexTo3D(uint idx, ivec3 gridResolution) {
    int z = int(float(idx) / float(gridResolution.x * gridResolution.y));
    idx = idx - int(z * gridResolution.x * gridResolution.y);
    int y = int(float(idx) / float(gridResolution.x));
    int x = int(mod(float(idx), float(gridResolution.x)));
    return ivec3(x, y, z);
}
out vec3 color;

out vec3 cellSize;

out mat4 modelViewProjectionMatrix;

void main(){
    mat4 modelMatrix = modelData[modelDataIndex].positionMatrix * modelData[modelDataIndex].rotationMatrix * modelData[modelDataIndex].scaleMatrix;
    modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    GridData grid = gridData[gridDataIndex];

    cellSize = grid.cellSize;

    uint localCellIndex = gl_VertexID;
    uint globalCellIndex = localCellIndex + grid.cellsBeginIndex;

    vec3 cellPosition = vec3(cellsIndexTo3D(localCellIndex, grid.gridResolution)) * grid.cellSize + grid.gridBegin;

    if (cells[globalCellIndex].beginOfTrianglesIndiciesArray == cells[globalCellIndex].endOfTrianglesIndiciesArray){
        color = vec3(1.0, 0.0, 0.0);
    } else {
        color = vec3(0.0, 1.0, 0.0);
    }

    gl_Position = vec4(cellPosition, 1.0);
}

