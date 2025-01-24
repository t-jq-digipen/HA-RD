//---------------------------------------------------------
// file:	Boss1.h
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Declaration of boss 1 structs and functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include <utils.h>
#include "Camera.h"

typedef struct Boss1 
{
	AEVec2 pos;
	f32 size;
	float current_hp, max_hp;
	bool isattacking;
	Timer attackInterval;
	Color color, hp_color;
	int phase;
} Boss1;

void Init_Boss1();

void Draw_Boss1();

void Update_Boss1();

void Boss1_ShootBullet(const AEVec2& start, const AEVec2& direction);

void Draw_Boss1HP();