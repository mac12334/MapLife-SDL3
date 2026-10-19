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

void makeEditScreen(void*);
void makeCreateProjectScreen(void* state);

void openProject(void* userData, const char* const* fileList, int filter) {
    if (fileList == nullptr) {
        printf("something bad happened\n");
        return;
    }

    if (*fileList == nullptr) {
        printf("no files selected\n");
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

void addTilemapDirectory(void* userData, const char* const* fileList, int filter) {
    if (fileList == nullptr) {
        printf("something bad happened\n");
        return;
    }

    if (*fileList == nullptr) {
        printf("no files selected\n");
        return;
    }

    void** params = (void**)userData;

    // grab project
    Project* project = *(Project**)(params + ButtonIndex_Project);

    // add filename to directory list
    size_t length = (sizeof(char) * strlen(*fileList)) + sizeof(char);
    char* dir = (char*)malloc(length);
    strcpy(dir, *fileList);
    Linked::addItem(&project->tilemapDirs, dir);
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

void makeMenuScreen(void* state) {
    GameState* gamestate = {static_cast<GameState*>(state)};
    SDL_Surface* surf;
    makeSurfaceFromColor(&surf, 100, 50, {255, 0, 0, 255});
    SDL_Surface* nSurf;
    makeSurfaceFromColor(&nSurf, 100, 50, {255, 0, 0, 255});

    gamestate->project->currentMap = nullptr;

    void (*func)(Button*, void**) = [](Button* button, void** parameters) -> void {
        SDL_Window* win = *(SDL_Window**)(parameters + ButtonIndex_Window);
        SDL_ShowOpenFileDialog(openProject, parameters, win, nullptr, 0, NULL, false);
    };

    void (*nProject)(Button*, void**) = [](Button* button, void** parameters) -> void {
        Screen* screen = *(Screen**)(parameters + ButtonIndex_Screen);

        screen->deleteLists = true;
        screen->transitionFunction = makeCreateProjectScreen;
    };

    // load button
    Button* loadButton = ALLOC_BUTTON();
    loadButton->defineButton(gamestate->rend, surf, 10, 10);
    loadButton->setCallback(func, gamestate->buttonParameters);
    loadButton->renderText(gamestate->rend, gamestate->font, "load", {255, 255, 255, 255});
    gamestate->testScreen->addButton(loadButton);

    // create/new button
    Button* createButton = ALLOC_BUTTON();
    createButton->defineButton(gamestate->rend, nSurf, 10, 70);
    createButton->setCallback(nProject, gamestate->buttonParameters);
    createButton->renderText(gamestate->rend, gamestate->font, "new", {255, 255, 255, 255});
    gamestate->testScreen->addButton(createButton);
}

void makeCreateProjectScreen(void* state) {
    GameState* gamestate = {static_cast<GameState*>(state)};

    SDL_Surface* cancelSurface = nullptr;
    makeSurfaceFromColor(&cancelSurface, 100, 50, {255, 0, 0, 255});
    SDL_Surface* selectTilemapSurface = nullptr;
    makeSurfaceFromColor(&selectTilemapSurface, 100, 50, {255, 0, 0, 255});
    SDL_Surface* createSurface = nullptr;
    makeSurfaceFromColor(&createSurface, 100, 50, {255, 0, 0, 255});

    void (*cancelFunc)(Button*, void**) = [](Button* button, void** parameters) -> void {
        Project* project = *(Project**)(parameters + ButtonIndex_Project);
        Screen* screen = *(Screen**)(parameters + ButtonIndex_Screen);

        freeProject(project);
        screen->deleteLists = true;
        screen->transitionFunction = makeMenuScreen;
    };
    void (*setTilemadDir)(Button*, void**) = [](Button* button, void** parameters) -> void {
        SDL_Window* win = *(SDL_Window**)(parameters + ButtonIndex_Window);
        SDL_ShowOpenFileDialog(addTilemapDirectory, parameters, win, NULL, 0, NULL, false);
    };
    void (*createFunc)(Button*, void**) = [](Button* button, void** parameters) -> void {
        // get the screen and project
        Screen* screen = *(Screen**)(parameters + ButtonIndex_Screen);
        Project* project = *(Project**)(parameters + ButtonIndex_Project);

        if (Linked::getLength(project->tilemapDirs) == 0) {
            printf("no tilemap selected\n");
            return;
        }

        // get the width and height textboxes
        TextBox* wTxtBox = screen->getTextboxIndex(0);
        TextBox* hTxtBox = screen->getTextboxIndex(1);

        // make a project with the dimensions
        char widthText[10]; char heightText[10];
        wTxtBox->getText(widthText); hTxtBox->getText(heightText);
        if (widthText[0] == '\0' || heightText[0] == '\0') {
            printf("size not selected!\n");
            return;
        }
        int width = atoi(widthText); int height = atoi(heightText);
        
        makeDefaultProject(project, width, height);
        project->currentMap = Linked::getIndex(project->mapList, 0, (Map*)nullptr);

        screen->deleteLists = true;
        screen->transitionFunction = makeEditScreen;
    };

    // cancel button
    Button* cancelButton = ALLOC_BUTTON();
    cancelButton->defineButton(gamestate->rend, cancelSurface, 10, 200);
    cancelButton->setCallback(cancelFunc, gamestate->buttonParameters);
    cancelButton->renderText(gamestate->rend, gamestate->font, "cancel", {255, 255, 255, 255});
    gamestate->testScreen->addButton(cancelButton);

    // select button
    Button* selectButton = ALLOC_BUTTON();
    selectButton->defineButton(gamestate->rend, selectTilemapSurface, 300, 10);
    selectButton->setCallback(setTilemadDir, gamestate->buttonParameters);
    selectButton->renderText(gamestate->rend, gamestate->font, "select tilemap", {255, 255, 255, 255});
    gamestate->testScreen->addButton(selectButton);

    // create button
    Button* createButton = ALLOC_BUTTON();
    createButton->defineButton(gamestate->rend, createSurface, 120, 200);
    createButton->setCallback(createFunc, gamestate->buttonParameters);
    createButton->renderText(gamestate->rend, gamestate->font, "create", {255, 255, 255, 255});
    gamestate->testScreen->addButton(createButton);

    // width textbox
    TextBox* widthTextbox = ALLOC_TEXTBOX();
    widthTextbox->defineTextBox(gamestate->rend, gamestate->font, 10, 70, 10, 50, "width: ");
    widthTextbox->setTextBoxType(TextBoxType_ONLY_NUMBERS);
    gamestate->testScreen->addTextBox(widthTextbox);

    // height textbox
    TextBox* heightTextbox = ALLOC_TEXTBOX();
    heightTextbox->defineTextBox(gamestate->rend, gamestate->font, 10, 100, 10, 50, "height: ");
    heightTextbox->setTextBoxType(TextBoxType_ONLY_NUMBERS);
    gamestate->testScreen->addTextBox(heightTextbox);
}

void makeEditScreen(void* appState) {
    GameState* gamestate = {static_cast<GameState*>(appState)};

    SDL_Surface* surf = IMG_Load(Linked::getIndex(gamestate->project->tilemapDirs, 0, (char*)nullptr));
    if (surf == NULL) {
        printf("problem\n");
    }
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
        renderRect.x = (renderStartX) + (index % renderColumn) * (renderSize + renderMargin); 
        renderRect.y = (renderStartY) + (index / renderColumn) * (renderSize + renderMargin);

        // allocate tile button
        Button* tilebutton = ALLOC_BUTTON();
        tilebutton->defineButton(gamestate->tileMapTexture, renderRect, tileRect);
        tilebutton->setCallback(func, gamestate->buttonParameters);
        tilebutton->index = index;

        gamestate->testScreen->addButton(tilebutton);
    }
    
    SDL_Surface* loadSurface = nullptr;
    makeSurfaceFromColor(&loadSurface, 90, 35, {150, 150, 150, 255});
    SDL_Surface* saveSurface = nullptr;
    makeSurfaceFromColor(&saveSurface, 90, 35, {150, 150, 150, 255});
    SDL_Surface* menuSurface = nullptr;
    makeSurfaceFromColor(&menuSurface, 90, 35, {150, 150, 150, 255});
    
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
    void (*menuFunc)(Button*, void**) = [](Button* button, void** parameters) -> void {
        Project* project = *(Project**)(parameters + ButtonIndex_Project);
        Screen* screen = *(Screen**)(parameters + ButtonIndex_Screen);

        freeProject(project);
        screen->deleteLists = true;
        screen->transitionFunction = makeMenuScreen;
    };
    // load button
    Button* loadButton = ALLOC_BUTTON();
    loadButton->defineButton(gamestate->rend, loadSurface, 10, 6);
    loadButton->setCallback(loadFunc, gamestate->buttonParameters);
    loadButton->renderText(gamestate->rend, gamestate->font, "load", {255, 255, 255, 255});
    gamestate->testScreen->addButton(loadButton);
    // save button
    Button* saveButton = ALLOC_BUTTON();
    saveButton->defineButton(gamestate->rend, saveSurface, 110, 6);
    saveButton->setCallback(saveFunc, gamestate->buttonParameters);
    saveButton->renderText(gamestate->rend, gamestate->font, "save", {255, 255, 255, 255});
    gamestate->testScreen->addButton(saveButton);
    // menu button
    Button* menuButton = ALLOC_BUTTON();
    menuButton->defineButton(gamestate->rend, menuSurface, 210, 6);
    menuButton->setCallback(menuFunc, gamestate->buttonParameters);
    menuButton->renderText(gamestate->rend, gamestate->font, "menu", {255, 255, 255, 255});
    gamestate->testScreen->addButton(menuButton);
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