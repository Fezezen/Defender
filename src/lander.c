#include "lander.h"
#include "main.h"
#include <stdio.h>
#include "human.h"

#include <time.h>
#include <stdlib.h>

#define LANDER_FALL_RATE .3
#define LANDER_SPEED 2
#define LANDER_ABDUCT_RANGE 200
#define LANDER_RISE_SPEED 3

lander_t* CreateLander(SDL_Renderer* renderer) {
    lander_t* lander;
    lander = malloc(sizeof(lander_t));

    lander->x = 0;
    lander->y = 0;
    lander->kill = 0;
    
    SDL_Surface* surface = SDL_LoadBMP("content/eyelander.bmp");

    if (!surface) {
        REPORT_ERR("Could not load image");
    } else {
        lander->texture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    SDL_FreeSurface(surface);

    lander->rect.x = 0;
    lander->rect.y = 0;
    lander->rect.w = 32;
    lander->rect.h = 32;

    lander->landVariation = .8f + ((float)(rand()%10))/7.0f;

    lander->state = 0;
    lander->roamdir = rand()%2;

    lander->target = NULL;
}

void DestroyLander(lander_t* obj) 
{
    if (obj->target && obj->state == lander_capture) {
        obj->target->pickedUp = 0;
        obj->target->falling = 1;
    }

    SDL_DestroyTexture(obj->texture);

    free(obj);
}

void UpdateLander(lander_t* obj)
{
    int w = GetWorldWidth();

    switch (obj->state)
    {
    case lander_roam:
        obj->x += LANDER_SPEED * obj->landVariation * (obj->roamdir ? 1 : -1);
        // limit from going off screen
        obj->x = (float)((w + (((int)obj->x)%w)) %w);
        //printf("%f\n", obj->x);
        
        if (obj->y < SCREEN_HEIGHT-250)
            obj->y+= LANDER_FALL_RATE * obj->landVariation;
        else {
            int dis;
            human_t* nearHuman = GetNearestHuman((int)obj->x, &dis);

            if (nearHuman && dis <= LANDER_ABDUCT_RANGE && rand()%50 == 0) { // random chance 
                obj->state = lander_abduct;
                obj->target = nearHuman;
            }
        }

        break;
    case lander_abduct:
        if (!obj->target->killed && !obj->target->pickedUp) {
            float dx = (obj->target->x - obj->x);
            float dy = (obj->target->y-25 - obj->y);
            float length = sqrtf(dx*dx + dy*dy);
            dx /= length;
            dy /= length;

            obj->x += dx;
            obj->y += dy;

            if (length < 10.0f) {
                obj->target->pickedUp = 1;
                obj->state = lander_capture;
            }
        } else {
            obj->state = lander_roam;
        }
        break;
    case lander_capture:
        obj->y -= LANDER_RISE_SPEED;
        obj->target->y -= LANDER_RISE_SPEED;

        if (obj->y <= 50.0f) {  
            obj->target->pickedUp = 0;
            obj->target->falling = 1;
            obj->state = lander_roam;
        }

        break;
    default:
        break;
    }

    obj->y = CLAMP(obj->y,50,SCREEN_HEIGHT-100);
}

void DrawLander(SDL_Renderer* renderer, lander_t* obj, int scroll) 
{
    int w = GetWorldWidth();
    obj->rect.x = (w + (((int)obj->x-obj->rect.w/2+scroll)%w)) %w;

    obj->rect.y = (int)obj->y;

    SDL_RenderCopy(renderer, obj->texture, NULL, &obj->rect);
}
