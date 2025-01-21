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
#include "Level1.h"
#include "GameStateManager.h"

AEVec2 dummy_pos;
f32 dummy_rad;

void Level1_Load()
{
	std::cout << "Level 1 Loaded\n";
}

void Level1_Initialize()
{
	std::cout << "Level 1 Initialized\n";

	// Meshes
	InitMesh(20);

	// Player
	Init_Player();

	// Dummy enemy
	dummy_pos = { 0.f, 200.f };
	dummy_rad = 50.f;
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

	// Player WASD Movement
	Update_Player();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Dummy
	DrawCircle(dummy_pos.x, dummy_pos.y, dummy_rad, SetColor(0xFFFF6961));

	// Player
	Draw_Player();
}

void Level1_Draw()
{

}

void Level1_Free()
{

}

void Level1_Unload()
{

}