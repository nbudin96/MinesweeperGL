#include "../include/headers/glad.h"
#include "../include/headers/glfw3.h"
#include "../include/headers/Sprite.h"
#include "../include/headers/Shader.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/headers/stb_image.h"

GLFWwindow *current_window;
int current_window_width = 800;
int current_window_height = 800;
bool running = true;


//Temp Globals
bool up_key_released = true;
bool down_key_released = true;
Spritesheet *global_spritesheet; 
Sprite *test_sprite;
Mesh *test_mesh;

const char *program_name = "MinesweeperGL";

void GLAPIENTRY opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
    fprintf(stderr, "\n");
}

void keyboard_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch(key)
    {
        case GLFW_KEY_ESCAPE:
            running = false;
            break;
        case GLFW_KEY_UP:
            if(action == GLFW_RELEASE)
            {
                up_key_released = true;
            }
            if(GLFW_PRESS && up_key_released)
            {
                up_key_released = false;
                mods == GLFW_MOD_SHIFT ? 
                    change_texture_coordinates(test_mesh, test_sprite->spritesheet_x_index + 1, test_sprite->spritesheet_y_index) : 
                    change_texture_coordinates(test_mesh, test_sprite->spritesheet_x_index, test_sprite->spritesheet_y_index + 1);
                break;
            }
        case GLFW_KEY_DOWN:
            if(action == GLFW_RELEASE)
            {
                down_key_released = true;
            }
            if(action == GLFW_PRESS && down_key_released)
            {
                down_key_released = false;
                mods == GLFW_MOD_SHIFT ? 
                    change_texture_coordinates(test_mesh, test_sprite->spritesheet_x_index - 1, test_sprite->spritesheet_y_index) : 
                    change_texture_coordinates(test_mesh, test_sprite->spritesheet_x_index, test_sprite->spritesheet_y_index - 1);
                break;
            }
    }
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
    current_window_width = width;
    current_window_height = height;
    glViewport(0, 0, current_window_width, current_window_height);
}

void window_close_callback(GLFWwindow *window)
{
    printf("Closing window...\n");
}

int main(int argc, char *args[])
{
    if(!glfwInit())
    {
        fprintf(stderr, "ERROR, GLFW FAILED TO INITIALIZE! EXITING...\n");
        return 1;
    }

    current_window = glfwCreateWindow(current_window_width, current_window_height, program_name, NULL, NULL);

    if(!current_window)
    {
        fprintf(stderr, "ERROR, GLFW FAILED TO CREATE A WINDOW! EXITING...\n");
        return 1;
    }

    glfwMakeContextCurrent(current_window);
    gladLoadGL();

    glfwSetKeyCallback(current_window, keyboard_input_callback);
    glfwSetWindowSizeCallback(current_window, window_size_callback);
    glfwSetWindowCloseCallback(current_window, window_close_callback);

    glViewport(0, 0, current_window_width, current_window_height);
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, 0);

    // Only need to load this once btw, all of our objects will be accessing this one texture in memory
    Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/minesweeper.png", 4, 3);
    //Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/Tiles.png", 3, 3);

    // Testing creating meshes and sprites off of one global spritesheet
    test_sprite = create_sprite(global_spritesheet, 0, 0);
    test_mesh = create_mesh(test_sprite);

    //TEXTURE LOADING
    glGenTextures(1, &global_spritesheet->texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, global_spritesheet->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_spritesheet->width, global_spritesheet->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, global_spritesheet->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(test_mesh->shader_program);
    glUniform1i(glGetUniformLocation(test_mesh->shader_program, "current_texture"), 0); // set it manually

    while(running && !glfwWindowShouldClose(current_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, global_spritesheet->texture);

        glUseProgram(test_mesh->shader_program);//here
        glBindVertexArray(test_mesh->vertex_array_object);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(current_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(current_window);
    return 0;
}
