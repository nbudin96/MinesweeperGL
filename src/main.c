#include "../include/headers/glad.h"
#include "../include/headers/glfw3.h"
#include "../include/headers/Sprite.h"
#include "../include/headers/Shader.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/headers/stb_image.h"


typedef struct GameState {
    GLFWwindow *current_window;
    int current_window_width;
    int current_window_height;
    bool running;
    float start_frame_time;
    float end_frame_time;
    float delta;
} GameState;

GameState global_state;

//Temp Globals
bool up_key_released = true;
bool down_key_released = true;
Spritesheet *global_spritesheet; 
Sprite *test_sprite;

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
            global_state.running = false;
            break;
        case GLFW_KEY_UP:
            if(action == GLFW_PRESS)
            {
                set_sprite_position(test_sprite, test_sprite->position_x, test_sprite->position_y - 10.0f);
                break;
            }
        case GLFW_KEY_DOWN:
            if(action == GLFW_PRESS)
            {
                set_sprite_position(test_sprite, test_sprite->position_x, test_sprite->position_y + 10.0f);
                break;
            }
        case GLFW_KEY_LEFT:
            if(action == GLFW_PRESS)
            {
                set_sprite_position(test_sprite, test_sprite->position_x + -10.0f, test_sprite->position_y);
                break;
            }
        case GLFW_KEY_RIGHT:
            if(action == GLFW_PRESS)
            {
                set_sprite_position(test_sprite, test_sprite->position_x + 10.0f, test_sprite->position_y);
                break;
            }
    }
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
    global_state.current_window_width = width;
    global_state.current_window_height = height;
    glViewport(0, 0, global_state.current_window_width, global_state.current_window_height);
}

void window_close_callback(GLFWwindow *window)
{
    printf("Closing window...\n");
}

int main(int argc, char *args[])
{
    global_state.current_window_width = 800;
    global_state.current_window_height = 800;
    global_state.running = true;

    if(!glfwInit())
    {
        fprintf(stderr, "ERROR, GLFW FAILED TO INITIALIZE! EXITING...\n");
        return 1;
    }

    global_state.current_window = glfwCreateWindow(global_state.current_window_width, global_state.current_window_height, program_name, NULL, NULL);

    if(!global_state.current_window)
    {
        fprintf(stderr, "ERROR, GLFW FAILED TO CREATE A WINDOW! EXITING...\n");
        return 1;
    }

    glfwMakeContextCurrent(global_state.current_window);
    gladLoadGL();

    glfwSetKeyCallback(global_state.current_window, keyboard_input_callback);
    glfwSetWindowSizeCallback(global_state.current_window, window_size_callback);
    glfwSetWindowCloseCallback(global_state.current_window, window_close_callback);

    glViewport(0, 0, global_state.current_window_width, global_state.current_window_height);
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, 0);

    // Only need to load this once btw, all of our objects will be accessing this one texture in memory
    Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/minesweeper.png", 4, 3);
    //Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/Tiles.png", 3, 3);

    // Testing creating meshes and sprites off of one global spritesheet
    test_sprite = create_sprite(global_spritesheet, 0, 0, global_state.current_window_width, global_state.current_window_height);

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

    glUseProgram(test_sprite->mesh->shader_program);
    glUniform1i(glGetUniformLocation(test_sprite->mesh->shader_program, "current_texture"), 0); // set it manually

    scale_sprite(test_sprite, 0.25f, 0.25f);
    set_sprite_position(test_sprite, (float)global_state.current_window_width / 2.0f, (float)global_state.current_window_height / 2.0f);
    //set_sprite_position(test_sprite, 0.0f, 0.0f);
    //set_sprite_position(test_sprite, (float)global_state.current_window_width, (float)global_state.current_window_height);

    while(global_state.running && !glfwWindowShouldClose(global_state.current_window))
    {
        global_state.start_frame_time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, global_spritesheet->texture);
        draw_sprite(test_sprite, global_state.current_window_width, global_state.current_window_height);

        glfwSwapBuffers(global_state.current_window);
        glfwPollEvents();
        global_state.end_frame_time = glfwGetTime();
        global_state.delta = global_state.end_frame_time - global_state.start_frame_time;
    }

    glfwDestroyWindow(global_state.current_window);
    return 0;
}
