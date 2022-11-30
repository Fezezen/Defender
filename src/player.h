#ifndef PLAYER_H
#define PLAYER_H
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

struct p_bullet_s; 
struct human_s;

typedef struct player
{
    SDL_Rect rect;
    struct xy {int x,y;} pos, vel;
    int scrollPos;
    SDL_Texture* texture;
    SDL_RendererFlip flip;

    unsigned int activeBullets;
    struct p_bullet_s** bullets;
    struct human_s* fallingHuman;
} Player;

// Create new player ptr
Player* CreatePlayer(SDL_Renderer*);

// Destroy player
void DestroyPlayer(Player*);

// Draw player's rect
void DrawPlayer(SDL_Renderer*, Player*, int scroll);

// Update player controls and physics
void UpdatePlayer(Player* player); 

// Shoot 
void PlayerAction0(Player* player);

#endif