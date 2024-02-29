#include "Tile.h"

Tile *create_tile(Sprite *sprite)
{
    Tile *new_tile = malloc(sizeof(Tile));
    new_tile->mouse_hover = false;
    new_tile->active = false;
    new_tile->wants_hover = true;
    new_tile->highlight_amt = 0.3f;
    new_tile->click_amt = -0.1f;
    new_tile->sprite = sprite;
    return new_tile;
} 

// TODO This does not work if the screen is resized
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

void highlight_tile(Tile *tile)
{
    add_coloring(tile, tile->highlight_amt);
}

void unhighlight_tile(Tile *tile)
{
    add_coloring(tile, 0.0f);
}

void click_tile(Tile *tile)
{
    add_coloring(tile, tile->click_amt);
    tile->active = true;
}

void unclick_tile(Tile *tile)
{
    add_coloring(tile, 0.0f);
    tile->active = false;
}

void add_coloring(Tile *tile, float color)
{
    glUseProgram(tile->sprite->mesh->shader_program);
    glUniform4f(glGetUniformLocation(tile->sprite->mesh->shader_program, "hover_amt"), color, color, color, 1.0f);
}
