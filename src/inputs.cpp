#include "inputs.hpp"

void _handleMouseButtons(SDL_Event event, Mouse *mouse) {
    switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) mouse->left = true;
            if (event.button.button == SDL_BUTTON_MIDDLE) mouse->center = true;
            if (event.button.button == SDL_BUTTON_RIGHT) mouse->right = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button == SDL_BUTTON_LEFT) mouse->left = false;
            if (event.button.button == SDL_BUTTON_MIDDLE) mouse->center = false;
            if (event.button.button == SDL_BUTTON_RIGHT) mouse->right = false;
            break;
        default:
            break;
    }
}

void updateMouse(SDL_Event event, Mouse *mouse) {
    Uint32 t = event.type;
    if (t != SDL_EVENT_MOUSE_BUTTON_DOWN && t != SDL_EVENT_MOUSE_BUTTON_UP && t != SDL_EVENT_MOUSE_MOTION) return;
    _handleMouseButtons(event, mouse);
    mouse->x = event.motion.x;
    mouse->y = event.motion.y;
}

void _handleKeys(SDL_Event event, bool *keymap, bool value) { // used to make editing this much much easier
    if (event.key.key == SDLK_Q) *(keymap + K_q) = value;
    if (event.key.key == SDLK_W) *(keymap + K_w) = value;
    if (event.key.key == SDLK_E) *(keymap + K_e) = value;
    if (event.key.key == SDLK_R) *(keymap + K_r) = value;
    if (event.key.key == SDLK_T) *(keymap + K_t) = value;
    if (event.key.key == SDLK_Y) *(keymap + K_y) = value;
    if (event.key.key == SDLK_U) *(keymap + K_u) = value;
    if (event.key.key == SDLK_I) *(keymap + K_i) = value;
    if (event.key.key == SDLK_O) *(keymap + K_o) = value;
    if (event.key.key == SDLK_P) *(keymap + K_p) = value;
    if (event.key.key == SDLK_A) *(keymap + K_a) = value;
    if (event.key.key == SDLK_S) *(keymap + K_s) = value;
    if (event.key.key == SDLK_D) *(keymap + K_d) = value;
    if (event.key.key == SDLK_F) *(keymap + K_f) = value;
    if (event.key.key == SDLK_G) *(keymap + K_g) = value;
    if (event.key.key == SDLK_H) *(keymap + K_h) = value;
    if (event.key.key == SDLK_J) *(keymap + K_j) = value;
    if (event.key.key == SDLK_K) *(keymap + K_k) = value;
    if (event.key.key == SDLK_L) *(keymap + K_l) = value;
    if (event.key.key == SDLK_Z) *(keymap + K_z) = value;
    if (event.key.key == SDLK_X) *(keymap + K_x) = value;
    if (event.key.key == SDLK_C) *(keymap + K_c) = value;
    if (event.key.key == SDLK_V) *(keymap + K_v) = value;
    if (event.key.key == SDLK_B) *(keymap + K_b) = value;
    if (event.key.key == SDLK_N) *(keymap + K_n) = value;
    if (event.key.key == SDLK_M) *(keymap + K_m) = value;
}

void updateKeys(SDL_Event event, bool *keymap) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            _handleKeys(event, keymap, true);
            break;
        case SDL_EVENT_KEY_UP:
            _handleKeys(event, keymap, false);
            break;
        default:
            break;
    }
}

void emptyKeys(bool *keymap) {
    for (int x = 0; x < KEY_LIST_LENGTH; x++) {
        keymap[x] = false;
    }
}
