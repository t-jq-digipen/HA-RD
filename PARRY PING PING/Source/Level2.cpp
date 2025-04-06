//---------------------------------------------------------
// File:	Level2.cpp
// Project: Heavy Assault : Rust & Dust
// Authors:	
//			Muhammad Raziq bin Mohd Rafig ------------- 35%
//			- Boss 2
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 35%
//			- Map
//			- Audio
// 
//			Manfred Sim ------------------------------- 15%
//			- File IO
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 15%
//			- Level Transition 
//
// Brief:	Implementation of Level 2 state functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "UI.h"
#include "player.h"
#include "Enemies.h"
#include "Level2.h"
#include "Boss2.h"
#include "GameStateManager.h"


AEGfxTexture* mapTexture2 = nullptr;

Timer cleartransitiontimer2;
Timer gameovertransitiontimer2;

bool showBossHitboxes2 = true;
bool TransitionL2_Start;

float PLAYER_BASE_OFFSET2{};
float BOSS_BASE_OFFSET2{};

f32 map_width2, map_height2;

int dummy_check2; //for sword SFX
AEVec2 dummy_pos2; //for sword SFX

// File IO
std::vector<float> L2_IO;


void Level2_Load()
{

	// UI elements
	Load_UI();
	Load_Player_UI();

	Load_Player();
	Load_Boss2();


	// Meshes
	Load_Mesh(20);

	mapTexture2 = AEGfxTextureLoad("Assets/map2.png");
	if (!mapTexture2)
	{
		std::cerr << "Failed to load map texture!\n";
	}

	// Read File IO
	ReadFromFile("Assets/FileIO/Level2.txt", L2_IO);
}

void Level2_Initialize()
{
	// Player
	Init_Player();

	// Camera
	Init_Camera();

	// Transition
	transition_xpos = f32(AEGfxGetWindowWidth());
	TransitionL2_Start = false;

	// Boss
	Init_Boss2();
	Init_Boss2_Indicator();

	// UI
	Init_UI();

	dummy_check2 = 0;
	dummy_pos2 = { L2_IO[5], L2_IO[6] };

	// Offset
	PLAYER_BASE_OFFSET2 = { L2_IO[8] };
	BOSS_BASE_OFFSET2 = { L2_IO[9] };

	// Level map
	map_width2 = L2_IO[10];
	map_height2 = L2_IO[11];

	// Incrementing Level Timer
	levelTimer2 = L2_IO[0];

	// Set Camera to Player
	LockCamera = false;
	player.canAttack = true;

	// Level Clear Transition Timer
	cleartransitiontimer2 = Create_Timer(L2_IO[1]);
	cleartransitiontimer2.current = 0.f;

	// Game Over Transition Timer
	gameovertransitiontimer2 = Create_Timer(L2_IO[2]);
	gameovertransitiontimer2.current = 0.f;
}

void ToggleBossHitboxes2()
{
	showBossHitboxes2 = !showBossHitboxes2;
}

void Level2_Update()
{
	// Delta time
	Get_DT();


	if (AEInputCheckTriggered(AEVK_B))
	{
		nextState = GS_RESTART;
		AEInputShowCursor(0);
	}



	if (levelState != PAUSED && levelState != OPTIONS && levelState != INQUIT && levelState != GAMEOVER)
	{
		LockCamera = false;
		AEInputShowCursor(0);

		// Boss behavior
		Update_Boss2();

		// Player WASD Movement
		Update_Player();

		dummy_check2 = Attacked_Melee(dummy_pos2);

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&levelTimer2);
		}
	}
	else
	{
		LockCamera = true;
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)
	{
		levelState = PAUSED;
		AEInputShowCursor(1);
	}
	else if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PAUSED)
	{
		levelState = PLAYING;
	}
}

void Level2_Draw()
{
	AEGfxSetBackgroundColor(0, 0, 0);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Define map size
	Color borderColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // Blue

	// Offset map drawing so (0,0) is centered in the camera view
	f32 map_x = -map_width2 / 2;
	f32 map_y = -map_height2 / 2;

	// Convert world positions to screen positions
	f32 screen_x = World_To_Screen_x(map_x);
	f32 screen_y = World_To_Screen_y(map_y);

	// Draw map borders
	DrawRect(screen_x + map_width2 / 2, screen_y, map_width2, 10, borderColor); // Bottom
	DrawRect(screen_x + map_width2 / 2, screen_y + map_height2, map_width2, 10, borderColor); // Top
	DrawRect(screen_x, screen_y + map_height2 / 2, 10, map_height2, borderColor); // Left
	DrawRect(screen_x + map_width2, screen_y + map_height2 / 2, 10, map_height2, borderColor); // Right


	// Call the DrawImageAttachedToRect to attach the image to the center of the map rectangle
	DrawImageAttachedToRect(screen_x, screen_y, map_width2, map_height2, mapTexture2);

	Update_Camera();


	if (showBossHitboxes2)
	{
		Draw_Boss2_Hitboxes();
	}

	if (player.pos.y - PLAYER_BASE_OFFSET2 > boss_2.pos.y - BOSS_BASE_OFFSET2)
	{
		Draw_Player();
		Draw_Boss2();
	}
	else
	{
		Draw_Boss2();
		Draw_Player();
	}

	// UI
	Draw_UI();

	// Level Timer
	Draw_LevelTimer(levelTimer2);

	// Level Score
	Draw_LevelScore();

	if (displayPointsAccumulated)
	{
		Draw_Points();
	}
	else
	{
		Update_LevelScore();
	}

	AEInputShowCursor(0);
	if (levelState == PAUSED)
	{
		Draw_PauseMenu();
		AEInputShowCursor(1);
	}
	else if (levelState == GAMEOVER)
	{
		if (!Update_Timer(&gameovertransitiontimer2))
		{
			Draw_GameOverMenu();
			AEInputShowCursor(1);
		}
	}
	else if (levelState == OPTIONS)
	{
		Draw_OptionsMenu();
		AEInputShowCursor(1);
	}
	else if (levelState == INQUIT)
	{
		Draw_QuitMenu();
		AEInputShowCursor(1);
	}
	else if (levelState == CLEARED)
	{
		if (!Update_Timer(&cleartransitiontimer2))
		{
			Draw_ClearMenu();
			AEInputShowCursor(1);
		}
	}

	// If flag has not been set, continue transition effect
	if (!TransitionL2_Start)
	{
		TransitionExit(TransitionL2_Start);
	}
}

void Level2_Free()
{
	Free_UI();
	AEInputShowCursor(1);
}

void Level2_Unload()
{
	AEInputShowCursor(1);
	Unload_UI();
	Unload_Player_UI();
	Unload_Mesh();
	Unload_Player();
	Unload_Boss2();
	if (mapTexture2)
	{
		AEGfxTextureUnload(mapTexture2);
		mapTexture2 = nullptr;
	}
	L2_IO.clear();
}