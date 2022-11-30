#include "main.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include "player.h"
#include "terrain.h"
#include "lander.h"
#include "p_bullet.h"
#include "human.h"

#include <time.h>
#include <stdlib.h>

#define MAX_ENEMIES 10
#define MAX_HUMANS 6

static int ScrollX;
static int tick;
Player* player;
terrain_t* terrain;

enemy_t enemies[MAX_ENEMIES];
human_t** humans;

int main(int argc, char *argv[]) 
{
    ScrollX = 0;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    srand(time(NULL));

    tick = 0;
    int lastSpawned = 0;
    int spawnRate = 60;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        REPORT_ERR("SDL could not initalize!");
        return 0;
    }

    window = SDL_CreateWindow("Defender", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        REPORT_ERR("SDL could not create window!");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        REPORT_ERR("SDL could not create renderer!");
    }

    // load texture
    SDL_Surface* dotSurface = SDL_LoadBMP("content/dot.bmp");
    SDL_Texture* dotTexture = SDL_CreateTextureFromSurface(renderer,dotSurface);
    SDL_FreeSurface(dotSurface);

    SDL_Surface* humanSurface = SDL_LoadBMP("content/human.bmp");
    SDL_Texture* humanTexture = SDL_CreateTextureFromSurface(renderer,humanSurface);
    SDL_FreeSurface(humanSurface);

    // init
    player = CreatePlayer(renderer);
    player->rect.y = 100;

    int terrainPoints[] = {0, 100, -10, 0, 0, 200, 100, 0, 0, -100, 
    0, 0, 50, 0, 100, 200, 100, 0, 0, 50, 100, 50, 0, 0, -10, -20, -10, 0,
    0, 50, 50, -10, -10, 0, 20, 0
    };
    terrain = Terrain_CreateTerrain(terrainPoints, arr_size(terrainPoints));

    // init enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i] = (enemy_t){0, NULL};
    }

    // init humans
    humans = malloc(MAX_HUMANS * sizeof(struct human_s));
    for (int i = 0; i < MAX_HUMANS; i++) {
        humans[i] = CreateHuman();
        humans[i]->x = rand()%terrain->w_width;
        humans[i]->y = SCREEN_HEIGHT-110;
    }

    int close = 0;
    while(!close) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) 
        {
            switch(event.type) {
            case SDL_QUIT:
                close = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    close = 1;
                    break;
                case SDL_SCANCODE_SPACE:
                    if (!event.key.repeat)
                        PlayerAction0(player);
                    break;
                default:
                    break;
                }
            default:
                break;
            }
        } 

        // update
        tick++;

        UpdatePlayer(player);

        for (int i = 0; i < MAX_ENEMIES; i++) {
            switch (enemies[i].type)
            {
            case lander:
                UpdateLander(enemies[i].enemy.lander);
                if (enemies[i].enemy.lander->kill) {
                    DestroyLander(enemies[i].enemy.lander);
                    enemies[i] = (enemy_t){0, NULL};
                }
                break;
            default:
                break;
            }
        }

        int deadHumans = 0;
        for (int i = 0; i < MAX_HUMANS; i++) {
            if (!humans[i]->killed) {
                UpdateHuman(humans[i]);
            } else {
                deadHumans++;
            }
        }

        if (deadHumans == MAX_HUMANS)
            close = 1;

        // spawn landers
        if (lastSpawned < tick) {
            lastSpawned = tick + (3+rand()%6)*spawnRate;
            if (spawnRate > 10 && lastSpawned%100) {
                spawnRate-=5;
            }

            // find free spawn
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].type == 0) { // empty slot
                    enemies[i].enemy.lander = CreateLander(renderer);
                    enemies[i].enemy.lander->x = rand()%terrain->w_width;
                    enemies[i].enemy.lander->y = 50;
                    enemies[i].type = lander;
                    break;
                }
            }
        }

        // draw
        SDL_RenderClear(renderer);
        
        Terrain_Draw(renderer, terrain, ScrollX);
        DrawPlayer(renderer, player, ScrollX);

        Terrain_DrawHUD(renderer, terrain, ScrollX);

        SDL_Rect r;
        r.w = 10;
        r.h = 10;

        for (int i = 0; i < MAX_ENEMIES; i++) {
            switch (enemies[i].type)
            {
            case lander:
                DrawLander(renderer,enemies[i].enemy.lander, ScrollX);

                SDL_SetRenderDrawColor(renderer,255,0,0,255);
                int x = SCREEN_WIDTH/2 + enemies[i].enemy.lander->x/10 - terrain->w_width/20;

                r.x = x-5;
                r.y = 10+enemies[i].enemy.lander->y/10;
                SDL_RenderCopy(renderer, dotTexture, NULL, &r);
                break;
            default:
                break;
            }
        }

        for (int i = 0; i < MAX_HUMANS; i++) {
            if (!humans[i]->killed) {
                r.w = 22;
                r.h = 25;

                r.x = (terrain->w_width + (((int)humans[i]->x- 11 + ScrollX)%terrain->w_width)) %terrain->w_width ;
                r.y = humans[i]->y;
                SDL_RenderCopy(renderer, humanTexture, NULL, &r);

                r.w = 5;
                r.h = 5;

                SDL_SetRenderDrawColor(renderer,0,255,0,255);
                int x = SCREEN_WIDTH/2 + humans[i]->x/10 - terrain->w_width/20;
                r.x = x-5;
                r.y = 10+humans[i]->y/10;
                SDL_RenderCopy(renderer, dotTexture, NULL, &r);
            }
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/60); // ~60 fps
    }

    // clean up
    SDL_DestroyTexture(dotTexture);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        switch (enemies[i].type)
        {
        case lander:
            DestroyLander(enemies[i].enemy.lander);
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < MAX_HUMANS; i++) {
        DestroyHuman(humans[i]);
    }
    free(humans);

    DestroyPlayer(player);
    Terrain_DestroyTerrain(terrain);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void SetScrollX(int x) {
    ScrollX = x;
}

int GetWorldWidth() {
    return terrain->w_width;
}

int GetTick() {
    return tick;
}

struct enemy_s* GetP_BulletIntersection(struct p_bullet_s* bullet) 
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        switch (enemies[i].type)
        {
        case lander:
            // for some reason SDL_IntersectRect doesn't work
            if (SDL_HasIntersection(&enemies[i].enemy.lander->rect,&bullet->rect)) {
                return &enemies[i];
            }
            break;
        default:
            break;
        }
    }

    return NULL;
}

human_t* GetNearestHuman(int x, int* dist) 
{
    int close = 100000;
    human_t* nearest = NULL;

    for (int i = 0; i < MAX_HUMANS; i++) {
        if (humans[i]->killed == 0 && humans[i]->falling == 0 && humans[i]->pickedUp == 0) {
            int dis = abs(x-(int)humans[i]->x);
            if (dis < close) {
                close = dis;
                nearest = humans[i];
            }
        }
    }

    if (dist != NULL)
        *dist = close;

    return nearest;
}

human_t* GetNearestFallingHuman(int x, int* dist) 
{
    int close = 100000;
    human_t* nearest = NULL;

    for (int i = 0; i < MAX_HUMANS; i++) {
        if (humans[i]->killed == 0 && humans[i]->falling) {
            int dis = abs(x-(int)humans[i]->x);
            if (dis < close) {
                close = dis;
                nearest = humans[i];
            }
        }
    }

    if (dist != NULL)
        *dist = close;

    return nearest;
}