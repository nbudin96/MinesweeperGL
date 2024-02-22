librarylocations := -L./include/libs/
headerlocations := -I./include/headers/
sourcefile_path := ./src/

libs := -lglfw3 -lopengl32 -lgdi32
flags := -g -Wall -std=c99

main.exe : $(sourcefile_path)main.c Mesh.o Shader.o Spritesheet.o
	gcc $(flags) -o main.exe $(headerlocations) $(librarylocations) $(sourcefile_path)main.c Mesh.o Shader.o Spritesheet.o $(sourcefile_path)glad.c $(libs) 

Mesh.o : $(sourcefile_path)Mesh.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Mesh.c -o Mesh.o $(libs)

Shader.o : $(sourcefile_path)Shader.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Shader.c -o Shader.o $(libs)

Spritesheet.o : $(sourcefile_path)Spritesheet.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Spritesheet.c -o Spritesheet.o $(libs)

run:
	main.exe

clean:
	del *.exe
	del *.o

