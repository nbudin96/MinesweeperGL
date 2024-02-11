#pragma once
#include "glad.h"
#include "Mesh.h"
#include "Spritesheet.h"
#include "Shader.h"

enum GAMEOBJECT_TYPE {MINE = 0, FLAG = 1, TILE = 2, NUMBER_TILE = 3};

typedef struct Gameobject {
    Mesh *mesh;
    float position[3] ;
    float scale[3];
    int type;
} Gameobject;

void position_object(Gameobject *gameobj);
void scale_object(Gameobject *gameobj);
