#include "screens.hpp"

void addButton(GameState* state, SDL_Surface* surf, int x, int y, void (*func)(Button*, void**), const char* label, SDL_Color color) {
    Button* button = ALLOC_BUTTON();
    
    button->defineButton(state->rend, surf, x, y);
    button->setCallback(func, state->buttonParameters);
    button->renderText(state->rend, state->font, label, color);
    state->testScreen->addButton(button);
}

void addTextbox(GameState *state, int x, int y, const char *label, uint textboxType, int maxCharLength, int minimumWidth)
{
    TextBox* textbox = ALLOC_TEXTBOX();
    textbox->defineTextBox(state->rend, state->font, x, y, maxCharLength, minimumWidth, label);
    textbox->setTextBoxType(textboxType);
    state->testScreen->addTextBox(textbox);
}

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
    addButton(gamestate, surf, 10, 10, func, "load", {255, 255, 255, 255});

    // create/new button
    addButton(gamestate, nSurf, 10, 70, nProject, "new", {255, 255, 255, 255});
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
    addButton(gamestate, cancelSurface, 10, 200, cancelFunc, "cancel", {255, 255, 255, 255});

    // select button
    addButton(gamestate, selectTilemapSurface, 300, 10, setTilemadDir, "select", {255, 255, 255, 255});

    // create button
    addButton(gamestate, createSurface, 120, 200, createFunc, "create", {255, 255, 255, 255});

    // width textbox
    addTextbox(gamestate, 10, 70, "width: ", TextBoxType_ONLY_NUMBERS);

    // height textbox
    addTextbox(gamestate, 10, 100, "height: ", TextBoxType_ONLY_NUMBERS);
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