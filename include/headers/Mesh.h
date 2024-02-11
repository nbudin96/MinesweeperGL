#pragma once
#include "Spritesheet.h"
#include "Shader.h"
#include "glad.h"
#include "glfw3.h"

typedef struct Mesh 
{
    unsigned int vertex_buffer_object, vertex_elements, vertex_array_object, texture, texture_coordinate_buffer, shader_program;
    float vertices[12];
    int indices[6];
    float tex_coordinates[8];
    Sprite *sprite;
    Shader fragment_shader, vertex_shader;
} Mesh;

Mesh *create_mesh(Sprite *sprite);
void change_texture_coordinates(Mesh *mesh, int sprite_ind_x, int sprite_ind_y);
void compile_shaders(Mesh *mesh);
