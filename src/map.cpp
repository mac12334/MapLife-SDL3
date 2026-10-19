#include "map.hpp"

typedef unsigned short ushort;

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

ushort* createMapBuffer(Map* map) {
    int area = (map->width * map->height);

    size_t size = (sizeof(ushort) * area) + (sizeof(ushort) * 2);
    ushort* buffer = (ushort*)malloc(size);

    buffer[0] = (ushort)map->width;
    buffer[1] = (ushort)map->height;
    
    int x, y = 0;
    for (int i = 0; i < area; i++) {
        y = i / map->width; x = i % map->width;
        buffer[i + 2] = (ushort)map->map[y][x];
    }

    return buffer;
}

void writeProject(Project *project, const char *zipName) {
    int err;
    zip_t* archive = zip_open(zipName, ZIP_CREATE, &err);

    void (*func)(Map*, int, void**) = [](Map* map, int index, void** params) -> void {
        // get archive
        zip_t* arch = *(zip_t**)(params);

        // create the buffer
        ushort* buffer = createMapBuffer(map);
        size_t s = (sizeof(ushort) * buffer[0] * buffer[1]) + (sizeof(ushort) * 2);

        // write the buffer onto a source
        zip_source_t* source = zip_source_buffer(arch, buffer, s, 1);

        // add the source to the zip file
        char name[13];
        snprintf(name, sizeof(char) * 13, "maps/map%d", index);
        zip_file_add(arch, name, source, ZIP_FL_OVERWRITE);

        // close the source
        zip_source_close(source);
    };

    cJSON* projectProperties = cJSON_CreateObject();
    cJSON* array = cJSON_AddArrayToObject(projectProperties, "tilemapDirs");

    void (*addDir)(char*, void**) = [](char* directory, void** params) -> void {
        cJSON* jsonArray = *(cJSON**)(params);
        cJSON* item = cJSON_CreateString(directory);
        cJSON_AddItemToArray(jsonArray, item);
    };

    Linked::doWhileWithIndex(project->mapList, func, (void**)&archive);
    Linked::doWhile(project->tilemapDirs, addDir, (void**)(&array));

    // write the json file into the zip archive
    // convert the json struct into a c string
    char* jsonContents = cJSON_Print(projectProperties);
    
    // write the c string json into the project file
    size_t jsonSize = (strlen(jsonContents) * sizeof(char)) + sizeof(char);
    zip_source* propSource = zip_source_buffer(archive, jsonContents, jsonSize, 0);

    zip_file_add(archive, "properties.json", propSource, ZIP_FL_OVERWRITE);

    zip_source_close(propSource);

    zip_close(archive);
    cJSON_free(jsonContents);
    cJSON_Delete(projectProperties);
}

void makeMapFromFile(Project* project, zip_file_t* file, size_t fileSize) {
    Map* map = (Map*)malloc(sizeof(Map));
    
    // make file buffer
    ushort* buffer = (ushort*)malloc(fileSize);
    zip_fread(file, buffer, fileSize);
    
    // make map from said buffer
    int w = (int)*(buffer + 0);
    int h = (int)*(buffer + 1);
    *map = makeMap(w, h);
    
    int index = 2;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            map->map[y][x] = buffer[index];
            index++;
        }
    }

    free(buffer);
    Linked::addItem<Map*>(&project->mapList, map);
}

void loadTileMaps(Project* project, zip_file* file, size_t fileSize) {
    char* buffer = (char*)malloc(fileSize);

    zip_fread(file, buffer, fileSize);

    cJSON* propJson = cJSON_Parse(buffer);
    cJSON* array = cJSON_GetObjectItem(propJson, "tilemapDirs");
    int length = cJSON_GetArraySize(array);
    for (int i = 0; i < length; i++) {
        cJSON* item = cJSON_GetArrayItem(array, i);
        char* sitem = cJSON_GetStringValue(item);

        size_t size = (sizeof(char) * strlen(sitem)) + sizeof(char);
        char* directory = (char*)malloc(size);

        strcpy(directory, sitem);
        Linked::addItem(&project->tilemapDirs, directory);
    }

    free(buffer);
    cJSON_Delete(propJson);
}

void loadProject(Project *project, const char *zipName) {
    int err;
    zip_t* archive = zip_open(zipName, ZIP_CREATE, &err);

    size_t propFileSize;
    
    // list out all files within the zip file and store map indexes
    int numFiles = zip_get_num_entries(archive, ZIP_FL_UNCHANGED);
    for (int i = 0; i < numFiles; i++) {
        // get the stats
        zip_stat_t stat;
        zip_stat_init(&stat);
        if (zip_stat_index(archive, i, 0, &stat) != 0) continue;
        
        if (strcmp(stat.name, "properties.json") == 0) {
            propFileSize = stat.size;
            continue;
        }
        if (strstr(stat.name, "maps/") == NULL) continue;
        
        // open file from index list
        zip_file_t* file = zip_fopen_index(archive, i, 0);
        
        //make map
        makeMapFromFile(project, file, stat.size);

        // close file
        zip_fclose(file);
    }

    zip_file_t* propFile = zip_fopen(archive, "properties.json", 0);

    loadTileMaps(project, propFile, propFileSize);

    zip_fclose(propFile);

    zip_close(archive);
}

void makeDefaultProject(Project *project, int width, int height, int def) {
    Map* map = (Map*)malloc(sizeof(Map));
    *map = makeMap(width, height, def);
    Linked::addItem(&project->mapList, map);
}

void _freeMapList(Map* map, void**) {
    freeMap(map);
    free(map);
}

void _freeTilemapList(char* tilemapDir, void**) {
    free(tilemapDir);
}

void freeProject(Project *project) {
    Linked::doWhile(project->mapList, _freeMapList);
    Linked::doWhile(project->tilemapDirs, _freeTilemapList);
    Linked::destroyList(&project->tilemapDirs);
    Linked::destroyList(&project->mapList);
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
    if (map == nullptr) return;
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
    if (map == nullptr) return;
    if (!mouse->left && !mouse->right) return;
    // figure out the x and y index
    int x = (mouse->x - xOff) / (map->tileWidth*5);
    int y = (mouse->y - yOff) / (map->tileHeight*5);

    if (mouse->x - xOff < 0) x = -1;
    if (mouse->y - yOff < 0) y = -1;

    if (!((0 <= x && x < map->width) && (0 <= y && y < map->height))) return;

    map->map[y][x] = (mouse->left) ? num + 1 : 0;
}
