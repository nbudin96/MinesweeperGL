#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D current_texture;

void main()
{
    //FragColor = vec4(tex_coord, 0, 1);
    FragColor = texture(current_texture, tex_coord);
}
