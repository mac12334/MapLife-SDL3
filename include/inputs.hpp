#ifndef INPUTS_H

#define INPUTS_H

#include <SDL3/SDL.h>

#define KEY_LIST_LENGTH 26

struct Mouse {
    bool left, right, center;
    int x, y;
};

enum Keys {
    K_q, K_w, K_e, K_r, K_t, K_y, K_u, K_i, K_o, K_p,
    K_a, K_s, K_d, K_f, K_g, K_h, K_j, K_k, K_l,
    K_z, K_x, K_c, K_v, K_b, K_n, K_m
};

void updateMouse(SDL_Event, Mouse *);
void updateKeys(SDL_Event, bool *);
void emptyKeys(bool *);

#endif