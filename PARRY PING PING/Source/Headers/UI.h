//---------------------------------------------------------
// file:	UI.h
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Declaration for all UI elements
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

enum LEVEL_STATE
{
	PLAYING = 0,
	PAUSED,
	GAMEOVER,
	CLEARED
};

extern int levelState;


//1 - draw boss hp bar, 0 - dont draw
void Draw_UI(int DrawBossHP = 1);

void Update_Player_UI(int ammo, float hp, float parry_timer);

// will neaten when more player parameters are finalized
void Init_UI();

void Init_Player_UI(int ammo, int maxammo, float hp, float maxhp);

void Init_Boss_UI(float hp);

void Update_Boss_UI(float hp);

void Update_LevelTimer(float *time_elapsed, float dt);

//void Draw_Boss1HP(float time_elapsed);

void Draw_PauseMenu();

void Draw_GameOverMenu();

void Draw_ClearMenu();

void Draw_LevelTimer(float time_elapsed);