#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "glad.h"
#include "glfw3.h"

typedef struct Shader {
    int shader_type;
    const char *code_string;
    const char *filename;
    unsigned int ID;
} Shader;

int read_shader_file(Shader *shader, const char* filename);
void compile_shader(Shader *shader);
void delete_shader(Shader *shader);
