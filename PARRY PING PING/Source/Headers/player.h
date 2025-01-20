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


typedef struct Attack 
{
	Timer timer;
	int combo;
} Attack;

void Movement();

void Init_Player();

void Update_Player();

void Draw_Player();

bool Attacking();