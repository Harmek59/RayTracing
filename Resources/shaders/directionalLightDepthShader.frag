#version 330 core

flat in int discardTriangle;

void main()
{
    if(discardTriangle == 0){
        discard;
    } else{
        gl_FragDepth = gl_FragCoord.z;
    }
//     gl_FragDepth = gl_FragCoord.z;
}