#include "../include/headers/glad.h"
#include "../include/headers/glfw3.h"
#include "../include/headers/Sprite.h"
#include "../include/headers/Tile.h"
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
    double mouse_x, mouse_y;
} GameState;

GameState global_state;

//Temp Globals
bool up_key_released = true;
bool down_key_released = true;
Spritesheet *global_spritesheet; 
Sprite *test_sprite;
Tile **tiles;

const char *program_name = "MinesweeperGL";

void GLAPIENTRY opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
    fprintf(stderr, "\n");
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        for(int i = 0; i < 2; i++)
        {
            if(check_mouse_hover(tiles[i], global_state.mouse_x, global_state.mouse_y))
            {
                tiles[i]->mouse_clicked = true;
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        for(int i = 0; i < 2; i++)
        {
            tiles[i]->mouse_clicked = false;
        }
    }
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

void update_tiles(Tile** tiles, GameState *gamestate, int size)
{
    // Get the current mouse position
    glfwGetCursorPos(gamestate->current_window, &gamestate->mouse_x, &gamestate->mouse_y);

    for(int i = 0; i < size; i++)
    {
        // Check for input or mouse hover
        check_mouse_hover(tiles[i], gamestate->mouse_x, gamestate->mouse_y);
        update_tile_coloring(tiles[i]);

        //draw sprites
        draw_sprite(tiles[i]->sprite, gamestate->current_window_width, gamestate->current_window_height);
    }
}

int main(int argc, char *args[])
{
    global_state.current_window_width = 1280;
    global_state.current_window_height = 720;
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
    glfwSetMouseButtonCallback(global_state.current_window, mouse_button_callback);

    glViewport(0, 0, global_state.current_window_width, global_state.current_window_height);
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, 0);

    // Only need to load this once btw, all of our objects will be accessing this one texture in memory
    Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/minesweeper.png", 4, 3);

    // Testing creating meshes and sprites off of one global spritesheet
    test_sprite = create_sprite(global_spritesheet, 3, 0, global_state.current_window_width, global_state.current_window_height);
    Sprite *test_sprite_2 = create_sprite(global_spritesheet, 2, 0, global_state.current_window_width, global_state.current_window_height);

    set_sprite_position(test_sprite, (float)global_state.current_window_width / 2.0f, (float)global_state.current_window_height / 2.0f);
    set_sprite_size(test_sprite, 32, 32);
    set_sprite_position(test_sprite_2, (float)global_state.current_window_width / 2.0f + 32, (float)global_state.current_window_height / 2.0f + 32);
    set_sprite_size(test_sprite_2, 32, 32);

    tiles = malloc(sizeof(Tile *) * 2);
    tiles[0] = create_tile(test_sprite);
    tiles[1] = create_tile(test_sprite_2);

    while(global_state.running && !glfwWindowShouldClose(global_state.current_window))
    {
        global_state.start_frame_time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, global_spritesheet->texture);
        update_tiles(tiles, &global_state, 2);

        glfwSwapBuffers(global_state.current_window);
        glfwPollEvents();
        global_state.end_frame_time = glfwGetTime();
        global_state.delta = global_state.end_frame_time - global_state.start_frame_time;
    }

    glfwDestroyWindow(global_state.current_window);
    return 0;
}
