#ifndef SCREEN_HPP

#define SCREEN_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "inputs.hpp"
#include "components.hpp"
#include "linkedlist.h"

void renderAndUpdateButton(Button* item, void** parameters);

void renderAndUpdateTextBoxes(TextBox* txtbox, void** parameters);

void _destroyTextBox(TextBox* txtbox, void**);
void _destroyButton(Button* button, void**);

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
        bool deleteLists = false;
        void (*transitionFunction)(void*);
        void* state;

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

        int getButtonListLength() {
            return Linked::getLength(buttonList);
        }

        Button* getButtonIndex(int i) {
            return Linked::getIndex(buttonList, i, (Button*)nullptr);
        }

        int getTextboxListLength() {
            return Linked::getLength(textboxList);
        }

        TextBox* getTextboxIndex(int i) {
            return Linked::getIndex(textboxList, i, (TextBox*)nullptr);
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

            if (this->deleteLists) {
                this->destroyLists();
                this->transitionFunction(state);
                this->transitionFunction = nullptr;
                this->deleteLists = false;
            }
        }

        void freeCompParams() {
            free(componentParameters);
        }

        // destroys all the items allocated for the screen
        void destroyLists() {
            Linked::doWhile(buttonList, _destroyButton);
            Linked::doWhile(textboxList, _destroyTextBox);
            Linked::destroyList(&buttonList);
            Linked::destroyList(&textboxList);
        }
};

// deallocates all memory allocated for the screen
void destroyScreen(Screen* screen);

#endif