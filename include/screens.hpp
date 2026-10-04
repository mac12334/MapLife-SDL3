#ifndef SCREENS_HPP

#define SCREENS_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "inputs.hpp"
#include "components.hpp"
#include "linkedlist.h"

void renderAndUpdateButton(Button* item, void** parameters) {
    if (parameters == nullptr) { // this function requires paramters to run
        printf("no parameters\n");
        return;
    }

    // grabs the needed paramters from the list
    SDL_Renderer* renderer = (SDL_Renderer*)(*(parameters));
    Mouse* mouse = (Mouse*)(*(parameters + 1));

    // updates and renders the button
    if (item->update(mouse)) printf("hello world\n");
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
    delete txtbox; // delete the textbox because we used new
}
void _destroyButton(Button* button, void**) {
    button->destroy(); // deallocate any memory the button used
    delete button; // delete the button because we used new
}

class Screen {
    // needed parameters for rendering and updating the components
    SDL_Renderer* renderer;
    Mouse* mouse;
    TextBox* currentTextBox = nullptr;
    // component lists
    Linked::LinkedList<Button*>* buttonList = nullptr;
    Linked::LinkedList<TextBox*>* textboxList = nullptr;
    // parameter lists
    void** componentParameters = nullptr;

    // initialize parameter lists with correct values
    void initParams() {
        // textbox parameters
        *(SDL_Renderer**)(componentParameters) = renderer;
        *(Mouse**)(componentParameters + 1) = mouse;
        *(TextBox***)(componentParameters + 2) = &currentTextBox;
    }

    public:
        Screen(SDL_Renderer* rend, Mouse* m) {
            renderer = rend;
            mouse = m;

            size_t tbplength = sizeof(SDL_Renderer*) + sizeof(Mouse*) + sizeof(TextBox**); // the length of the textbox parameters
            // allocate the memory for parameter lists
            componentParameters = (void**)malloc(tbplength);

            initParams(); // initalize the parameter list
        }

        // required to write to the current textbox
        TextBox* getCurrentTextBox() {
            return currentTextBox;
        }

        // adds a button to the button list
        void addButton(Button* button) {
            Linked::addItem(&buttonList, button);
        }

        // adds a textbox to the textbox list
        void addTextBox(TextBox*  txtBox) {
            Linked::addItem(&textboxList, txtBox);
        }

        // ran every iteration of the app
        void tick() {
            Linked::doWhile(buttonList, renderAndUpdateButton, componentParameters);
            Linked::doWhile(textboxList, renderAndUpdateTextBoxes, componentParameters);
        }

        // destroys all the items allocated for the screen
        void destroyLists() {
            Linked::doWhile(buttonList, _destroyButton);
            Linked::doWhile(textboxList, _destroyTextBox);
            Linked::destroyList(&buttonList);
            Linked::destroyList(&textboxList);
            free(componentParameters);
        }
};

// deallocates all memory allocated for the screen
void destroyScreen(Screen* screen) {
    screen->destroyLists();
    delete screen;
}

#endif