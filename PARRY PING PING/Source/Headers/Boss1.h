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

enum BOSS_PHASE
{
	PHASE_1,	// shoot once every 3 seconds
	PHASE_2,	// shoot once every 1.5 seconds
	PHASE_3,	// shoot once every 0.5 seconds
	DEFEAT
};

extern Boss1 boss1;

void Init_Boss1();

void Draw_Boss1();

void Update_Boss1();

void Boss1_ShootBullet(const AEVec2& start, const AEVec2& direction);