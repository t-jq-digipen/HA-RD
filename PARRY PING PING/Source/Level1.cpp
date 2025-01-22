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

	// Dummy enemy
	dummy_pos = { 0.f, 200.f };
	dummy_rad = 25.f;
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
	Update_Camera();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Dummy
	DrawCircle(World_To_Screen_x(dummy_pos.x), World_To_Screen_y(dummy_pos.y), dummy_rad, SetColor(0xFFFF6961));

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
	Unload_Mesh();
	Unload_Player();
	std::cout << "level1_unload";
}