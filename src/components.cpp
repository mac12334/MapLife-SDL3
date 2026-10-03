#include "components.hpp"

void makeSurfaceFromColor(SDL_Surface **surf, int width, int height, SDL_Color color) {
    *surf = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
    SDL_Surface* surfP = *surf;
    Uint32 c = SDL_MapRGBA(SDL_GetPixelFormatDetails(surfP->format), NULL, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {0, 0, width, height};
    SDL_FillSurfaceRect(*surf, &rect, c);
    SDL_SetSurfaceBlendMode(*surf, SDL_BLENDMODE_BLEND);
}

void handleTextBox(SDL_Renderer *rend, TTF_Font *font, SDL_Event event, TextBox *txtbox)
{
    if (txtbox == nullptr) return;
    switch (event.type) {
        case SDL_EVENT_TEXT_INPUT:
            txtbox->concatText(event.text.text);
            txtbox->rerender(rend, font);
            break;
        case SDL_EVENT_KEY_DOWN:
            if ((event.key.key != SDLK_BACKSPACE) || txtbox->popped) break;
            txtbox->popChar();
            txtbox->rerender(rend, font);
            txtbox->popped = true;
            break;
        case SDL_EVENT_KEY_UP:
            if (event.key.key == SDLK_BACKSPACE) txtbox->popped = false;
            break;
        default:
            break;
    }
}

void updateTextBoxes(Mouse *mouse, TextBox **currentT, int length, ...) {
    va_list vlist;
    va_start(vlist, length);

    bool focused = false;

    TextBox* item = nullptr;
    for (int i = 0; i < length; i++) {
        item = va_arg(vlist, TextBox*);
        item->update(mouse);
        if (item->focused) {*(currentT) = item; focused = true; }
    }
    if (focused) return;
    *(currentT) = nullptr;
}

void updateTextBox(Mouse *mouse, TextBox **currentT, TextBox *item) {
    item->update(mouse);
    if (item->focused) {*currentT = item;}
    else {*currentT = nullptr;}
}
