#ifndef LANDER_H
#define LANDER_H

#include <SDL2/SDL.h>

typedef enum lander_e_states {
    lander_roam, // moves around randomly
    lander_abduct, // goes toward human
    lander_capture // goes to top of screen with human
} lander_state;

struct human_s;

typedef struct lander_s 
{
    float x,y;
    SDL_Texture* texture;
    SDL_Rect rect;
    float landVariation;
    int kill;
    lander_state state;
    unsigned roamdir; // 0 = left, 1 = right
    struct human_s* target;
} lander_t;

lander_t* CreateLander(SDL_Renderer*);
void DestroyLander(lander_t*);

void UpdateLander(lander_t*);
void DrawLander(SDL_Renderer*, lander_t*, int);

#endif