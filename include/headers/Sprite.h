#pragma once
#include "./stb_image.h"
#include "stdbool.h"
#include "Shader.h"

typedef struct Mesh 
{
    unsigned int vertex_buffer_object, vertex_elements, vertex_array_object, texture, texture_coordinate_buffer, shader_program;
    float vertices[12];
    int indices[6];
    float tex_coordinates[8];
    Shader fragment_shader, vertex_shader;
} Mesh;

typedef struct Spritesheet {
    int height;
    int width;
    int nrchannels;
    unsigned int texture;
    unsigned char *data;
    int cols, rows;
} Spritesheet;

typedef struct Sprite {
    bool keep_relative_position;
    int sprite_height;
    int sprite_width;
    int spritesheet_x_index;
    int spritesheet_y_index;
    int screen_width;
    int screen_height;
    float scale_x;
    float scale_y;
    float position_x;
    float position_y;
    float normalized_position_x;
    float normalized_position_y;
    float top_left[2], bottom_left[2], top_right[2], bottom_right[2];
    Spritesheet *spritesheet;
    Mesh *mesh;
} Sprite;

// Opengl Mesh stuff
Mesh *create_mesh(Sprite *sprite);
void change_texture_coordinates(Sprite *sprite, int new_x_index, int new_y_index);
void compile_shaders(Mesh *mesh);

// Sprites and spritesheet stuff
Spritesheet *create_spritesheet(const char *filename, int cols, int rows);
unsigned char *load_spritesheet(Spritesheet *spritesheet, const char *filename);
void free_spritesheet(Spritesheet *spritesheet);
Sprite *create_sprite(Spritesheet *spritesheet, int x_index, int y_index, int screen_width, int screen_height);
void calculate_sprite_size(Sprite *sprite);
void set_sprite_size(Sprite *sprite, int pixels_x, int pixels_y);
void set_sprite_texture(Sprite *sprite, int new_x_index, int new_y_index); 
void calculate_texture_offsets(Sprite *sprite);

void scale_sprite(Sprite *sprite, float scale_x, float scale_y);
void set_sprite_position(Sprite *sprite, float new_position_x, float new_position_y);
void draw_sprite(Sprite *sprite, int screen_width, int screen_height);
