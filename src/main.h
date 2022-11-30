#ifndef MAIN_H
#define MAIN_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define arr_size(arr)(sizeof(arr)/sizeof(arr[0]))

#define REPORT_ERR(msg)(fprintf(stdout, "%s SDL_Error: %s\n", SDL_GetError()))

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

void SetScrollX(int x);
int GetWorldWidth();
int GetTick();

struct lander_s;
struct p_bullet_s;

typedef enum enemey_type_e
{
    none = 0,
    lander = 1
} enemy_type;

typedef union {
    struct lander_s* lander;
} enemy_u;

typedef struct enemy_s 
{
    enemy_type type;
    enemy_u enemy;
} enemy_t;

struct enemy_s* GetP_BulletIntersection(struct p_bullet_s* bullet); // test bullet intersction

struct human_s;
// get closest non-dead and non-falling human to X (optionally returns dis)
struct human_s* GetNearestHuman(int x, int* dis); // don't care about Y
// get closest falling human
struct human_s* GetNearestFallingHuman(int x, int* dis); // don't care about Y

#endif