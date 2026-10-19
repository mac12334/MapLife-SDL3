#ifndef MAP_HPP

#define MAP_HPP

#include <iostream>
#include <zip/zip.h>
#include <cjson/cJSON.h>
#include <SDL3/SDL.h>

#include "inputs.hpp"
#include "linkedlist.h"

struct Map {
    int** map;
    int width, height;
    int tileWidth, tileHeight;
    int tileColumns;
};

struct Project {
    Map* currentMap;
    Linked::LinkedList<Map*>* mapList;
    Linked::LinkedList<char*>* tilemapDirs;
};


void writeMap(Map* map, const char* fileName);
void loadMap(Map* map, const char* fileName);

void writeProject(Project* project, const char* zipName);
void loadProject(Project* project, const char* zipName);
void makeDefaultProject(Project* project, int width, int height, int def=0);
void freeProject(Project* project);

Map makeMap(int width, int height, int def = 0);
void freeMap(Map* map);

void printMap(const Map* map);

void renderMap(SDL_Renderer* render, SDL_Texture* texture, const Map* map, int xOff = 0, int yOff = 0, bool grid = false);
void modifyMap(int num, Map* map, const Mouse* mouse, int xOff = 0, int yOff = 0);

#endif