// 3. Compile the shader?
// 4. Do we want to create a program while we are at it?

#include "Shader.h"

const int SHADER_BUFFER_SIZE = 1024;

int read_shader_file(Shader *shader, const char* filename)
{
    FILE *file_ptr = fopen(filename, "r");
    if(file_ptr == NULL)
    {
        fprintf(stderr, "ERROR, could not open the shader file %s for reading!\n", filename);
        return 1;
    }
    char *buffer = calloc(SHADER_BUFFER_SIZE, sizeof(char));
    char line[256];
    int buffer_location_offset = 0;

    while(fgets(line, SHADER_BUFFER_SIZE, file_ptr))
    {
        int line_size = strlen(line);
        if(line_size + buffer_location_offset >= SHADER_BUFFER_SIZE)
        {
            fprintf(stderr, "ERROR, Shader file is too large to read! (current max size is %d bytes)\n", SHADER_BUFFER_SIZE);
            free(buffer);
            fclose(file_ptr);
            return 1;
        }
        strcpy(buffer + buffer_location_offset, line);
        buffer_location_offset += line_size;
    }

    shader->code_string = buffer;
    buffer = NULL;
    fclose(file_ptr);
    return 0;
}

void compile_shader(Shader *shader)
{
    shader->ID = glCreateShader(shader->shader_type);
    glShaderSource(shader->ID, 1, &shader->code_string, NULL);
    glCompileShader(shader->ID);
    free((char*)shader->code_string);
    shader->code_string = NULL;
}

void delete_shader(Shader *shader)
{
    glDeleteShader(shader->ID);
}

