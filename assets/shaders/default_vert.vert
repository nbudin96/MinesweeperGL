#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coordinates;


uniform vec2 scale;
uniform vec2 sprite_position;

out vec2 tex_coord;

void main()
{
    //gl_Position = vec4((position.x + sprite_position.x) * scale.x, (position.y + sprite_position.y) * scale.y, position.z, 1.0);
    gl_Position = vec4((position.x * scale.x) + sprite_position.x, (position.y * scale.y) + sprite_position.y, position.z, 1.0);
    tex_coord = tex_coordinates;
}


