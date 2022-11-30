#ifndef P_BULLET_H
#define P_BULLET_H

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

// projectile that the player fires

typedef struct p_bullet_s 
{
    struct p_bxy {int x,y;} pos, vel; 
    SDL_Rect rect;
} p_bullet;

p_bullet* CreateP_Bullet(int px, int py, int vx, int vy);
void DestroyP_Bullet(p_bullet*);

void UpdateP_Bullet(p_bullet*);
void DrawP_Bullet(SDL_Renderer* renderer, p_bullet*, int scroll);

#endif