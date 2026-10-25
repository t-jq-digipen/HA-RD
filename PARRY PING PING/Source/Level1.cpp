//---------------------------------------------------------
// File:	Level1.cpp
// Project: Heavy Assault : Rust & Dust
// Authors :	
//			Manfred Sim ------------------------------- 25%
//			- Level states
// 
//			Muhammad Raziq bin Mohd Rafig ------------- 25%
//			- Boss 1 implementation
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 20%
//			- Level map
//			- Audio
// 
//			Tan Jun Quan ------------------------------ 20%
//			- Drawing state
//			- Player
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 10%
//			- Level Transition 
//
// Brief:	Implementation of Level 1 state functions.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "UI.h"
#include "player.h"
#include "Enemies.h"
#include "Level1.h"
#include "Boss1.h"
#include "GameStateManager.h"


AEGfxTexture* mapTexture = nullptr;

Timer cleartransitiontimer;
Timer gameovertransitiontimer;

AEVec2 dummy_pos;
f32 dummy_rad;

Timer enemyattacktimer;
Timer stunnedtimer;

int dummy_check;

bool showBossHitboxes = true;

// Flags for transitions
bool TransitionL1_Start;

float PLAYER_BASE_OFFSET{};
float BOSS_BASE_OFFSET{};

f32 map_width, map_height;

// File IO
std::vector<float> L1_IO;

void Level1_Load()
{

	// UI elements
	Load_UI();
	Load_Player_UI();

	// Player
	Load_Player();

	// Boss
	Load_Boss();

	// Meshes
	Load_Mesh(20);

	// Map Texture
	mapTexture = AEGfxTextureLoad("Assets/testMapBoundary5.png");
	if (!mapTexture)
	{
		std::cerr << "Failed to load map texture!\n";
	}

	// Read File IO
	ReadFromFile("Assets/FileIO/Level1.txt", L1_IO);
}

void Level1_Initialize()
{
	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	// Level Transition
	transition_xpos = f32(AEGfxGetWindowWidth());
	TransitionL1_Start = false;

	// Level Clear Transition Timer
	cleartransitiontimer = Create_Timer(L1_IO[1]);
	cleartransitiontimer.current = 0.f;

	// Game Over Transition Timer
	gameovertransitiontimer = Create_Timer(L1_IO[2]);
	gameovertransitiontimer.current = 0.f;

	// Enemy Attack Timer
	enemyattacktimer = Create_Timer(L1_IO[3]);
	stunnedtimer = Create_Timer(L1_IO[4]);
	stunnedtimer.current = 0;

	// UI
	Init_UI();

	// Proto boss
	Init_Boss();
	Init_Boss_Indicator();

	// Dummy enemy
	dummy_pos = { L1_IO[5], L1_IO[6] };
	dummy_rad = L1_IO[7];
	dummy_check = 0;

	// Offset
	PLAYER_BASE_OFFSET = { L1_IO[8] };
	BOSS_BASE_OFFSET = { L1_IO[9] };

	// Level map
	map_width = L1_IO[10];
	map_height = L1_IO[11];

	// Incrementing Level Timer (after Init_UI, which resets it)
	levelTimer = L1_IO[0];

	// Set Camera to Player
	LockCamera = false;
	player.canAttack = true;
}

void ToggleBossHitboxes()
{
	showBossHitboxes = !showBossHitboxes;
}

void Level1_Update()
{
	// Delta time
	Get_DT();


	if (AEInputCheckTriggered(AEVK_B) && levelState != CLEARED && levelState != GAMEOVER)
	{
		nextState = GS_RESTART;
		AEInputShowCursor(0);
	}


	if (levelState != PAUSED && levelState != OPTIONS && levelState != INQUIT && levelState != GAMEOVER)
	{
		LockCamera = false;
		AEInputShowCursor(0);

		// Proto boss
		Update_Boss();

		// Player WASD Movement
		Update_Player();

		dummy_check = Attacked_Melee(dummy_pos);

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&levelTimer);
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



void Level1_Draw()
{
	AEGfxSetBackgroundColor(0.8667f, 0.6667f, 0.4863f); // Beige color
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Define map size
	Color borderColor = { 0.902f, 0.725f, 0.451f, 0.0f };

	// Offset map drawing so (0,0) is centered in the camera view
	f32 map_x = -map_width / 2;
	f32 map_y = -map_height / 2;


	// Convert world positions to screen positions
	f32 screen_x = World_To_Screen_x(map_x);
	f32 screen_y = World_To_Screen_y(map_y);


	// Call the DrawImageAttachedToRect to attach the image to the center of the map rectangle
	DrawImageAttachedToRect(screen_x, screen_y, map_width, map_height, mapTexture);


	Update_Camera();

	if (showBossHitboxes)
	{
		Draw_Boss_Hitboxes();
	}

	if (player.pos.y - PLAYER_BASE_OFFSET > boss_1.pos.y - BOSS_BASE_OFFSET)
	{
		//Draw_Boss_Hitboxes();
		Draw_Player();
		Draw_Boss();
	}
	else
	{
		//Draw_Boss_Hitboxes();
		Draw_Boss();
		Draw_Player();
	}

	// UI
	Draw_UI();

	// Level Timer
	Draw_LevelTimer(levelTimer);
	
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
		if (!Update_Timer(&gameovertransitiontimer))
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
		if (!Update_Timer(&cleartransitiontimer))
		{
			Draw_ClearMenu();
			AEInputShowCursor(1);
		}
	}

	// If flag has not been set, continue transition effect
	if (!TransitionL1_Start)
	{
		TransitionExit(TransitionL1_Start);
	}
}

void Level1_Free()
{
	Free_UI();
	AEInputShowCursor(1);
}

void Level1_Unload()
{
	AEInputShowCursor(1);
	Unload_UI();
	Unload_Player_UI();
	Unload_Mesh();
	Unload_Player();
	Unload_Boss();
	if (mapTexture)
	{
		AEGfxTextureUnload(mapTexture);
		mapTexture = nullptr;
	}
	L1_IO.clear();
}