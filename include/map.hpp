#ifndef MAP_HPP

#define MAP_HPP

#include <iostream>
#include <SDL3/SDL.h>
#include "inputs.hpp"

struct Map {
    int** map;
    int width, height;
    int tileWidth, tileHeight;
    int tileColumns;
};

void writeMap(Map*, const char*);
void loadMap(Map*, const char*);

Map makeMap(int, int, int = 0);
void freeMap(Map*);

void printMap(const Map*);

void renderMap(SDL_Renderer*, SDL_Texture*, const Map*, int = 0, int = 0, bool = false);
void modifyMap(int, Map*, const Mouse*, int = 0, int = 0);

#endif