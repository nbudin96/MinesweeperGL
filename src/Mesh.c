#include "Mesh.h"
#include "Spritesheet.h"

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

void change_texture_coordinates(Mesh *mesh, int sprite_ind_x, int sprite_ind_y)
{
    //TEXTURE COORDINATES WORKING WITH SPRITESHEETS
    float tex_coordinates[8] = {
        mesh->sprite->bottom_left[0], mesh->sprite->bottom_left[1],
        mesh->sprite->bottom_right[0], mesh->sprite->bottom_right[1],
        mesh->sprite->top_right[0], mesh->sprite->top_right[1],
        mesh->sprite->top_left[0], mesh->sprite->top_left[1]
    };

    memcpy(mesh->tex_coordinates, tex_coordinates, sizeof(tex_coordinates));
    change_sprite_offsets(mesh->sprite, sprite_ind_x, sprite_ind_y);
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
