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



void Draw_UI();

void Update_UI(int ammo, float hp, float parry_timer);

// will neaten when more player parameters are finalized
void Init_UI(int ammo, int maxammo, float hp, float maxhp);