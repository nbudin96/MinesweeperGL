#include "Spritesheet.h"

// Creates a spritesheet struct
Spritesheet *create_spritesheet(const char *filename, int cols, int rows)
{
    Spritesheet *spritesheet = calloc(1, sizeof(Spritesheet));
    spritesheet->cols = cols;
    spritesheet->rows = rows;
    if(load_spritesheet(spritesheet, filename))
    {
        return spritesheet;
    }
    fprintf(stderr, "ERROR: could not create spritesheet!\n");
    free(spritesheet);
    return NULL;
}

// Loads an image into memory of the spritesheet
unsigned char *load_spritesheet(Spritesheet *spritesheet, const char *filename)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &spritesheet->width, &spritesheet->height, &spritesheet->nrchannels, 0);
    if(data)
    {
        spritesheet->data = data;
        return data;
    }
    else{
        fprintf(stderr, "ERROR, could not load image : \"%s\"!\n", filename);
        return NULL;
    }
}

// Frees spritesheet data and then spritesheet
void free_spritesheet(Spritesheet *spritesheet)
{
    stbi_image_free(spritesheet->data);
    free(spritesheet);
}

// Calculates indv. size of the sprite located in the spritesheet and stores in Sprite struct
void calculate_sprite_size(Sprite *sprite, int cols, int rows)
{
    sprite->sprite_width = sprite->spritesheet->width / cols;
    sprite->sprite_height = sprite->spritesheet->height / rows;
}

// Calculates the pixel offset (x and y) of the sprite's location in the spritesheet and stores in the Sprite's struct
void calculate_texture_offsets(Sprite *sprite)
{
    printf("Sprite Width: %d\n", sprite->sprite_width);
    printf("Sprite Height: %d\n", sprite->sprite_height);
    printf("New X index : %d, New Y index : %d\n\n", sprite->spritesheet_x_index, sprite->spritesheet_y_index);
    sprite->bottom_left[0] = (float)(sprite->spritesheet_x_index * sprite->sprite_width);
    sprite->bottom_left[0] = sprite->bottom_left[0] / (float)sprite->spritesheet->width;
    sprite->bottom_left[1] = (float)(sprite->spritesheet_y_index * sprite->sprite_height);
    sprite->bottom_left[1] = sprite->bottom_left[1] / (float)sprite->spritesheet->height;
    printf("Sprite_bottom_left x : %f\n", sprite->bottom_left[0]);
    printf("Sprite_bottom_left y : %f\n", sprite->bottom_left[1]);

    sprite->bottom_right[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width) + sprite->sprite_width);
    sprite->bottom_right[0] = sprite->bottom_right[0] / (float)sprite->spritesheet->width;
    sprite->bottom_right[1] = (float)(sprite->spritesheet_y_index * sprite->sprite_height);
    sprite->bottom_right[1] = sprite->bottom_right[1] / (float)sprite->spritesheet->height;
    printf("Sprite_bottom_right x : %f\n", sprite->bottom_right[0]);
    printf("Sprite_bottom_right y : %f\n", sprite->bottom_right[1]);

    sprite->top_left[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width));
    sprite->top_left[0] = sprite->top_left[0] / (float)sprite->spritesheet->width;
    sprite->top_left[1] = (float)((sprite->spritesheet_y_index * sprite->sprite_height) + sprite->sprite_height);
    sprite->top_left[1] = sprite->top_left[1] / (float)sprite->spritesheet->height;
    printf("Sprite_top_left x : %f\n", sprite->top_left[0]);
    printf("Sprite_top_left y : %f\n", sprite->top_left[1]);

    sprite->top_right[0] = (float)((sprite->spritesheet_x_index * sprite->sprite_width) + sprite->sprite_width);
    sprite->top_right[0] = sprite->top_right[0] / (float)sprite->spritesheet->width;
    sprite->top_right[1] = (float)((sprite->spritesheet_y_index * sprite->sprite_height) + sprite->sprite_height);
    sprite->top_right[1] = sprite->top_right[1] / (float)sprite->spritesheet->height;
    printf("Sprite_top_right x : %f\n", sprite->top_right[0]);
    printf("Sprite_top_right y : %f\n\n", sprite->top_right[1]);
}

// Creates a sprite from a spritesheet
Sprite *create_sprite(Spritesheet *spritesheet, int x_index, int y_index)
{
    Sprite *new_sprite = malloc(sizeof(Sprite));
    new_sprite->spritesheet = spritesheet;
    calculate_sprite_size(new_sprite, new_sprite->spritesheet->cols, new_sprite->spritesheet->rows);
    new_sprite->spritesheet_x_index = x_index;
    new_sprite->spritesheet_y_index = y_index;
    calculate_texture_offsets(new_sprite);
    return new_sprite;
}

void change_sprite_offsets(Sprite *sprite, int new_x_index, int new_y_index)
{
    sprite->spritesheet_x_index = new_x_index;
    sprite->spritesheet_y_index = new_y_index;
    calculate_texture_offsets(sprite);
}
