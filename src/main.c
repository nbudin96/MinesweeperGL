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
    bool win;
    bool lost;
    bool restart;
    float start_frame_time;
    float end_frame_time;
    float delta;
    double mouse_x, mouse_y;
} GameState;

typedef struct Grid {
    Tile **tiles;
    int rows, cols;
} Grid;

GameState global_state;
Grid game_grid;

//Temp Globals
bool up_key_released = true;
bool down_key_released = true;
Spritesheet *global_spritesheet; 

const char *program_name = "MinesweeperGL";
const unsigned int TILE_SIZE = 32;

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
        for(int i = 0; i < game_grid.cols * game_grid.rows; i++)
        {
            if(check_mouse_hover(game_grid.tiles[i], global_state.mouse_x, global_state.mouse_y))
            {
                game_grid.tiles[i]->selected = true;
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        for(int i = 0; i < game_grid.cols * game_grid.rows; i++)
        {
            if(check_mouse_hover(game_grid.tiles[i], global_state.mouse_x, global_state.mouse_y))
            {
                game_grid.tiles[i]->mouse_clicked = true;
                game_grid.tiles[i]->selected = false;
                handle_tile_click(game_grid.tiles[i]);
            }
            else
            {
                game_grid.tiles[i]->selected = false;
            }
        }
    }
}

void keyboard_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // For now let's skip keyboard stuff
    switch(key)
    {
        case GLFW_KEY_ESCAPE:
            global_state.running = false;
            break;
        default:
            break;
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
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(opengl_message_callback, 0);

    // Only need to load this once btw, all of our objects will be accessing this one texture in memory
    Spritesheet *global_spritesheet = create_spritesheet("./assets/spritesheets/minesweeper.png", 4, 3);
    game_grid.cols = 10;
    game_grid.rows = 10;
    game_grid.tiles = (Tile **)malloc(sizeof(Tile *) * (game_grid.cols * game_grid.rows));
    for(int i = 0; i < game_grid.rows * game_grid.cols; i++)
    {
        Sprite *sprite = create_sprite(global_spritesheet, 1, 0, global_state.current_window_width, global_state.current_window_height);
        game_grid.tiles[i] = create_tile(sprite);
        set_sprite_size(game_grid.tiles[i]->sprite, TILE_SIZE, TILE_SIZE);
    }
    for(int row_ind = 0; row_ind < game_grid.rows; row_ind++)
    {
        for(int col_ind = 0; col_ind < game_grid.cols; col_ind++)
        {
            Sprite *curr_sprite= game_grid.tiles[row_ind * game_grid.rows + col_ind]->sprite;
            int new_sprite_x = (int)((col_ind + 1) - ((game_grid.cols + 1) / 2)) * curr_sprite->sprite_width;
            int new_sprite_y = (int)((row_ind + 1) - ((game_grid.rows + 1) / 2)) * curr_sprite->sprite_height;
            set_sprite_position(curr_sprite, 
                    (int)global_state.current_window_width / 2 + new_sprite_x, 
                    (int)global_state.current_window_height / 2 + new_sprite_y);
        }
    }

    game_grid.tiles[0]->mine = true;

    while(global_state.running && !glfwWindowShouldClose(global_state.current_window))
    {
        global_state.start_frame_time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, global_spritesheet->texture);
        update_tiles(game_grid.tiles, &global_state, game_grid.cols * game_grid.rows);

        glfwSwapBuffers(global_state.current_window);
        glfwPollEvents();
        global_state.end_frame_time = glfwGetTime();
        global_state.delta = global_state.end_frame_time - global_state.start_frame_time;
    }

    glfwDestroyWindow(global_state.current_window);
    return 0;
}
