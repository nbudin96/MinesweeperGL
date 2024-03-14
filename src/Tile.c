#include "Tile.h"
#include "Sprite.h"

Tile *create_tile(Sprite *sprite)
{
    Tile *new_tile = malloc(sizeof(Tile));
    new_tile->mine = false;
    new_tile->mouse_hover = false;
    new_tile->selected = false;
    new_tile->mouse_clicked = false;
    new_tile->active = false;
    new_tile->can_color = true;
    new_tile->wants_hover = true;
    new_tile->highlight_amt = 0.3f;
    new_tile->click_amt = -0.1f;
    new_tile->sprite = sprite;
    new_tile->x_ind = 0;
    new_tile->y_ind = 0;
    return new_tile;
} 

// Checks if the mouse is hovering over the tile
bool check_mouse_hover(Tile *tile, double mouse_x, double mouse_y)
{
    float left_bounds, right_bounds, upper_bounds, lower_bounds;
    left_bounds = tile->sprite->position_x - ((float)tile->sprite->sprite_width / 2.0f);
    right_bounds = tile->sprite->position_x + ((float)tile->sprite->sprite_width) / 2.0f;
    upper_bounds = tile->sprite->position_y - ((float)tile->sprite->sprite_height / 2.0f);
    lower_bounds = tile->sprite->position_y + ((float)tile->sprite->sprite_height / 2.0f);

    if(mouse_x > left_bounds && mouse_x < right_bounds && mouse_y > upper_bounds && mouse_y < lower_bounds)
    {
        tile->mouse_hover = true;
        return true;
    }
    tile->mouse_hover = false;
    return false;
}

// Resets the tile coloring
void reset_tile_color(Tile *tile)
{
    add_coloring(tile, 0.0f);
}

// Update the tile tinting (coloring) based on the state of the tile (currently if the user clicked on it or hovered mouse)
void update_tile_coloring(Tile *tile)
{
    if(tile->mouse_hover && tile->can_color)
    {
        if(tile->selected)
        {
            add_coloring(tile, tile->click_amt);
            return;
        }
        else
        {
            add_coloring(tile, tile->highlight_amt);
            return;
        }
    }
    reset_tile_color(tile);
}

// Sends the coloring data to the GPU shader uniform to mix with the sampled texture color
void add_coloring(Tile *tile, float color)
{
    glUseProgram(tile->sprite->mesh->shader_program);
    glUniform4f(glGetUniformLocation(tile->sprite->mesh->shader_program, "hover_amt"), color, color, color, 1.0f);
}

void handle_tile_click(Tile *tile)
{
    // Set tile to empty square
    if(!tile->mine)
    {
        set_sprite_texture(tile->sprite, 0, 0);
    }
    else if (tile->mine)
    {
        set_sprite_texture(tile->sprite, 2, 0);
    }
}

// Sets the indices of the tile in it's tile grid
void set_indices(Tile *tile, int x, int y)
{
    tile->x_ind = x;
    tile->y_ind = y;
}
