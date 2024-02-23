#include "Sprite.h"

// 
// Opengl Mesh Stuff
//

Mesh *create_mesh(Sprite *sprite)
{
    Mesh *new_mesh = malloc(sizeof(Mesh));
    new_mesh->sprite = sprite;

    float initial_vertices[12] =
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    int initial_indices[6] = {
        0, 1, 2,
        0, 2, 3,
    };

    //TEXTURE COORDINATES WORKING WITH SPRITESHEETS
    float tex_coordinates[8] = {
        new_mesh->sprite->bottom_left[0], new_mesh->sprite->bottom_left[1],
        new_mesh->sprite->bottom_right[0], new_mesh->sprite->bottom_right[1],
        new_mesh->sprite->top_right[0], new_mesh->sprite->top_right[1],
        new_mesh->sprite->top_left[0], new_mesh->sprite->top_left[1]
    };

    memcpy(new_mesh->vertices, initial_vertices, sizeof(initial_vertices));
    memcpy(new_mesh->indices, initial_indices, sizeof(initial_indices));
    memcpy(new_mesh->tex_coordinates, tex_coordinates, sizeof(tex_coordinates));

    glGenBuffers(1, &new_mesh->vertex_buffer_object);
    glGenBuffers(1, &new_mesh->texture_coordinate_buffer);
    glGenBuffers(1, &new_mesh->vertex_elements);
    glGenVertexArrays(1, &new_mesh->vertex_array_object);

    glBindVertexArray(new_mesh->vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, new_mesh->vertex_buffer_object); // address?
    glBufferData(GL_ARRAY_BUFFER, sizeof(new_mesh->vertices), new_mesh->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh->vertex_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(new_mesh->indices), new_mesh->indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, new_mesh->texture_coordinate_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(new_mesh->tex_coordinates), new_mesh->tex_coordinates, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    compile_shaders(new_mesh);

    return new_mesh;
}

void change_texture_coordinates(Mesh *mesh, int new_x_index, int new_y_index)
{
    int max_cols = mesh->sprite->spritesheet->cols;
    int max_rows = mesh->sprite->spritesheet->rows;
    if(new_x_index < 0 || new_x_index > max_cols)
    {
        fprintf(stderr, "ERROR: Sprite index x out of range!\n");
        return;
    }
    if(new_y_index < 0 || new_y_index > max_rows)
    {
        fprintf(stderr, "ERROR: Sprite index y out of range!\n");
        return;
    }

    change_sprite_offsets(mesh->sprite, new_x_index, new_y_index);

    //TEXTURE COORDINATES WORKING WITH SPRITESHEETS
    float tex_coordinates[8] = {
        mesh->sprite->bottom_left[0], mesh->sprite->bottom_left[1],
        mesh->sprite->bottom_right[0], mesh->sprite->bottom_right[1],
        mesh->sprite->top_right[0], mesh->sprite->top_right[1],
        mesh->sprite->top_left[0], mesh->sprite->top_left[1]
    };

    // Copy new texture data into mesh->buffer and then send the new buffer data to the GPU
    memcpy(mesh->tex_coordinates, tex_coordinates, sizeof(tex_coordinates));
    glBindBuffer(GL_ARRAY_BUFFER, mesh->texture_coordinate_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), (void*)mesh->tex_coordinates); 
}

void compile_shaders(Mesh *mesh)
{
    //SHADERS
    mesh->vertex_shader.filename = "./assets/shaders/default_vert.vert";
    mesh->vertex_shader.shader_type = GL_VERTEX_SHADER;
    read_shader_file(&mesh->vertex_shader, mesh->vertex_shader.filename);
    compile_shader(&mesh->vertex_shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(mesh->vertex_shader.ID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(mesh->vertex_shader.ID, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    mesh->fragment_shader.filename = "./assets/shaders/default_frag.frag";
    mesh->fragment_shader.shader_type = GL_FRAGMENT_SHADER;
    read_shader_file(&mesh->fragment_shader, mesh->fragment_shader.filename);
    compile_shader(&mesh->fragment_shader);

    glGetShaderiv(mesh->fragment_shader.ID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(mesh->fragment_shader.ID, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    mesh->shader_program = glCreateProgram();
    glAttachShader(mesh->shader_program, mesh->vertex_shader.ID);
    glAttachShader(mesh->shader_program, mesh->fragment_shader.ID);
    glLinkProgram(mesh->shader_program);
}

//
// Sprites stuff
//

// Creates a spritesheet struct
Spritesheet *create_spritesheet(const char *filename, int cols, int rows)
{
    Spritesheet *spritesheet = calloc(1, sizeof(Spritesheet));
    spritesheet->cols = cols;
    spritesheet->rows = rows;
    if(load_spritesheet(spritesheet, filename))
    {
        return spritesheet;
    }
    fprintf(stderr, "ERROR: could not create spritesheet!\n");
    free(spritesheet);
    return NULL;
}

// Loads an image into memory of the spritesheet
unsigned char *load_spritesheet(Spritesheet *spritesheet, const char *filename)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &spritesheet->width, &spritesheet->height, &spritesheet->nrchannels, 0);
    if(data)
    {
        spritesheet->data = data;
        return data;
    }
    else{
        fprintf(stderr, "ERROR, could not load image : \"%s\"!\n", filename);
        return NULL;
    }
}

// Frees spritesheet data and then spritesheet
void free_spritesheet(Spritesheet *spritesheet)
{
    stbi_image_free(spritesheet->data);
    free(spritesheet);
}

// Calculates indv. size of the sprite located in the spritesheet and stores in Sprite struct
void calculate_sprite_size(Sprite *sprite, int cols, int rows)
{
    sprite->sprite_width = sprite->spritesheet->width / cols;
    sprite->sprite_height = sprite->spritesheet->height / rows;
}

// Calculates the pixel offset (x and y) of the sprite's location in the spritesheet and stores in the Sprite's struct
void calculate_texture_offsets(Sprite *sprite)
{
    printf("Sprite Width: %d\n", sprite->sprite_width);
    printf("Sprite Height: %d\n", sprite->sprite_height);
    printf("New X index : %d, New Y index : %d\n\n", sprite->spritesheet_x_index, sprite->spritesheet_y_index);
    sprite->bottom_left[0] = (float)(sprite->spritesheet_x_index * sprite->sprite_width);
    sprite->bottom_left[0] = sprite->bottom_left[0] / (float)sprite->spritesheet->width;
    sprite->bottom_left[1] = (float)(sprite->spritesheet_y_index * sprite->sprite_height);
    sprite->bottom_left[1] = sprite->bottom_left[1] / (float)sprite->spritesheet->height;
    printf("Sprite_bottom_left x : %f\n", sprite->bottom_left[0]);
    printf("Sprite_bottom_left y : %f\n", sprite->bottom_left[1]);

    sprite->bottom_right[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width) + sprite->sprite_width);
    sprite->bottom_right[0] = sprite->bottom_right[0] / (float)sprite->spritesheet->width;
    sprite->bottom_right[1] = (float)(sprite->spritesheet_y_index * sprite->sprite_height);
    sprite->bottom_right[1] = sprite->bottom_right[1] / (float)sprite->spritesheet->height;
    printf("Sprite_bottom_right x : %f\n", sprite->bottom_right[0]);
    printf("Sprite_bottom_right y : %f\n", sprite->bottom_right[1]);

    sprite->top_left[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width));
    sprite->top_left[0] = sprite->top_left[0] / (float)sprite->spritesheet->width;
    sprite->top_left[1] = (float)((sprite->spritesheet_y_index * sprite->sprite_height) + sprite->sprite_height);
    sprite->top_left[1] = sprite->top_left[1] / (float)sprite->spritesheet->height;
    printf("Sprite_top_left x : %f\n", sprite->top_left[0]);
    printf("Sprite_top_left y : %f\n", sprite->top_left[1]);

    sprite->top_right[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width) + sprite->sprite_width);
    sprite->top_right[0] = sprite->top_right[0] / (float)sprite->spritesheet->width;
    sprite->top_right[1] = (float)((sprite->spritesheet_y_index * sprite->sprite_height) + sprite->sprite_height);
    sprite->top_right[1] = sprite->top_right[1] / (float)sprite->spritesheet->height;
    printf("Sprite_top_right x : %f\n", sprite->top_right[0]);
    printf("Sprite_top_right y : %f\n\n", sprite->top_right[1]);
}

// Creates a sprite from a spritesheet
Sprite *create_sprite(Spritesheet *spritesheet, int x_index, int y_index)
{
    Sprite *new_sprite = malloc(sizeof(Sprite));
    new_sprite->spritesheet = spritesheet;
    calculate_sprite_size(new_sprite, new_sprite->spritesheet->cols, new_sprite->spritesheet->rows);
    new_sprite->spritesheet_x_index = x_index;
    new_sprite->spritesheet_y_index = y_index;
    calculate_texture_offsets(new_sprite);
    return new_sprite;
}

void change_sprite_offsets(Sprite *sprite, int new_x_index, int new_y_index)
{
    sprite->spritesheet_x_index = new_x_index;
    sprite->spritesheet_y_index = new_y_index;
    calculate_texture_offsets(sprite);
}
