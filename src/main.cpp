#include <stdio.h>
#include <sys/stat.h>
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
    int primaryTile, secondaryTile;
    void** buttonParameters;
};

void makeEditScreen(void*);

void openProject(void* userData, const char* const* fileList, int filter) {
    if (fileList == nullptr) {
        printf("No files picked\n");
        return;
    }

    void** params = (void**)userData;

    // open up screen and project
    Screen* screen = *(Screen**)(params + ButtonIndex_Screen);
    Project* project = *(Project**)(params + ButtonIndex_Project);

    loadProject(project, *fileList);
    project->currentMap = Linked::getIndex(project->mapList, 0, (Map*)nullptr);

    screen->deleteLists = true;
    screen->transitionFunction = makeEditScreen;
}

void initializeButtonParameters(GameState* state) {
    size_t arrayLength = sizeof(Map*) + (sizeof(int*) * 2) + sizeof(Mouse*) + sizeof(SDL_Window*) + sizeof(Screen*) + sizeof(GameState*);
    state->buttonParameters = (void**)malloc(arrayLength);

    // set each index to the correct item
    *(Project**)(state->buttonParameters + ButtonIndex_Project) = state->project;
    *(int**)(state->buttonParameters + ButtonIndex_PrimaryTile) = &state->primaryTile;
    *(int**)(state->buttonParameters + ButtonIndex_SecondaryTile) = &state->secondaryTile;
    *(Mouse**)(state->buttonParameters + ButtonIndex_Mouse) = &state->m;
    *(SDL_Window**)(state->buttonParameters + ButtonIndex_Window) = state->win;
    *(Screen**)(state->buttonParameters + ButtonIndex_Screen) = state->testScreen;
    *(GameState**)(state->buttonParameters + ButtonIndex_GameState) = state;
}

void makeMenuScreen(GameState* gamestate) {
    SDL_Surface* surf;
    makeSurfaceFromColor(&surf, 100, 50, {255, 0, 0, 255});
    SDL_Surface* nSurf;
    makeSurfaceFromColor(&nSurf, 100, 50, {255, 0, 0, 255});

    void (*func)(Button*, void**) = [](Button* button, void** parameters) -> void {
        SDL_Window* win = *(SDL_Window**)(parameters + ButtonIndex_Window);
        SDL_ShowOpenFileDialog(openProject, parameters, win, nullptr, 0, "/User/that_nerdy_dino/Documents/", false);
    };

    void (*nProject)(Button*, void**) = [](Button* button, void** parameters) -> void {
        Screen* screen = *(Screen**)(parameters + ButtonIndex_Screen);
        Project* project = *(Project**)(parameters + ButtonIndex_Project);

        makeDefaultProject(project, 10, 10);
        project->currentMap = Linked::getIndex(project->mapList, 0, (Map*)nullptr);
        screen->deleteLists = true;
        screen->transitionFunction = makeEditScreen;
    };

    gamestate->testScreen->addButton(new Button(gamestate->rend, surf, 10, 10, func, gamestate->buttonParameters));
    gamestate->testScreen->getButtonIndex(0)->renderText(gamestate->rend, gamestate->font, "load", {255, 255, 255, 255});
    gamestate->testScreen->addButton(new Button(gamestate->rend, nSurf, 10, 70, nProject, gamestate->buttonParameters));
    gamestate->testScreen->getButtonIndex(1)->renderText(gamestate->rend, gamestate->font, "new", {255, 255, 255, 255});
}

void makeEditScreen(void* appState) {
    GameState* gamestate = {static_cast<GameState*>(appState)};

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
    
    gamestate->project->currentMap->tileWidth = tileSize; gamestate->project->currentMap->tileHeight = tileSize;
    gamestate->project->currentMap->tileColumns = colums;

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
        Project* project = *(Project**)(parmaters + ButtonIndex_Project);
        // store tilemap data
        int col = project->currentMap->tileColumns;
        int w = project->currentMap->tileWidth;
        int h = project->currentMap->tileHeight;
        // open project
        freeProject(project);
        loadProject(project, "test-project.MapLife");
        project->currentMap = Linked::getIndex(project->mapList, 0, (Map*)nullptr);
        // restore tilemap data
        project->currentMap->tileColumns = col;
        project->currentMap->tileWidth = w;
        project->currentMap->tileHeight = h;
    };
    void (*saveFunc)(Button*, void**) = [](Button* button, void** parmaters)-> void {
        Project* project = *(Project**)(parmaters + ButtonIndex_Project);
        writeProject(project, "test-project.MapLife");
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
    state->testScreen->state = state;

    state->project = (Project*)malloc(sizeof(Project));
    state->project->currentMap = nullptr;
    
    initializeButtonParameters(state);
    
    makeMenuScreen(state);

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

    modifyMap(state->primaryTile, state->project->currentMap, &state->m, 120, 50);
    renderMap(state->rend, state->tileMapTexture, state->project->currentMap, 120, 50, true);
    state->testScreen->tick();

    SDL_RenderPresent(state->rend);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    GameState* state = {static_cast<GameState*>(appstate)};
    
    SDL_StopTextInput(state->win);

    freeProject(state->project);
    free(state->project);

    free(state->buttonParameters);

    TTF_CopyFont(state->font);
    SDL_DestroyTexture(state->tileMapTexture);
    SDL_DestroyRenderer(state->rend);
    SDL_DestroyWindow(state->win);

    destroyScreen(state->testScreen);
    delete state;

    SDL_Quit();
}