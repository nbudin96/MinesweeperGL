UNAME := $(shell uname)
build_location := ./build/
librarylocations := -L./include/libs/
libs := -lglfw3 -lopengl32 -lgdi32
output_filename := MinesweeperGL.exe

ifeq ($(UNAME), Linux)
	librarylocations := 
	libs := -lm -lglfw -lGL -lX11 -lpthread -lXrandr -ldl
	output_filename := MinesweeperGL
endif
ifeq ($(UNAME), Windows)
	librarylocations := -L./include/libs/
endif

headerlocations := -I./include/headers/
sourcefile_path := ./src/

flags := -g -Wall -std=c99

$(build_location)$(output_filename) : $(sourcefile_path)main.c $(build_location)Shader.o $(build_location)Sprite.o $(build_location)Tile.o
	gcc $(flags) -o $(build_location)$(output_filename) $(headerlocations) $(librarylocations) $(sourcefile_path)main.c $(build_location)Shader.o $(build_location)Sprite.o $(build_location)Tile.o $(sourcefile_path)glad.c $(libs) 

$(build_location)Shader.o : $(sourcefile_path)Shader.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Shader.c -o $(build_location)Shader.o $(libs)

$(build_location)Sprite.o : $(sourcefile_path)Sprite.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Sprite.c -o $(build_location)Sprite.o $(libs)

$(build_location)Tile.o : $(sourcefile_path)Tile.c
	gcc $(flags) $(headerlocations) $(librarylocations) -c $(sourcefile_path)Tile.c -o $(build_location)Tile.o $(libs)

run:
	$(build_location)$(output_filename)

clean:
	rm $(build_location)$(output_filename)
	rm $(build_location)*.o

