#include "p_bullet.h"

p_bullet* CreateP_Bullet(int px, int py, int vx, int vy) 
{
    p_bullet* bullet;
    bullet = malloc(sizeof(p_bullet));

    bullet->pos.x = px;
    bullet->pos.y = py;

    bullet->vel.x = vx;
    bullet->vel.y = vy;

    bullet->rect.w = 20;
    bullet->rect.h = 2;

    return bullet;
}

void DestroyP_Bullet(p_bullet* bullet) 
{
    free(bullet);
}

void UpdateP_Bullet(p_bullet* bullet) 
{
    bullet->pos.x += bullet->vel.x;
    bullet->pos.y += bullet->vel.y;

    bullet->rect.w+=5;
}

void DrawP_Bullet(SDL_Renderer* renderer, p_bullet* bullet, int scroll) 
{
    SDL_SetRenderDrawColor(renderer, 240, 255, 255, 0);

    bullet->rect.x = bullet->pos.x-bullet->rect.w*(bullet->vel.x > 0 ? 1 : 0) + scroll;
    int w = GetWorldWidth();
    bullet->rect.x = (w + (bullet->rect.x%w)) %w;

    bullet->rect.y = bullet->pos.y-bullet->rect.h/2;
    SDL_RenderFillRect(renderer, &bullet->rect);
}