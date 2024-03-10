#pragma once
#include "stdbool.h"
#include "Sprite.h"
#include "Shader.h"

typedef struct Tile {
    bool mine;
    bool mouse_hover;
    bool selected;
    bool mouse_clicked;
    bool active;
    bool can_color;
    bool wants_hover;
    float highlight_amt;
    float click_amt;
    Sprite *sprite;
} Tile;

Tile *create_tile(Sprite *sprite);
bool check_mouse_hover(Tile *tile, double mouse_x, double mouse_y);
void highlight_tile(Tile *tile);
void unhighlight_tile(Tile *tile);
void click_tile(Tile *tile);
void unclick_tile(Tile *tile);
void reset_tile_color(Tile *tile);
void update_tile_coloring(Tile *tile);
void add_coloring(Tile *tile, float color);
void handle_tile_click(Tile *tile);
