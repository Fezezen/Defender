#include "human.h"
#include "main.h"

#include <stdlib.h>
#include <stdio.h>

#define HUMAN_SPEED 1
#define HUMAN_FALL_RATE 2

human_t* CreateHuman() 
{
    human_t* human;
    human = malloc(sizeof(struct human_s));

    human->x = 0;
    human->y = 0;
    human->roam = 0;
    human->falling = 0;
    human->lastRoamCheck = 0;
    human->killed = 0;
    human->pickedUp = 0;
}

void DestroyHuman(human_t* human) 
{
    free(human);
}

void UpdateHuman(human_t* human) 
{
    if (!human->falling && !human->pickedUp) 
    {
        // decide to walk around/walk
        int tick = GetTick();
        if (human->roam) {
            human->x += HUMAN_SPEED * human->roam;
            int w = GetWorldWidth();
            human->x = (w + (((int)human->x)%w)) %w;

            if (human->lastRoamCheck < tick) {
                int outcome = rand()%3; // 1 in 3 chance to stop roaming
                if (outcome == 0) {
                    human->roam = 0;
                }
                human->lastRoamCheck = tick+100;
            }
        } else {
            if (human->lastRoamCheck < tick) {
                int outcome = rand()%5; // 1 in 5 chance to start roaming
                if (outcome == 0) {
                    human->roam = (rand()%2)? -1 : 1;
                }
                human->lastRoamCheck = tick+100;
            }
        }
    } else {
        if (human->falling) {
            human->y += HUMAN_FALL_RATE;
            human->falling += HUMAN_FALL_RATE;

            if (human->y > SCREEN_HEIGHT-100) {
                if (human->falling > 200) {
                    human->killed = 1;
                } else {
                    human->falling = 0;
                }
            }
        }
    }
}