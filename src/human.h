#ifndef HUMAN_H
#define HUMAN_H

typedef struct human_s {
    float x,y;
    int roam ; // 0 is not roaming -1 or 1 is dir
    int lastRoamCheck;
    int pickedUp; // getting picked up?
    int falling;
    int killed;// bool
} human_t;

human_t* CreateHuman();
void DestroyHuman(human_t* human);

void UpdateHuman(human_t* human);

#endif