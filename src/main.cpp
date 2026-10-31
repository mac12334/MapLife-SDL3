#include <stdio.h>
#include <sys/stat.h>
#include <string>
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