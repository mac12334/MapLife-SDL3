LIB = -L/opt/homebrew/Cellar/sdl3/3.4.16/lib/ -L/opt/homebrew/Cellar/sdl3_image/3.4.6/lib/ -L/opt/homebrew/Cellar/sdl3_ttf/3.2.2/lib/
SRC = $(wildcard src/*.cpp)

default:
	g++ $(SRC) -lsdl3 -lsdl3_image -lsdl3_ttf -Iinclude $(LIB) -o bin/main
	./bin/main