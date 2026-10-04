#include "map.hpp"

void writeMap(Map *map, const char *_file) {
    // open the file in write mode
    FILE* file = fopen(_file, "wb");

    // create the buffer that will hold the compressed map into a 1 dimensional array with the width and height as the first two indexes
    size_t compressedSize = (sizeof(unsigned short) * (map->width * map->height)) + (sizeof(unsigned short) * 2);
    unsigned short *compressedMap = (unsigned short*)malloc(compressedSize);

    // wrtie the map size onto the first two indices
    compressedMap[0] = map->width; compressedMap[1] = map->height;

    // write the map items converted from 2d into 1d starting at index 2
    int area = map->width * map->height;
    int x, y;
    for (int i = 0; i < area; i++) {
        // writes the item of the map using the equation
        // x = i % numColumns
        // y = i / numColumns
        y = i / map->width; x = i % map->width;
        compressedMap[i + 2] = (unsigned short)map->map[y][x];
    }


    // write the buffer array onto the file
    fwrite(compressedMap, sizeof(compressedMap), 2 + (map->width*map->height), file);

    // free the buffer array and close the file
    free(compressedMap);
    fclose(file);
}

void loadMap(Map *map, const char *_file) {
    FILE* file = fopen(_file, "rb");

    fseek(file, 0, SEEK_END);
    long int s = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned short* buffer = (unsigned short*)malloc(s);

    size_t numItems = s / sizeof(unsigned short);
    fread(buffer, sizeof(buffer), numItems, file);

    int tWidth = map->tileWidth; int tHeight = map->tileHeight;
    int tColumns = map->tileColumns;
    
    *map = makeMap((int)buffer[0], (int)buffer[1]);
    map->tileWidth = tWidth; map->tileHeight = tHeight;
    map->tileColumns = tColumns;

    int i = 2;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            map->map[y][x] = buffer[i];
            i++;
        }
    }
    
    free(buffer);
    fclose(file);
}

Map makeMap(int width, int height, int def)
{
    int** map = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        map[i] = (int*)malloc(width * sizeof(int));
    }

    // set each value to default value 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            map[y][x] = def;
        }
    }
    return {map, width, height, 0, 0, 0};
}

void freeMap(Map *map) {
    for (int i = 0; i < map->height; i++) {
        free(map->map[i]);
    }
    free(map->map);
}

void printMap(const Map* map) {
    printf("width: %d, height: %d, level: \n", map->width, map->height);
    for (int y = 0; y < map->height; y++) {
        printf("\t");
        for (int x = 0; x < map->width; x++) {
            printf("%d ", map->map[y][x]);
        }
        printf("\n");
    }
}

void renderMap(SDL_Renderer *render, SDL_Texture *texture, const Map *map, int xOff, int yOff, bool grid) {
    SDL_FRect source = {0, 0, (float)map->tileWidth, (float)map->tileHeight};
    SDL_FRect dest = {(float)xOff, (float)yOff, (float)source.w * 5, (float)source.h * 5};

    int index = -1;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            dest.x = (x * dest.w) + xOff; dest.y = (y * dest.h) + yOff;
            if (map->map[y][x] != 0) {
                index = map->map[y][x] - 1;
                source.x = (index % map->tileColumns) * map->tileWidth;
                source.y = (index / map->tileColumns) * map->tileHeight;
                SDL_RenderTexture(render, texture, &source, &dest);
            }
            if (!grid) continue;
            SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
            SDL_RenderRect(render, &dest);
        }
    }
}

void modifyMap(int num, Map *map, const Mouse *mouse, int xOff, int yOff) {
    if (!mouse->left && !mouse->right) return;
    // figure out the x and y index
    int x = (mouse->x - xOff) / (map->tileWidth*5);
    int y = (mouse->y - yOff) / (map->tileHeight*5);

    if (mouse->x - xOff < 0) x = -1;
    if (mouse->y - yOff < 0) y = -1;

    if (!((0 <= x && x < map->width) && (0 <= y && y < map->height))) return;

    map->map[y][x] = (mouse->left) ? num + 1 : 0;
}
