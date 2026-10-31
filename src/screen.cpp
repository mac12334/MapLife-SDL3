#include "screen.hpp"


void renderAndUpdateButton(Button* item, void** parameters) {
    if (parameters == nullptr) { // this function requires paramters to run
        printf("no parameters\n");
        return;
    }

    // grabs the needed paramters from the list
    SDL_Renderer* renderer = (SDL_Renderer*)(*(parameters));
    Mouse* mouse = (Mouse*)(*(parameters + 1));

    // updates and renders the button
    item->callbackUpdate(mouse);
    item->render(renderer);
}

void renderAndUpdateTextBoxes(TextBox* txtbox, void** parameters) {
    if (parameters == nullptr) { // this function requirs parameters
        printf("no parameters\n");
        return;
    }
    // grab the needed parameters
    SDL_Renderer* renderer = (SDL_Renderer*)(*(parameters));
    Mouse* mouse = (Mouse*)(*(parameters + 1));
    TextBox** currentTextBox = (TextBox**)(*(parameters + 2));

    // update and render the textbox
    updateTextBox(mouse, currentTextBox, txtbox);
    txtbox->render(renderer);
}

void _destroyTextBox(TextBox* txtbox, void**) {
    txtbox->destroy(); // deallocate any memory the textbox used
    free(txtbox);
}

void _destroyButton(Button* button, void**) {
    button->destroy(); // deallocate any memory the button used
    free(button);
}

void destroyScreen(Screen* screen) {
    screen->destroyLists();
    screen->freeCompParams();
    delete screen;
}