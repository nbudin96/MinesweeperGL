librarylocations := -L./include/libs/
headerlocations := -I./include/headers/
sourcefile_path := ./src/

libs := -lglfw3 -lopengl32 -lgdi32
flags := -g -Wall -std=c99

main.exe : $(sourcefile_path)main.c Shader.o Sprite.o
	gcc $(flags) -o main.exe $(headerlocations) $(librarylocations) $(sourcefile_path)main.c Shader.o Sprite.o $(sourcefile_path)glad.c $(libs) 

Shader.o : $(sourcefile_path)Shader.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Shader.c -o Shader.o $(libs)

Sprite.o : $(sourcefile_path)Sprite.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Sprite.c -o Sprite.o $(libs)

run:
	main.exe

clean:
	del *.exe
	del *.o

