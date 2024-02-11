librarylocations := -L./include/libs/
headerlocations := -I./include/headers/
sourcefile_path := ./src/

libs := -lglfw3 -lopengl32 -lgdi32
flags := -g -Wall -std=c99

main.exe : $(sourcefile_path)main.c
	gcc $(flags) -o main.exe $(headerlocations) $(librarylocations) $(sourcefile_path)main.c $(sourcefile_path)Mesh.c $(sourcefile_path)Shader.c $(sourcefile_path)Spritesheet.c $(sourcefile_path)glad.c $(libs) 

run:
	main.exe

clean:
	del *.exe
	del *.o
