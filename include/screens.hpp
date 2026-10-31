#ifndef SCREENS_HPP

#define SCREENS_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <screen.hpp>
#include <map.hpp>
#include <constants.hpp>

#define ALLOC_BUTTON() ((Button*)malloc(sizeof(Button)))
#define ALLOC_TEXTBOX() ((TextBox*)malloc(sizeof(TextBox)))

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

// abstraction to make adding components easier
void addButton(GameState* state, SDL_Surface* surf, int x, int y, void (*func)(Button*, void**), const char* label, SDL_Color color);
void addTextbox(GameState* state, int x, int y, const char* label, uint textboxType = TextBoxType_ANY_TEXT, int maxCharLength = 10, int minimumWidth = 50);

// screen construction functions
void makeMenuScreen(void* state);
void makeEditScreen(void* state);
void makeCreateProjectScreen(void* state);

// file selector functions
void openProject(void* userData, const char* const* fileList, int filter);
void addTilemapDirectory(void* userData, const char* const* fileList, int filter);

#endif