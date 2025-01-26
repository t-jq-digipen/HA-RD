//---------------------------------------------------------
// file:	player.h
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Declaration of player struct and 
//			Movement function
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"
#include "Camera.h"
#include "UI.h"

// make player struct
typedef struct Player 
{
	AEVec2 pos;
	AEVec2 vel;
	float speed;
	float hp; //health
	float radius;
	Timer dodgetimer;
	Color color;
} Player;

extern Player player;

void Movement();

void Load_Player();

void Init_Player();

void Update_Player();

void Draw_Player();

void Free_Player();

void Unload_Player();

bool Attacking();

int Attacked_Melee(AEVec2 target);