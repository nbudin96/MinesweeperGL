#include "Sprite.h"

// 
// Opengl Mesh Stuff
//

Mesh *create_mesh(Sprite *sprite)
{
    Mesh *new_mesh = malloc(sizeof(Mesh));

    float initial_vertices[12] =
    {
        -0.5f, -0.5f, 0.0f, // Lower Left
        0.5f, -0.5f, 0.0f, // Lower Right
        0.5f, 0.5f, 0.0f, // Upper Right
        -0.5f, 0.5f, 0.0f // Upper Left
    };

    int initial_indices[6] = {
        0, 1, 2,
        0, 2, 3,
    };

    //TEXTURE COORDINATES WORKING WITH SPRITESHEETS
    float tex_coordinates[8] = {
        sprite->bottom_left[0], sprite->bottom_left[1],
        sprite->bottom_right[0],sprite->bottom_right[1],
        sprite->top_right[0], sprite->top_right[1],
        sprite->top_left[0], sprite->top_left[1]
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

void destroy_mesh(Mesh *mesh)
{
    free(mesh);
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
    if(!load_spritesheet(spritesheet, filename))
    {
        fprintf(stderr, "ERROR: could not create spritesheet!\n");
        free(spritesheet);
        return NULL;
    }

    //TEXTURE LOADING
    glGenTextures(1, &spritesheet->texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spritesheet->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, spritesheet->width, spritesheet->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spritesheet->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return spritesheet;
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

// Creates a sprite from a spritesheet
Sprite *create_sprite(Spritesheet *spritesheet, int x_index, int y_index, int screen_width, int screen_height)
{
    Sprite *new_sprite = malloc(sizeof(Sprite));
    new_sprite->keep_relative_position = true;
    new_sprite->spritesheet = spritesheet;
    new_sprite->spritesheet_x_index = x_index;
    new_sprite->spritesheet_y_index = y_index;
    new_sprite->screen_width = screen_width;
    new_sprite->screen_height = screen_height;
    set_sprite_position(new_sprite, 0, 0);
    new_sprite->scale_x = 1.0f;
    new_sprite->scale_y = 1.0f;
    calculate_sprite_size(new_sprite);
    calculate_texture_offsets(new_sprite);

    // Create and attach the mesh for the quad
    Mesh *new_mesh = create_mesh(new_sprite);
    new_sprite->mesh = new_mesh;

    // Set the spritesheet you are using
    glUseProgram(new_sprite->mesh->shader_program);
    glUniform1i(glGetUniformLocation(new_sprite->mesh->shader_program, "current_texture"), 0); // set it manually
                                                                                               //
    return new_sprite;
}

void destroy_sprite(Sprite *sprite)
{
    destroy_mesh(sprite->mesh);
    free(sprite);
}

// Calculates indv. size of a sprite located in the spritesheet and stores in Sprite struct
void calculate_sprite_size(Sprite *sprite)
{
    // Since we are using a distance of 1 between each vertex, you don't need this below
    //float x_distance = sprite->mesh->vertices[3] - sprite->mesh->vertices[0];
    //float y_distance = sprite->mesh->vertices[7] - sprite->mesh->vertices[4];
    //float x_distance_scaled = x_distance * sprite->scale_x;
    //float y_distance_scaled =  y_distance * sprite->scale_y;
    //float x_distance_scaled = sprite->scale_x;
    //float y_distance_scaled = sprite->scale_y;

    //convert to screen size
    sprite->sprite_width = (sprite->scale_x * (sprite->screen_width / 2.0f));
    sprite->sprite_height = (sprite->scale_y * (sprite->screen_height / 2.0f));
}

// This should manipulate the scale factor so that sprite size is set to passed pixel sizes
void set_sprite_size(Sprite *sprite, int pixels_x, int pixels_y)
{
    // See above "calculate_sprite_size" comments for reasons this calc is so simple
    sprite->sprite_width = pixels_x;
    sprite->sprite_height = pixels_y;

    sprite->scale_x = (float)sprite->sprite_width / ((float)sprite->screen_width / 2.0f);
    sprite->scale_y = (float)sprite->sprite_height / ((float)sprite->screen_height / 2.0f);
}

// Sets the sprite's texture based on the spritesheet x and y index
// Lower left of spritesheet is origin (0,0)
void set_sprite_texture(Sprite *sprite, int new_x_index, int new_y_index)
{
    int max_cols = sprite->spritesheet->cols;
    int max_rows = sprite->spritesheet->rows;
    if(new_x_index < 0 || new_x_index >= max_cols)
    {
        fprintf(stderr, "ERROR: Sprite index x out of range!\n");
        return;
    }
    if(new_y_index < 0 || new_y_index >= max_rows)
    {
        fprintf(stderr, "ERROR: Sprite index y out of range!\n");
        return;
    }

    sprite->spritesheet_x_index = new_x_index;
    sprite->spritesheet_y_index = new_y_index;
    calculate_texture_offsets(sprite);

    //TEXTURE COORDINATES WORKING WITH SPRITESHEETS
    //This is creating tex_coords to send to the mesh
    float tex_coordinates[8] = {
        sprite->bottom_left[0], sprite->bottom_left[1],
        sprite->bottom_right[0], sprite->bottom_right[1],
        sprite->top_right[0], sprite->top_right[1],
        sprite->top_left[0], sprite->top_left[1]
    };

    // Copy new texture data into mesh->buffer and then send the new buffer data to the GPU
    memcpy(sprite->mesh->tex_coordinates, tex_coordinates, sizeof(tex_coordinates));
    glBindBuffer(GL_ARRAY_BUFFER, sprite->mesh->texture_coordinate_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), (void*)sprite->mesh->tex_coordinates); 
}

// Calculates the pixel offset (x and y) of the sprite's location in the spritesheet and stores in the Sprite's struct
// This essentially picks a sprite out of the spritesheet
// TODO : Add spritesheet sprite size to spritesheet struct (Replace all 16's below with this)
void calculate_texture_offsets(Sprite *sprite)
{
    sprite->bottom_left[0] = (float)(sprite->spritesheet_x_index * 16);
    sprite->bottom_left[0] = sprite->bottom_left[0] / (float)sprite->spritesheet->width;
    sprite->bottom_left[1] = (float)(sprite->spritesheet_y_index * 16);
    sprite->bottom_left[1] = sprite->bottom_left[1] / (float)sprite->spritesheet->height;

    sprite->bottom_right[0] = (float)((sprite->spritesheet_x_index * 16) + 16);
    sprite->bottom_right[0] = sprite->bottom_right[0] / (float)sprite->spritesheet->width;
    sprite->bottom_right[1] = (float)(sprite->spritesheet_y_index * 16);
    sprite->bottom_right[1] = sprite->bottom_right[1] / (float)sprite->spritesheet->height;

    sprite->top_left[0] = (float)((sprite->spritesheet_x_index * 16));
    sprite->top_left[0] = sprite->top_left[0] / (float)sprite->spritesheet->width;
    sprite->top_left[1] = (float)((sprite->spritesheet_y_index * 16) + 16);
    sprite->top_left[1] = sprite->top_left[1] / (float)sprite->spritesheet->height;

    sprite->top_right[0] = (float)((sprite->spritesheet_x_index * 16) + 16);
    sprite->top_right[0] = sprite->top_right[0] / (float)sprite->spritesheet->width;
    sprite->top_right[1] = (float)((sprite->spritesheet_y_index * 16) + 16);
    sprite->top_right[1] = sprite->top_right[1] / (float)sprite->spritesheet->height;
}

// Scales the sprite by the passed amount
// Updates sprite's size
void set_sprite_scale(Sprite *sprite, float scale_x, float scale_y)
{
    sprite->scale_x = scale_x;
    sprite->scale_y = scale_y;
    sprite->sprite_width = sprite->sprite_width * scale_x;
    sprite->sprite_height = sprite->sprite_height * scale_y;
    calculate_sprite_size(sprite);
}

// Sets the sprite's position in screen coordinates
// This function converts screen coords to normalized device coordinates and stores those in the sprite struct as well
void set_sprite_position(Sprite *sprite, float new_position_x, float new_position_y)
{
    sprite->position_x = new_position_x;
    sprite->position_y = new_position_y;

    // Convert to normalized device coordinates
    sprite->normalized_position_x = (new_position_x - (sprite->screen_width / 2.0f)) / (sprite->screen_width / 2.0f);
    sprite->normalized_position_y = -(new_position_y - (sprite->screen_height / 2.0f)) / (sprite->screen_height / 2.0f);
}

// Returns the sprite's position calculated from normalized device coordinates
// This is dependent on the Sprite's screen width and height fields being accurate!
void get_sprite_position(Sprite *sprite, float *position_x, float *position_y)
{
    // Convert from normalized device coordinates to regular screen coords
    *position_x = (int)(sprite->normalized_position_x * ((float)sprite->screen_width / 2) + (float)sprite->screen_width / 2);
    *position_y = (int)(-sprite->normalized_position_y * ((float)sprite->screen_height / 2) + (float)sprite->screen_height / 2);
}

// Draws the sprite. Updates uniforms: sprite_position, scale so that the sprite's postiion and scale are used in shader
void draw_sprite(Sprite *sprite, int screen_width, int screen_height) {
    // Update the screen size in case it was resized and the sprite size needs to be recalculated
    sprite->screen_width = screen_width;
    sprite->screen_height = screen_height;
    if(!sprite->keep_relative_position)
    {
        set_sprite_position(sprite, sprite->position_x, sprite->position_y);
    }
    set_sprite_scale(sprite, sprite->scale_x, sprite->scale_y);
    get_sprite_position(sprite, &sprite->position_x, &sprite->position_y);

    //update uniforms
    glUniform2f(glGetUniformLocation(sprite->mesh->shader_program, "scale"), sprite->scale_x, sprite->scale_y);
    glUniform2f(glGetUniformLocation(sprite->mesh->shader_program, "sprite_position"), sprite->normalized_position_x, sprite->normalized_position_y);

    glUseProgram(sprite->mesh->shader_program);//here
    glBindVertexArray(sprite->mesh->vertex_array_object);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
