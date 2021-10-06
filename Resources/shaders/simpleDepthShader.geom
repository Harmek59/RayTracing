#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=50) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)
in int discardTriangle[];

void main()
{
    if(discardTriangle[0] == 0){
        return;
    }
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();

//        gl_Position = vec4(0.5, 0.5, -0.5, 1.0);
//        EmitVertex();
//        gl_Position = vec4(-0.5, 0.5, -0.5, 1.0);
//        EmitVertex();
//        gl_Position = vec4(0.0, -0.5, -0.5, 1.0);
//        EmitVertex();
//        EndPrimitive();


    }
}