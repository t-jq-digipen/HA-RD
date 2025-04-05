//---------------------------------------------------------
// File:	SpriteSheet.h
// Author:	AVID PARRY ENJOYERS
// email:	-
//
// Brief:	Spritesheet class declaration
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "mobs.h"

const f32 size = 25.f;
const Color color = SetColor(0xFFFF0000);

// Define a global mobs_list_type object
static mobs_list_type mobs_list;

void Init_Mobs() 
{
    clear_mobs(); // Initialize by clearing the list
}

void Update_Mobs() 
{

    // Check for input to spawn a new mob
    if (AEInputCheckTriggered(AEVK_I)) 
    {
        std::cout<<"I key pressed, attempting to spawn mob..."<<'\n';
        // Spawn a mob at a random position
        f32 x = AERandFloat() * 1500.f; // Map width
        f32 y = AERandFloat() * 1250.f; // Map heght
        std::cout<<"Spawning mob at x: "<<x<<"and y: "<< y<<'\n';
        spawn_mob(x, y, size, color);
    }
    if (AEInputCheckTriggered(AEVK_O))
    {
        clear_mobs();
    }

    // Update logic for each mob (e.g., movement, attacking, etc.)
    //for (int i = 0; i < mobs_list.count; ++i) 
    //{
    //    mobs_type* mob = &mobs_list.mobs[i];
    //    // Example: Move mobs to the right
    //    mob->x += 1.0f;
    //}
}

void Draw_Mobs() 
{
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    for (u32 i = 0; i < mobs_list.count; ++i) 
    {
        mobs_type* mob = &mobs_list.mobs[i];
        DrawCircle(World_To_Screen_x(mob->x), World_To_Screen_y(mob->y), mob->size, mob->color);
    }
}

void spawn_mob(f32 x, f32 y, f32 sizey, Color colory) 
{
    if (mobs_list.count < array_count(mobs_list.mobs)) 
    {
        mobs_type* mob = &mobs_list.mobs[mobs_list.count++];
        mob->x = x;
        mob->y = y;
        mob->size = sizey;
        mob->color = colory;
        mob->isattacking = false;
        mob->attackrange = 0.0f; // Initialize attack range if needed
    }
}

void clear_mobs() 
{
    mobs_list.count = 0;
}