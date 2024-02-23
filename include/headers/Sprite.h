#pragma once
#include "./stb_image.h"
#include "stdbool.h"
#include "Shader.h"

typedef struct Spritesheet {
    int height;
    int width;
    int nrchannels;
    unsigned int texture;
    unsigned char *data;
    int cols, rows;
} Spritesheet;

typedef struct Sprite {
    int sprite_height;
    int sprite_width;
    Spritesheet *spritesheet;
    int spritesheet_x_index;
    int spritesheet_y_index;
    float top_left[2], bottom_left[2], top_right[2], bottom_right[2];
} Sprite;

typedef struct Mesh 
{
    unsigned int vertex_buffer_object, vertex_elements, vertex_array_object, texture, texture_coordinate_buffer, shader_program;
    float vertices[12];
    int indices[6];
    float tex_coordinates[8];
    Sprite *sprite;
    Shader fragment_shader, vertex_shader;
} Mesh;

// Opengl Mesh stuff
Mesh *create_mesh(Sprite *sprite);
void change_texture_coordinates(Mesh *mesh, int sprite_ind_x, int sprite_ind_y);
void compile_shaders(Mesh *mesh);

// Sprites and spritesheet stuff
Spritesheet *create_spritesheet(const char *filename, int cols, int rows);
unsigned char *load_spritesheet(Spritesheet *spritesheet, const char *filename);
void free_spritesheet(Spritesheet *spritesheet);
void calculate_sprite_size(Sprite *sprite, int rows, int columns);
void calculate_texture_offsets(Sprite *sprite);
Sprite *create_sprite(Spritesheet *spritesheet, int x_index, int y_index);
void change_sprite_offsets(Sprite *sprite, int new_x_index, int new_y_index);
