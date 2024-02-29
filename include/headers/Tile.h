#pragma once
#include "stdbool.h"
#include "sprite.h"
#include "Shader.h"

typedef struct Tile {
    bool mouse_hover;
    bool active;
    bool wants_hover;
    float highlight_amt;
    Sprite *sprite;
} Tile;

Tile *create_tile(Sprite *sprite);
bool check_mouse_hover(Tile *tile, double mouse_x, double mouse_y);
void highlight_tile(Tile *tile, bool highlighted);
