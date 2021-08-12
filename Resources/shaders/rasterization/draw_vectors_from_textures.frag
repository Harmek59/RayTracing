# version 430 core

out vec4 FragColor;

layout(rgba32f, binding = 2) uniform image2D img_color;

uniform vec3 color = vec3(1.0, 0.0, 0.0);

flat in ivec2 pixel_coords;

void main()
{
    FragColor = imageLoad(img_color, pixel_coords);
    //FragColor = vec4(color, 1.0);   
} 