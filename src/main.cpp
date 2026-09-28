#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "inputs.hpp"
#include "components.hpp"
#include "map.hpp"

#define INIT_FLAGS SDL_INIT_VIDEO

struct GameState {
    SDL_Window* win;
    SDL_Renderer* rend;
    TTF_Font* font;
    Mouse m;
    Button* button;
    TextBox* tbox;
    TextBox* currentTextBox{nullptr};
};

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
    SDL_Surface* bSurf = nullptr;
    makeSurfaceFromColor(&bSurf, 60, 60, {255, 0, 0, 255});
    state->button = new Button(state->rend, bSurf, 10, 10);
    state->tbox = new TextBox(state->rend, state->font, 10, 80, 20, 100, "Test");

    SDL_StartTextInput(state->win);

    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e) {
    GameState* state = {static_cast<GameState*>(appstate)};

    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    updateMouse(*e, &state->m);
    handleTextBox(state->rend, state->font, *e, state->currentTextBox);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    GameState* state = {static_cast<GameState*>(appstate)};

    updateTextBoxes(&state->m, &state->currentTextBox, 1, state->tbox);

    SDL_SetRenderDrawColor(state->rend, 255, 255, 255, 255);
    SDL_RenderClear(state->rend);

    state->button->render(state->rend);
    if (state->button->update(&state->m)) printf("It Works!\n");
    state->tbox->render(state->rend);

    SDL_RenderPresent(state->rend);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    GameState* state = {static_cast<GameState*>(appstate)};
    
    SDL_StopTextInput(state->win);

    TTF_CopyFont(state->font);
    SDL_DestroyRenderer(state->rend);
    SDL_DestroyWindow(state->win);

    delete state->button;
    delete state->tbox;
    delete state;

    SDL_Quit();
}