#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coordinates;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    tex_coord = tex_coordinates;
}


