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
};

extern int levelState;

void Draw_UI();

void Update_UI(int ammo, float hp, float parry_timer);

// will neaten when more player parameters are finalized
void Init_UI(int ammo, int maxammo, float hp, float maxhp);

void Draw_Boss1HP();

void Draw_PauseMenu();