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
    bool mouse_button_down;
    int flags_remaining;
    int number_of_mines;
    bool first_click;
    int grid_size;
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

void reset_game(GameState *gamestate)
{
    global_state.current_window_width = 1280;
    global_state.current_window_height = 720;
    global_state.running = true;
    global_state.grid_size = 10;
    global_state.number_of_mines = (int)(global_state.grid_size * global_state.grid_size * 0.2f);
    global_state.flags_remaining = global_state.number_of_mines;
    global_state.first_click = true;

}

// Updates all the tiles based on their state. Also get the current cursor position
void update_tiles(Tile** tiles, GameState *gamestate, int size)
{
    // Get the current mouse position
    glfwGetCursorPos(gamestate->current_window, &gamestate->mouse_x, &gamestate->mouse_y);

    int correct_tiles = 0;

    for(int i = 0; i < size; i++)
    {
        if(!gamestate->win && !gamestate->lost)
        {
            // Check for input or mouse hover
            check_mouse_hover(tiles[i], gamestate->mouse_x, gamestate->mouse_y);
            update_tile_coloring(tiles[i], global_state.mouse_button_down);
            if(tiles[i]->mine && tiles[i]->flagged)
            {
                correct_tiles++;
            }

            if(correct_tiles == gamestate->number_of_mines)
            {
                gamestate->win = true;
            }
        }
        else if (gamestate->win)
        {
            if(tiles[i]->flagged)
            {
                add_win_coloring(tiles[i]);
            }
            else{
                reset_tile_color(tiles[i]);
            }

        } 
        else if (gamestate->lost) 
        {
            if(tiles[i]->mine)
            {
                tiles[i]->revealed = true;
                add_loss_coloring(tiles[i]);
            }
            else{
                reset_tile_color(tiles[i]);
            }
        }

        determine_sprite(tiles[i]);

        //draw sprites
        //
        draw_sprite(tiles[i]->sprite, gamestate->current_window_width, gamestate->current_window_height);
    }
}

// Connects the surrounding tiles in an array that can be used to check for how close mines are
void connect_tiles(Tile *curr_tile, Grid *game_grid)
{
    // Checks a subsection of the tilemap surrounding the "curr_tile" (8 surrounding tiles) and stores them in its adjacent tiles list
    // This respects grid bounds, any positions that would be outside the grid are left null
    for(int row = -1; row < 2; row++)
    {
        if(curr_tile->y_ind + row < game_grid->rows && curr_tile->y_ind + row >= 0)
        {
            for(int col = -1; col < 2; col++)
            {
                int grid_adj_index = ((curr_tile->y_ind + row) * game_grid->cols) + curr_tile->x_ind + col;
                int local_adj_index = ((3 * row) + 3) + (col + 1);

                if(curr_tile->x_ind + col < game_grid->cols && curr_tile->x_ind + col >= 0 && game_grid->tiles[grid_adj_index] != curr_tile)
                {
                    curr_tile->adj[local_adj_index] = game_grid->tiles[grid_adj_index];
                }
            }
        }
    }
}

bool create_game_grid(int cols, int rows, Grid *new_grid)
{
    if(cols < 2 || rows < 2)
    {
        fprintf(stderr, "ERROR: Cannot have the columns or rows of the grid be less than 2!\n");
        return false;
    }
    new_grid->cols = cols;
    new_grid->rows = rows;
    new_grid->tiles = (Tile **)malloc(sizeof(Tile *) * (new_grid->cols * new_grid->rows));
    // Loop through grid and create sprite and tile with sprite
    for(int i = 0; i < new_grid->rows * new_grid->cols; i++)
    {
        Sprite *sprite = create_sprite(global_spritesheet, 1, 0, global_state.current_window_width, global_state.current_window_height);
        new_grid->tiles[i] = create_tile(sprite);
        set_sprite_size(new_grid->tiles[i]->sprite, TILE_SIZE, TILE_SIZE);
    }

    // Loop through grids and cols and place the tile appropriately in the grid
    for(int row_ind = 0; row_ind < new_grid->rows; row_ind++)
    {
        for(int col_ind = 0; col_ind < new_grid->cols; col_ind++)
        {
            Tile *current_tile = new_grid->tiles[row_ind * new_grid->cols + col_ind];
            current_tile->y_ind = row_ind;
            current_tile->x_ind = col_ind;
            Sprite *curr_sprite = current_tile->sprite;
            int new_sprite_x = (int)((col_ind + 1) - ((new_grid->cols + 1) / 2)) * curr_sprite->sprite_width;
            new_sprite_x -= curr_sprite->sprite_width / 2;
            int new_sprite_y = (int)((row_ind + 1) - ((new_grid->rows + 1) / 2)) * curr_sprite->sprite_height;
            new_sprite_y -= curr_sprite->sprite_height / 2;
            set_sprite_position(curr_sprite, 
                    (int)global_state.current_window_width / 2 + new_sprite_x, 
                    (int)global_state.current_window_height / 2 + new_sprite_y);
            // Attach all of the adjacent tiles to the current cols and rows tiles
            connect_tiles(current_tile, new_grid);
        }
    }
    return true;
}

bool destroy_grid(Grid *game_grid)
{
    for(int i = 0; i < game_grid->cols * game_grid->rows; i++)
    {
        destroy_tile(game_grid->tiles[i]);
        game_grid->tiles[i] = NULL;
    }
    free(game_grid->tiles);
    return true;
}

void place_mines(Grid *game_grid, Tile *clicked_tile, int number_of_mines)
{
    int rand_x, rand_y;
    bool tile_good;
    while(number_of_mines > 0)
    {
        tile_good = true;
        rand_x = rand() % game_grid->cols;
        rand_y = rand() % game_grid->rows;
        Tile *rand_tile = game_grid->tiles[rand_y * game_grid->cols + rand_x];
        if(rand_tile != clicked_tile && !rand_tile->mine)
        {
            for(int i = 0; i < 9; i++)
            {
                if(clicked_tile->adj[i] == rand_tile) 
                {
                    tile_good = false;
                }
            }
            if(tile_good)
            {
                rand_tile->mine = true;
                number_of_mines--;
            }
        }
    }
}

// calculates grid numbers and stuff based on click
void calculate_grid_change(Tile *curr_tile, Tile *checked_tiles)
{
    int mine_count = 0;
    curr_tile->mouse_clicked = true;
    for(int i = 0; i < 9; i++)
    {
        if(curr_tile->adj[i] != NULL && curr_tile->adj[i]->mine)
        {
            mine_count++;
        }
    }
    if(mine_count > 0)
    {
        curr_tile->adjacent_mine_count = mine_count;
    }
    else{
        for(int i = 0; i < 9; i++)
        {
            if(curr_tile->adj[i] != NULL)
            {
                if(!curr_tile->adj[i]->mouse_clicked && !curr_tile->adj[i]->mouse_clicked)
                {
                    calculate_grid_change(curr_tile->adj[i], NULL);
                }
            }
        }
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    switch(button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            if(action == GLFW_PRESS) global_state.mouse_button_down = true;
            else if(action == GLFW_RELEASE)
            {
                global_state.mouse_button_down = false;
                for(int i = 0; i < game_grid.cols * game_grid.rows; i++)
                {
                    if(game_grid.tiles[i]->mouse_hover && !game_grid.tiles[i]->flagged)
                    {
                        if(global_state.first_click)
                        {
                            global_state.first_click = false;
                            place_mines(&game_grid, game_grid.tiles[i], global_state.number_of_mines);
                        }
                        if(handle_tile_click(game_grid.tiles[i]))
                        {
                            global_state.lost = true;
                            return;
                        }
                        else
                        {
                            calculate_grid_change(game_grid.tiles[i], NULL);
                        }
                    }
                    else
                    {
                        game_grid.tiles[i]->selected = false;
                    }
                }
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if(action == GLFW_PRESS) global_state.mouse_button_down = true;
            if(action == GLFW_RELEASE)
            {
                global_state.mouse_button_down = false;
                for(int i = 0; i < game_grid.cols * game_grid.rows; i++)
                {
                    if(game_grid.tiles[i]->mouse_hover)
                    {
                        if(!game_grid.tiles[i]->flagged && global_state.flags_remaining - 1 >= 0 && !game_grid.tiles[i]->mouse_clicked)
                        {
                            global_state.flags_remaining--;
                            flag_tile(game_grid.tiles[i], true);
                        }
                        else if(game_grid.tiles[i]->flagged && !game_grid.tiles[i]->mouse_clicked)
                        {
                            global_state.flags_remaining++;
                            flag_tile(game_grid.tiles[i], false);
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

void keyboard_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch(key)
    {
        case GLFW_KEY_ESCAPE:
            global_state.running = false;
            break;
        case GLFW_KEY_R:
            if(action == GLFW_RELEASE)
            {
                global_state.restart = true;
            }
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

int main(int argc, char *args[])
{
    global_state.current_window_width = 1280;
    global_state.current_window_height = 720;
    global_state.running = true;
    global_state.restart = false;
    global_state.grid_size = 10;
    global_state.number_of_mines = (int)(global_state.grid_size * global_state.grid_size * 0.2f);
    global_state.flags_remaining = global_state.number_of_mines;
    global_state.first_click = true;

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
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(opengl_message_callback, 0);

    // Only need to load this once btw, all of our objects will be accessing this one texture in memory
    global_spritesheet = create_spritesheet("./assets/spritesheets/minesweeper.png", 4, 3);
    if(!create_game_grid(global_state.grid_size, global_state.grid_size, &game_grid))
    {
        glfwDestroyWindow(global_state.current_window);
        return 1;
    }

    while(global_state.running && !glfwWindowShouldClose(global_state.current_window))
    {
        if(global_state.restart == true)
        {
            global_state.restart = false;
            destroy_grid(&game_grid);
            global_state.flags_remaining = global_state.number_of_mines;
            global_state.first_click = true;
            global_state.win = false;
            global_state.lost = false;
            create_game_grid(global_state.grid_size, global_state.grid_size, &game_grid);
        }

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
    free_spritesheet(global_spritesheet);
    return 0;
}
