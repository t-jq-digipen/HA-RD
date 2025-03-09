#pragma once
#include "utils.h"
#include "Camera.h"

#define array_count(x) sizeof(x)/sizeof(*x)

typedef struct mobs_type 
{
    f32 x, y, size, attackrange;
    bool isattacking;
    Color color;
} mobs_type;

typedef struct mobs_list_type 
{
    mobs_type mobs[100];
    u32 count;
} mobs_list_type;

// Declare functions
void Init_Mobs();
void Update_Mobs();
void Draw_Mobs();
void spawn_mob(f32 x, f32 y, f32 size, Color color);
void clear_mobs();