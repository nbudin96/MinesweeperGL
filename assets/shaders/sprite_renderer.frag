#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D current_texture;

void main()
{
    //FragColor = vec4(0.5f, 0.5f, 0.6f, 1.0f);
    FragColor = texture(current_texture, tex_coord);
}
