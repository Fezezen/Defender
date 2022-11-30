#include "player.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include "main.h"
#include "p_bullet.h"
#include "lander.h"
#include "human.h"

#define PLAYER_HOR_ACC 5
#define PLAYER_HOR_FRIC 2
#define PLAYER_MAX_HOR_SPEED 200

#define PLAYER_VER_ACC 10
#define PLAYER_VER_FRIC 5
#define PLAYER_MAX_VER_SPEED 40

#define PLAYER_BULLET_SPEED 35
#define PLAYER_MAX_BULLETS 4

Player* CreatePlayer(SDL_Renderer* renderer) 
{
    Player* newPlayer;
    newPlayer = malloc(sizeof(Player));

    newPlayer->pos.x = 0;
    newPlayer->pos.y = 100;

    newPlayer->vel.x = 0;
    newPlayer->vel.y = 0;

    newPlayer->rect.w = 48;
    newPlayer->rect.h = 25;

    newPlayer->scrollPos = -10;
    newPlayer->flip = SDL_FLIP_NONE;

    SDL_Surface* surface = SDL_LoadBMP("content/ship.bmp");

    if (!surface) {
        REPORT_ERR("Could not load image");
    } else {
        newPlayer->texture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    SDL_FreeSurface(surface);

    newPlayer->activeBullets = 0;
    newPlayer->bullets = malloc(PLAYER_MAX_BULLETS * sizeof(struct p_bullet_s));

    return newPlayer;
}

void DestroyPlayer(Player* plr) 
{
    SDL_DestroyTexture(plr->texture);

    for (unsigned int i = 0; i < 4; i++) 
    {
        free(plr->bullets[i]);
    }

    free(plr);
}

void DrawPlayer(SDL_Renderer* renderer, Player* plr, int scroll) 
{
    SDL_SetRenderDrawColor(renderer,255,255,255,0);

    plr->rect.x = plr->pos.x + (plr->flip == SDL_FLIP_HORIZONTAL ? -plr->rect.w/2 : 0) + scroll;
    plr->rect.y = plr->pos.y;
    SDL_RenderCopyEx(renderer, plr->texture, NULL, &plr->rect, 0, NULL, plr->flip);

    // draw bullets
    for (unsigned int i = 0; i < 4; i++) {
        if (plr->bullets[i]) {
            DrawP_Bullet(renderer, plr->bullets[i],scroll);
        }
    }
}

void UpdatePlayer(Player* player) 
{
    // check keyboard state for inpit
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    
    int acc = 1;
    // accel left and right
    if (keyboardState[SDL_SCANCODE_LEFT]) 
    {
        player->vel.x -= PLAYER_HOR_ACC;
        player->flip = SDL_FLIP_HORIZONTAL;
    } else if (keyboardState[SDL_SCANCODE_RIGHT]) 
    {
        player->vel.x += PLAYER_HOR_ACC;
        player->flip = SDL_FLIP_NONE;
    } else { // not accelerating, apply friction
        acc = 0;
        if (player->vel.x > 0) {
            int result = player->vel.x - PLAYER_HOR_FRIC;
            player->vel.x = (result <= 0) ? 0 : result;
        } else {
            int result = player->vel.x + PLAYER_HOR_FRIC;
            player->vel.x = (result >= 0) ? 0 : result;
        }
    }
    player->vel.x = CLAMP(player->vel.x,-PLAYER_MAX_HOR_SPEED,PLAYER_MAX_HOR_SPEED);

    // up/down
    if (keyboardState[SDL_SCANCODE_UP]) 
    {
        player->vel.y -= PLAYER_VER_ACC;
    } else if (keyboardState[SDL_SCANCODE_DOWN]) 
    {
        player->vel.y += PLAYER_VER_ACC;
    } else { // not accelerating, apply friction
        if (player->vel.y > 0) {
            int result = player->vel.y - PLAYER_VER_FRIC;
            player->vel.y = (result <= 0) ? 0 : result;
        } else {
            int result = player->vel.y + PLAYER_VER_FRIC;
            player->vel.y = (result >= 0) ? 0 : result;
        }
    }
    player->vel.y = CLAMP(player->vel.y,-PLAYER_MAX_VER_SPEED,PLAYER_MAX_VER_SPEED);

    // got lazy, didn't want to change to floats
    player->pos.x += player->vel.x*.1;
    player->pos.y += player->vel.y*.1;

    if (player->pos.y < 100) {
        player->pos.y = 100;
        player->vel.y = 0;
    } else if (player->pos.y > SCREEN_HEIGHT-100) {
        player->pos.y = SCREEN_HEIGHT-100;
        player->vel.y = 0;
    }

    // proper scroll should look forward in the dir the player is moving
    if (acc)
        player->scrollPos = CLAMP(player->scrollPos + (player->flip == SDL_FLIP_NONE ? 7 : -7), -SCREEN_WIDTH*.3f, SCREEN_WIDTH*.3f);
    

    int scroll = -(player->pos.x-SCREEN_WIDTH/2+player->rect.w/2 + player->scrollPos);
    SetScrollX(scroll);

    // update bullets
    for (int i = 3; i >= 0; i--) {
        if (player->bullets[i]) {
            UpdateP_Bullet(player->bullets[i]);

            enemy_t* hitEnemy = GetP_BulletIntersection(player->bullets[i]);

            if (hitEnemy && hitEnemy->type != 0) {
                switch (hitEnemy->type)
                {
                case lander:
                    hitEnemy->enemy.lander->kill = 1;
                    break;
                default:
                    break;
                }
            }

            if ((hitEnemy && hitEnemy->type != 0) || player->bullets[i]->pos.x < -(scroll+100+player->bullets[i]->rect.w) || player->bullets[i]->pos.x > -scroll+(SCREEN_WIDTH+100)+player->bullets[i]->rect.w) { // mark to delete or off screen
                DestroyP_Bullet(player->bullets[i]);
                player->bullets[i] = NULL;
                player->activeBullets--;
            }
        }
    }

    //find closest human and catch falling ones
    if (!player->fallingHuman) {
        int dis;
        int w = GetWorldWidth();
        human_t* nearHuman = GetNearestFallingHuman((w + ((player->pos.x)%w)) %w, &dis);
        if (nearHuman && dis < 50 && abs((int)nearHuman->y-player->pos.y) < 50) {
            nearHuman->pickedUp = 1;
            nearHuman->falling = 0;
            player->fallingHuman = nearHuman;
        }
    } else {
        player->fallingHuman->x = player->pos.x;
        player->fallingHuman->y = player->pos.y;

        if (player->pos.y > SCREEN_HEIGHT-150) {
            player->fallingHuman->y = SCREEN_HEIGHT-110;
            player->fallingHuman->pickedUp = 0;
            player->fallingHuman = NULL;
        }
    }
}

// shoot
void PlayerAction0(Player* player) 
{
    if (player->activeBullets < 4) {
        // find empty spot
        for (unsigned int i = 0; i < 4; i++) {
            if (!player->bullets[i]) {
                int vel = PLAYER_BULLET_SPEED;
                if (player->flip == SDL_FLIP_HORIZONTAL)
                    vel = -vel;
                player->bullets[i] = CreateP_Bullet(player->pos.x,player->pos.y+20, vel, 0);
                player->activeBullets++;
                
                break;
            }
        }
    }
}