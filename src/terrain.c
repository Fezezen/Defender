#include "terrain.h"
#include "main.h"
#include <stdio.h>

terrain_t* Terrain_CreateTerrain(int points[], int size) 
{
    terrain_t* terrain;
    terrain = malloc(sizeof(struct terrain_s));
    terrain->chunks = size;

    terrain->heights = (int*)malloc(terrain->chunks * sizeof(int));
    memcpy(terrain->heights, points, terrain->chunks * sizeof(int));

    terrain->w_width = terrain->chunks * 100;

    return terrain;
}

void Terrain_DestroyTerrain(terrain_t* terrain) 
{
    free(terrain->heights);
    free(terrain);
}

void Terrain_Draw(SDL_Renderer* renderer, terrain_t* terrain, int scroll) 
{
    SDL_SetRenderDrawColor(renderer,255,165,0,0);

    int nearest = (-(scroll+100)/100);

    for (int i = nearest; i < nearest+10; i++) 
    {
        int p1 = (terrain->chunks + (i%terrain->chunks)) %terrain->chunks, p2 = (terrain->chunks + ((i+1)%terrain->chunks)) %terrain->chunks;
        SDL_RenderDrawLine(renderer, i*100 + scroll, 500-terrain->heights[p1], (i+1)*100 + scroll, 500-terrain->heights[p2]);
    }
}

void Terrain_DrawHUD(SDL_Renderer* renderer, terrain_t* terrain, int scroll) 
{
    SDL_SetRenderDrawColor(renderer,255,165,0,0);
    const int hScreenHeight = SCREEN_WIDTH/2;
    int width = (terrain->chunks*10);
    int half = width/2;
    for (int i = 0; i < terrain->chunks-1; i++) 
    {
        SDL_RenderDrawLine(renderer,hScreenHeight+ i*10 - half, 50-terrain->heights[i]*.1,hScreenHeight+ (i+1)*10 - half, 50-terrain->heights[i+1]*.1);
    }

    width *= 10;
    int scrollX = (hScreenHeight-half)+ ((width + (-(scroll-400)%width)) %width)/10;
    SDL_Rect r;
    r.x = scrollX-15;
    r.y = 10;
    r.w = 30;
    r.h = 60;

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderDrawRect(renderer, &r);
}