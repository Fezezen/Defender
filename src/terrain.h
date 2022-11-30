#ifndef TERRAIN_H
#define TERRAIN_H
#include <SDL2/SDL.h>

// the terrain is made of chunks
// each chunk as a height value
// this will make the appearance of hills

typedef struct terrain_s 
{
    int chunks;
    int w_width;
    int* heights;
} terrain_t;

terrain_t* Terrain_CreateTerrain(int points[], int size);
void Terrain_DestroyTerrain(terrain_t*);

void Terrain_Draw(SDL_Renderer*, terrain_t*, int scroll);

// Draws a tiny version
void Terrain_DrawHUD(SDL_Renderer*, terrain_t*, int scroll); 

#endif