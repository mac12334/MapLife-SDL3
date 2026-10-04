#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include "inputs.hpp"
#include "components.hpp"
#include "map.hpp"
#include "screens.hpp"
#include "constants.hpp"

#define INIT_FLAGS SDL_INIT_VIDEO

struct GameState {
    SDL_Window* win;
    SDL_Renderer* rend;
    TTF_Font* font;
    Mouse m;
    Screen* testScreen;
    Project* project;
    SDL_Texture* tileMapTexture;
    Map* map;
    int primaryTile, secondaryTile;
    void** buttonParameters;
};

void initializeButtonParameters(GameState* state) {
    size_t arrayLength = sizeof(Map*) + (sizeof(int*) * 2) + sizeof(Mouse*) + sizeof(SDL_Window*);
    state->buttonParameters = (void**)malloc(arrayLength);

    // set each index to the correct item
    *(Map**)(state->buttonParameters + ButtonIndex_Map) = state->map;
    *(int**)(state->buttonParameters + ButtonIndex_PrimaryTile) = &state->primaryTile;
    *(int**)(state->buttonParameters + ButtonIndex_SecondaryTile) = &state->secondaryTile;
    *(Mouse**)(state->buttonParameters + ButtonIndex_Mouse) = &state->m;
    *(SDL_Window**)(state->buttonParameters + ButtonIndex_Window) = state->win;
}

void makeEditScreen(GameState* gamestate) {
    SDL_Surface* surf = IMG_Load("/Users/that_nerdy_dino/Documents/pixel-art/DungeonTiles-tileset.png");
    gamestate->tileMapTexture = SDL_CreateTextureFromSurface(gamestate->rend, surf);
    SDL_DestroySurface(surf);

    SDL_SetTextureScaleMode(gamestate->tileMapTexture, SDL_SCALEMODE_NEAREST);

    void (*func)(Button*, void**) = [](Button* button, void** parmaters)-> void {
        int* primaryTile = *(int**)(parmaters + ButtonIndex_PrimaryTile);
        *primaryTile = button->index;
    };

    int tileSize = 8;
    int colums = 4;
    int numTiles = 9;
    int renderSize = 50;
    int renderColumn = 2;

    int renderMargin = 5;

    int renderStartX = 5;
    int renderStartY = 50;

    gamestate->map->tileWidth = tileSize; gamestate->map->tileHeight = tileSize;
    gamestate->map->tileColumns = colums;

    SDL_FRect tileRect = {0, 0, (float)tileSize, (float)tileSize};
    SDL_FRect renderRect = {0, 0, (float)renderSize, (float)renderSize};

    for (int index = 0; index < numTiles; index++) {
        tileRect.x = (index % colums) * tileSize; tileRect.y = (index / colums) * tileSize;
        renderRect.x = (renderStartX) + (index % renderColumn) * (renderSize + renderMargin); renderRect.y = (renderStartY) + (index / renderColumn) * (renderSize + renderMargin);

        gamestate->testScreen->addButton(new Button(gamestate->tileMapTexture, renderRect, tileRect, func, gamestate->buttonParameters, index));
    }

    SDL_Surface* loadSurface = nullptr;
    makeSurfaceFromColor(&loadSurface, 90, 35, {150, 150, 150, 255});
    SDL_Surface* saveSurface = nullptr;
    makeSurfaceFromColor(&saveSurface, 90, 35, {150, 150, 150, 255});

    void (*loadFunc)(Button*, void**) = [](Button* button, void** parmaters)-> void {
        Map* map = *(Map**)(parmaters + ButtonIndex_Map);
        freeMap(map);
        loadMap(map, "bin/test");
    };
    void (*saveFunc)(Button*, void**) = [](Button* button, void** parmaters)-> void {
        Map* map = *(Map**)(parmaters + ButtonIndex_Map);
        writeMap(map, "bin/test");
    };

    gamestate->testScreen->addButton(new Button(gamestate->rend, loadSurface, 10, 6, loadFunc, gamestate->buttonParameters));
    gamestate->testScreen->addButton(new Button(gamestate->rend, saveSurface, 120, 6, saveFunc, gamestate->buttonParameters));

    int length = gamestate->testScreen->getButtonListLength();
    gamestate->testScreen->getButtonIndex(length - 2)->renderText(gamestate->rend, gamestate->font, "load", {255, 255, 255, 255});
    gamestate->testScreen->getButtonIndex(length - 1)->renderText(gamestate->rend, gamestate->font, "save", {255, 255, 255, 255});
}

SDL_AppResult SDL_AppInit(void** appstate, int, char**) {
    if (!SDL_Init(INIT_FLAGS)) {
        printf("SDL did not initialize: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!TTF_Init()) {
        printf("ttf did not initialize: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    
    GameState* state = {new GameState()};
    
    SDL_CreateWindowAndRenderer("Map Life", 600, 600, SDL_WINDOW_RESIZABLE, &state->win, &state->rend);
    state->font = TTF_OpenFont("assets/GalaferaMedium-V4xze.ttf", 15);

    state->testScreen = new Screen(state->rend, &state->m);
    state->map = (Map*)malloc(sizeof(Map));
    *state->map = makeMap(10, 10, 0);

    initializeButtonParameters(state);

    makeEditScreen(state);

    SDL_StartTextInput(state->win);

    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e) {
    GameState* state = {static_cast<GameState*>(appstate)};

    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    updateMouse(*e, &state->m);
    handleTextBox(state->rend, state->font, *e, state->testScreen->getCurrentTextBox());

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    GameState* state = {static_cast<GameState*>(appstate)};
    
    SDL_SetRenderDrawColor(state->rend, 255, 255, 255, 255);
    SDL_RenderClear(state->rend);

    modifyMap(state->primaryTile, state->map, &state->m, 120, 50);
    renderMap(state->rend, state->tileMapTexture, state->map, 120, 50, true);
    state->testScreen->tick();

    SDL_RenderPresent(state->rend);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    GameState* state = {static_cast<GameState*>(appstate)};
    
    SDL_StopTextInput(state->win);
    freeMap(state->map);
    free(state->map);
    free(state->buttonParameters);

    TTF_CopyFont(state->font);
    SDL_DestroyTexture(state->tileMapTexture);
    SDL_DestroyRenderer(state->rend);
    SDL_DestroyWindow(state->win);

    destroyScreen(state->testScreen);
    delete state;

    SDL_Quit();
}