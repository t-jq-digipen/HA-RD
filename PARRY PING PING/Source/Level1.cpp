//---------------------------------------------------------
// file:	Level1.cpp
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Implementation of Level 1 functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "player.h"
#include "Enemies.h"
#include "Level1.h"
#include "Boss1.h"
#include "GameStateManager.h"

AEVec2 dummy_pos;
f32 dummy_rad;

Timer parryTimer;

int counter{};

int dummy_check;

void Level1_Load()
{
	Load_Player();

	// Meshes
	Load_Mesh(20);
	std::cout << "Level 1 Loaded\n";
}

void Level1_Initialize()
{
	std::cout << "Level 1 Initialized\n";


	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	parryTimer = Create_Timer(1.f);

	// Boss
	Init_Boss1();

	// Dummy enemy
	dummy_pos = { 300.f, 200.f };
	dummy_rad = 25.f;
	dummy_check = 0;
}

void Level1_Update()
{
	// Delta time
	dt = (f32)AEFrameRateControllerGetFrameTime();

	// Basic way to trigger exiting the application
	// when ESCAPE is hit or when the window is closed
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		nextState = GS_QUIT;
	}


	// Boss behavior
	Update_Boss1();

	// Player WASD Movement
	Update_Player();

	dummy_check = Attacked_Melee(dummy_pos);

}

void Level1_Draw()
{
	Update_Camera();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Parryable dummy!
	EnemyAttackPattern(&parryTimer, &counter, dummy_pos, dummy_rad, player.pos, player.radius);
	Loop_Timer(&parryTimer);


	// Boss
	Draw_Boss1();

	// Player
	Draw_Player();

	// Boss HP
	Draw_Boss1HP();

	// UI
	Draw_UI();

}

void Level1_Free()
{

}

void Level1_Unload()
{
	Unload_Mesh();
	Unload_Player();
	std::cout << "level1_unload";
}