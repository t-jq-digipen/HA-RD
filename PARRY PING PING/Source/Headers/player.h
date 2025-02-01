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

typedef enum PlayerState
{
	IDLE,
	MOVING,
	MELEE,
	RANGE,
	HEAVY,
	DODGE,
	HIT,
	DEAD,
	BLOCK,
	BLOCKED,
	PARRY,
	PARRIED,
	MISSED
} PlayerState;

// make player struct
typedef struct Player 
{
	AEVec2 pos;

	AEVec2 vel;
	float speed;

	float hp; //health

	float radius;
	float width, height;
	aabb hitbox;

	int ammo;

	Timer dodgetimer;
	Timer hittimer;
	Timer blocktimer;

	PlayerState state;

	int direction_v;
	int direction_h;

	Color color;
	SpriteSheet* spritesheet;
} Player;



enum Direction_v
{
	DOWN,
	UP,
};

enum Direction_h
{
	RIGHT,
	LEFT,
};

extern Player player;

void Movement();

void Load_Player();

void Init_Player();

void Update_Player();

void Draw_Player();

void Free_Player();

void Unload_Player();

void Attacking();

int Attacked_Melee(AEVec2 target);

bool parrycheck(Player* player, int* counter, Timer* stunnedtimer);

void UpdateBlockParry(Player* player, Timer* blockCDtimer, int* counter,Timer*stunnedtimer);