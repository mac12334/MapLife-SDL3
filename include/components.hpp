#ifndef COMPONENTS_HPP

#define COMPONENTS_HPP

#include <stdlib.h>
#include "inputs.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef unsigned int uint;

/*
intended for making buttons from colors

@param surf -> a null pointer to an SDL surface to be made into the desired surface
@param width -> the width of the surface you want
@param height -> the height of the surface you want
@param color -> the color you want your surface to be, defaults to black
*/
void makeSurfaceFromColor(SDL_Surface** surf, int width, int height, SDL_Color color = SDL_Color {0, 0, 0, 0});

enum ButtonInitMode {
    BUTTON_SURFACE_INIT,
    BUTTON_TEXTURE_INIT
};

/*
Button is a class that allows you to get button inputs
To run button effectively you need to call both the Button::update method
and the Button::render method

There are two possible constructors, 
- one -> where you are able to initialize the button effectively.
that constructor, the safer constructor, will initialize all vital attributes for the instance of Button.
- The second -> is an empty constructor for quick list initializations. With this method you need to call
Button::defineButton which takes in all the same inputs as the safer constructor and then initializes the button.
*/
class Button {
    public:
        SDL_FRect rect; // rect for bounding texture and collision/button presses
        SDL_FRect textRect; // rect for text texture

        /*
        One of the recommended button constructors for for the button class
        Do not use in cases of lists or quick initializations
        @param texture -> the texture pointer to the desired background texture
        @param destinationRect -> the portion of the screen that will hold the button
        @param sourceRect -> the portion of the texture the button will be made from
        @warning DO NOT DESTROY THE TEXTURE UNTIL THE END OF THE PROGRAM
        */
        Button(SDL_Texture* texture, SDL_FRect destinationRect, SDL_FRect sourceRect = {0, 0, 0, 0}) {
            this->backgroundTexture = texture;
            this->rect = destinationRect;
            this->textureSourceRect = sourceRect;
            this->buttonInitMode = BUTTON_TEXTURE_INIT;
        }

        /*
        One of the recommended button constructor for the buttton class
        Do not use in cases of lists or quick initialization.
        @param rend -> the window renderer
        @param surf -> the surface you want to use for the background of the button
        @param x -> the x position of the window
        @param y -> the y position of the window

        surf is freed in this method you do not need to free the surface after using this method.
        If you still wanted to use that surface you can use the SDL_SurfaceCopy function to get
        a copy of the surface passed into this method YOU MUST USE SDL_SurfaceCopy BEFORE CALLING
        THIS METHOD IF YOU STILL WANT THE SURFACE USED.
        */
        Button(SDL_Renderer* rend, SDL_Surface* surf, int x, int y) {
            this->rect = {(float)x, (float)y, (float)surf->w, (float)surf->h};
            this->backgroundTexture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_DestroySurface(surf);
            this->buttonInitMode = BUTTON_SURFACE_INIT;
        }

        /*
        Empty constructor for making lists of buttons, recommended to use the Button::defineButton method to initialize
        each button properly in a for loop. A button requires a background texture, x, y, width, and height.
        @warning You need to define the background texture and the background rect
        use Button::defineButton(); to define the button if you are using the default constructor.
        */
        Button() {}

        /*
        Use this method to define the buttons texture and rectangle before using the button
        IF you have NOT already used the non-default constructor.

        @param texture -> the texture pointer to the desired background texture
        @param destinationRect -> the portion of the screen that will hold the button
        @param sourceRect -> the portion of the texture the button will be made from
        @warning DO NOT DESTROY THE TEXTURE UNTIL THE END OF THE PROGRAM
        */
        void defineButton(SDL_Texture* texture, SDL_FRect destinationRect, SDL_FRect sourceRect = {0, 0, 0, 0}) {
            this->backgroundTexture = texture;
            this->rect = destinationRect;
            this->textureSourceRect = sourceRect;

            this->buttonInitMode = BUTTON_TEXTURE_INIT;
        }

        /*
        Use this method to define the buttons texture and rectangle before using the button
        IF you have NOT already used the non-default constructor.

        @param rend -> the rendering target
        @param surf -> the background image you want to use
        @param x -> the x position of the rendering target you want the button
        @param y -> the y position of hte rendering target you want the button
        */
        void defineButton(SDL_Renderer* rend, SDL_Surface* surf, int x, int y) {
            this->rect = {(float)x, (float)y, (float)surf->w, (float)surf->h};
            this->backgroundTexture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_DestroySurface(surf);

            this->buttonInitMode = BUTTON_SURFACE_INIT;
        }

        bool getPressed() {
            return this->pressed;
        }

        /*
        Render text into the text texture CALL ONCE!!

        @param rend -> the render target where the texture is associated with
        @param font -> a TTF font used to describe how to use convert the text into a texture
        @param text -> the text you want to display on the button to previde guidance
        @param textColor -> the color of the text, defaults to black
        */
        void renderText(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color textColor = {0, 0, 0, 255}) {
            SDL_Surface* surf = TTF_RenderText_Solid(font, text, strlen(text), textColor);
            int x = this->rect.x + ((this->rect.w/2) - (surf->w/2));
            int y = this->rect.y + ((this->rect.h/2) - (surf->h/2));
            this->textRect = {(float)x, (float)y, (float)surf->w, (float)surf->h};

            this->textTexture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_DestroySurface(surf);
        }

        /*
        checks if the user clicked the button, if the button was clicked then return true
        this does not allow for continuous button pressing.

        @param mouse -> pointer to the mouse struct that is updated each event poll.

        @return if the button was pressed or not. if the button is held down it will only be true for the first frame of holding.
        The button will return true if you stop holding and click the button again.
        */
        bool update(const Mouse* mouse) {
            bool inBoundX = (this->rect.x < mouse->x) && (mouse->x < (this->rect.w + this->rect.x));
            bool inBoundY = (this->rect.y < mouse->y) && (mouse->y < (this->rect.h + this->rect.y));

            if (!mouse->left && this->pressed) this->pressed = false;
            if (!(inBoundX && inBoundY && mouse->left) || this->pressed) return false;

            this->pressed = true;
            return true;
        }

        /*
        renders the button onto the desired rendering target.

        @param rend -> The SDL rendering target
        @param focused -> a boolean value that when true draws a rectangle around the button.
        Focused is intended for multiple selection buttons. defaults to false
        */
        void render(SDL_Renderer* rend, bool focused = false) {
            SDL_RenderTexture(rend, this->backgroundTexture, (this->textureSourceRect.w == 0) ? NULL : &this->textureSourceRect, &this->rect);

            if (this->textTexture) SDL_RenderTexture(rend, this->textTexture, NULL, &this->textRect);
            if (focused) {
                SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
                SDL_RenderRect(rend, &this->rect);
            }
        }

        /*
        The deconstructor assumes that the background texture has been initialized and does not eqaul
        NULL
        */
        void destroy() {
            if (this->buttonInitMode == BUTTON_SURFACE_INIT) SDL_DestroyTexture(this->backgroundTexture);
            if (this->textTexture != NULL) SDL_DestroyTexture(this->textTexture);
        }
    private:
        bool pressed = false; // ensures the button function does not happen multiple times
        SDL_Texture* backgroundTexture; // the texture that is used for the back of the button
        SDL_Texture* textTexture = NULL; // the texture that is for the text if text is required

        SDL_FRect textureSourceRect = {0, 0, 0, 0};

        uint buttonInitMode;
};

/*
TextBox is a class that allows a user to write inputs into a box and then get the value when needed
*/
class TextBox {
    public:
        SDL_FRect textboxRect; // the rectangle enclosing the text being used
        SDL_FRect textureRect; // the destination rectangle for the text texture.
        bool popped = false; // have you deleted a character
        bool focused = false; // is this text box being used right now

        /*
        The recommended constructor of TextBox.
        Do not use in cases of list initializations or quick initializations

        @param rend -> The rendering target where this textbox will be renderered
        @param font -> the font that the user will be writing in
        @param x -> the x position of the textbox
        @param y -> the y position of the textbox
        @param maxCharLength -> the maximum character length you permit for the user, defaults to 10
        @param minBoxWidth -> the minimum box width that ensures the text box still has
        area even when there is no text in the textbox, defaults to 100.
        @param ID_Text -> the text you want your textbox to be called. this text is displayed before the
        textbox to add clarity when using if NULL then there is no ID, defaults to NULL.
        */
        TextBox(SDL_Renderer* rend, TTF_Font* font, int x, int y, int maxCharLength = 10, int minBoxWidth = 100, const char* ID_Text = NULL) {
            this->defineTextBox(rend, font, x, y, maxCharLength, minBoundingWidth, ID_Text);
        }

        /*
        An empty constructor for Textbox used for list initializations use the TextBox::defineTextBox method to
        properly initialize the TextBox instance before practical use.
        A TextBox requires a minimum box width, maximum character length, x, y, and the text texture.
        This is all done by using the TextBox::defineTextBox method.
        */
        TextBox() {}

        /*
        this method defines key features like the position of the textbox, the maximum character length,
        the minimum bounding width, and the text texture to display on the screen. This method is used only
        when you have used the default constructor.

        The recommoneded constructor is Button::Button(SDL_Renderer*, TTF_Font*, int, int, int, int);

        @param rend -> The rendering target where this textbox will be renderered
        @param font -> the font that the user will be writing in
        @param x -> the x position of the textbox
        @param y -> the y position of the textbox
        @param maxCharLength -> the maximum character length you permit for the user, defaults to 10
        @param minBoxWidth -> the minimum box width that ensures the text box still has
        area even when there is no text in the textbox, defaults to 100.
        @param IDText -> the text you want your textbox to be called. this text is displayed before the
        textbox to add clarity when using if NULL then there is no ID, defaults to NULL.

        @warning Do not call this method if you have used the recommended constructor, Do not call this method
        multiple times.
        */
        void defineTextBox(SDL_Renderer* rend, TTF_Font* font, int x, int y, int maxCharLength = 10, int minBoxWidth = 100, const char* IDText = NULL) {
            this->maxCharLength = maxCharLength;
            this->text = (char*)malloc(sizeof(char) * maxCharLength);
            this->text[0] = ' ';
            this->minBoundingWidth = minBoxWidth;
            if (IDText != NULL) {
                SDL_Surface* surf = TTF_RenderText_Solid(font, IDText, strlen(IDText), SDL_Color {0, 0, 0, 255});
                this->ID_Rect = {0, 0, (float)surf->w, (float)surf->h};
                this->IDTexture = SDL_CreateTextureFromSurface(rend, surf);
                SDL_DestroySurface(surf);
            }
            this->textboxSurf = TTF_RenderText_Solid(font, this->text, strlen(this->text), SDL_Color {0, 0, 0, 255});

            this->calculateRectDim(x, y, this->textboxSurf->w, this->textboxSurf->h, IDText != NULL);

            this->textboxTexture = SDL_CreateTextureFromSurface(rend, this->textboxSurf);

            SDL_DestroySurface(this->textboxSurf);
        }

        /*
        this method checks if you have clicked on the textbox.
        this method changes the attribute 'focused' to true or false depending on whether or not you are
        click on the textbox. If you click on the textbox 'focused' is set to true, if you click off of
        the textbox 'focused' is set to false.

        @param mouse -> the mouse struct that is updated everyframe, used to get mouse data.
        */
        void update(Mouse* mouse) {
            bool xBound = (this->textboxRect.x < mouse->x) && (mouse->x < (this->textboxRect.x + this->textboxRect.w));
            bool yBound = (this->textboxRect.y < mouse->y) && (mouse->y < (this->textboxRect.y + this->textboxRect.h));

            if ((xBound && yBound) && mouse->left) this->focused = true;
            else if (!(xBound && yBound) && mouse->left) this->focused = false;
        }

        /*
        used internally to get the bounding rect and texture rect to the correct size and position.

        @param x -> the x position of the textbox
        @param y -> the y position of the textbox
        @param width -> the width of the text texture
        @param height -> the height of the text texture
        @param id -> boolean value that indicates if you have an ID
        */
        void calculateRectDim(int x, int y, int width, int height, bool id) {
            int boundingWidth = ((width + 10) < this->minBoundingWidth) ? this->minBoundingWidth : width + 10;
            if (!id) {
                this->textboxRect = {(float)x, (float)y, (float)boundingWidth, (float)height + 10};
                this->textureRect = {(float)x + 5, (float)y + 5, (float)width, (float)height};
            } else {
                this->ID_Rect.x = x; this->ID_Rect.y = y;
                this->textboxRect = {x + this->ID_Rect.w + 5, (float)y - 2, (float)boundingWidth, (float)height + 5};
                this->textureRect = {x + this->ID_Rect.w + 10, (float)y, (float)width, (float)height};
            }
        }

        /*
        After changing any text value always call this method, it updates the
        rectangle width and heights, updates the text texture.

        @param rend -> the rendering target of the textbox used to store the texture
        @param font -> the font that the user writes in
        */
        void rerender(SDL_Renderer* rend, TTF_Font* font) {
            SDL_DestroyTexture(this->textboxTexture);
            this->textboxSurf = TTF_RenderText_Solid(font, this->text, strlen(this->text), SDL_Color {0, 0, 0, 255});

            this->calculateRectDim((this->IDTexture == NULL) ? this->textboxRect.x : this->ID_Rect.x, (this->IDTexture == NULL) ? this->textboxRect.y : this->ID_Rect.y, this->textboxSurf->w, this->textboxSurf->h, this->IDTexture != NULL);
            this->textboxTexture = SDL_CreateTextureFromSurface(rend, this->textboxSurf);

            SDL_DestroySurface(this->textboxSurf);
        }

        /*
        This method concatonates(adds) a second string to the textboxes text.
        The string will not concatenate if the length of the two strings combined
        is equal to or greater than the maximum character length.
        If the first character of the string is a space then the entire string will
        be rewritten.

        @param s2 -> the second c-style string to be concatenated.
        */
        void concatText(const char* s2) {
            size_t otherLength = strlen(s2);
            if ((this->textLength() + otherLength) >= this->maxCharLength) return;
            
            // check if the first character is = to ' ' do special function
            if (this->text[0] == ' ') this->text[0] = '\0';
            strcat(this->text, s2);
        }

        /*
        removes the last character from the string, except if the last character is
        the first character in the string. In which case the first character will be set to ' '.
        */
        void popChar() {
            if (this->textLength() > 1) {
                this->text[this->textLength() - 1] = '\0';
            } else {
                this->text[0] = ' ';
            }
        }

        /*
        this method gives you the length of the text upto the null character.

        @return the length of the text upto the null character
        */
        size_t textLength() {
            return strlen(this->text);
        }

        /*
        copies the text onto the destination pointer. The length of the destination pointer
        must be equal to or greater than the text length keep the program safe.

        @param dst -> a c-style string which is then given the content of the textbox text
        */
        void getText(char* dst) {
            strcpy(dst, this->text);
        }

        /*
        prints the value of the textbox text onto the console
        */
        void print() {
            printf("%s\n", this->text);
        }

        /*
        renders the textbox on the screen. always renders the background color
        in the bounding rectangle, and the text to the screen. The bounding rectangle outline
        is only drawn to the screen when the textbox is being focused.

        @param rend -> the rendering target for draw the textbox onto the screen
        */
        void render(SDL_Renderer* rend) {
            if (this->IDTexture != NULL) {
                SDL_RenderTexture(rend, this->IDTexture, NULL, &this->ID_Rect);
            }

            SDL_SetRenderDrawColor(rend, 220, 220, 220, 255);
            SDL_RenderFillRect(rend, &this->textboxRect);

            if (this->focused) {
                SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
                SDL_RenderRect(rend, &this->textboxRect);
            }

            SDL_RenderTexture(rend, this->textboxTexture, NULL, &this->textureRect);
        }

        /*
        deallocates both the text pointer and the textboxTexture. and assumes both have already
        been initialized and don't equal NULL or nullptr.
        */
        void destroy() {
            free(this->text); // free's the text pointer to release hold of memory

            SDL_DestroyTexture(textboxTexture);

            if (this->IDTexture != NULL) SDL_DestroyTexture(this->IDTexture);
        }
    private:
        SDL_Texture* textboxTexture; // holds the text texture of the textbox
        SDL_Surface* textboxSurf; // because the text changes constantly this allows for less repeatability
        SDL_Texture* IDTexture = NULL;
        SDL_FRect ID_Rect;
        char* text = nullptr; // can be any length
        int minBoundingWidth; // minimum width of the bounding rectangle.
        int maxCharLength; // maximum length of the text string
};

void handleTextBox(SDL_Renderer*, TTF_Font*, SDL_Event, TextBox*);
void updateTextBoxes(Mouse*, TextBox**, int, ...);
void updateTextBox(Mouse*, TextBox**, TextBox*);

#endif