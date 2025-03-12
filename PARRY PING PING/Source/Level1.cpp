//---------------------------------------------------------
// file:	Level1.cpp
// author:	MANFRED SIM, MUHAMMAD FAUZAN BIN MOHAMED FAUZY
// email:	manfred.sim@digipen.edu, muhammadfauzan.b@digipen.edu
//
// brief:	Implementation of Level 1 functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "UI.h"
#include "player.h"
#include "Enemies.h"
#include "Level1.h"
#include "Boss1.h"
#include "Prototype_Boss.h"
#include "GameStateManager.h"
#include "mobs.h"


AEGfxTexture* mapTexture = nullptr;


AEVec2 dummy_pos, JQMonke_pos;
f32 dummy_rad, JQMonke_rad;
int JQCounter{};

Timer enemyattacktimer;
Timer stunnedtimer;

//to be moved into enemy struct btw!
int counter{};
f32 StunCounter{ 0.f };

int dummy_check;

Timer cleartransitiontimer;
float levelTimer;


void Level1_Load()
{
	Load_Player();
	Load_Boss();


	// Meshes
	Load_Mesh(20);

	mapTexture = AEGfxTextureLoad("Assets/testMapBoundary3.png");
	if (!mapTexture)
	{
		printf("Failed to load map texture!\n");
	}
	std::cout << "Level 1 Loaded\n";


}

void Level1_Initialize()
{
	std::cout << "Level 1 Initialized\n";

	// Music
	AEAudioInit();

	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	// Enemy Attack Timer
	enemyattacktimer = Create_Timer(1.f);
	stunnedtimer = Create_Timer(2.f);
	stunnedtimer.current = 0;

	// Mobs
	Init_Mobs();

	// Boss
	Init_Boss1();

	// UI
	Init_UI();

	// Proto boss
	Init_Boss();
	Init_Boss_Indicator();

	// Dummy enemy
	dummy_pos = { 300.f, 200.f };
	dummy_rad = 25.f;
	dummy_check = 0;

	//JQMonke
	JQMonke_pos = { -300.f, 200.f };
	JQMonke_rad = 50.f;

	// Incrementing Level Timer
	levelTimer = 0.f;

	// Level Clear Transition Timer
	cleartransitiontimer = Create_Timer(5.f);
	cleartransitiontimer.current = 0;
}

void Level1_Update()
{
	// Delta time
	Get_DT();

	// Music
	AEAudioUpdate();

	if (AEInputCheckTriggered(AEVK_B))
	{
		nextState = GS_RESTART;
		AEInputShowCursor(0);
	}

	if (levelState != PAUSED && levelState != OPTIONS && levelState != GAMEOVER)
	{
		AEInputShowCursor(0);
		//std::cout << "Playing\n";
		// Boss behavior
		Update_Boss1();

		// Proto boss
		Update_Boss();

		// Mobs
		Update_Mobs();

		// Player WASD Movement
		Update_Player();

		dummy_check = Attacked_Melee(dummy_pos);

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&levelTimer);
		}
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
	AEGfxSetBackgroundColor(0, 0, 0);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Define map size
	f32 map_width = 3000.0f, map_height = 2500.0f;
	Color borderColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // Blue

	// Offset map drawing so (0,0) is centered in the camera view
	f32 map_x = -map_width / 2;
	f32 map_y = -map_height / 2;

	// Convert world positions to screen positions
	f32 screen_x = World_To_Screen_x(map_x);
	f32 screen_y = World_To_Screen_y(map_y);

	// Draw map borders
	DrawRect(screen_x + map_width / 2, screen_y, map_width, 10, borderColor); // Bottom
	DrawRect(screen_x + map_width / 2, screen_y + map_height, map_width, 10, borderColor); // Top
	DrawRect(screen_x, screen_y + map_height / 2, 10, map_height, borderColor); // Left
	DrawRect(screen_x + map_width, screen_y + map_height / 2, 10, map_height, borderColor); // Right


	// Call the DrawImageAttachedToRect to attach the image to the center of the map rectangle
	DrawImageAttachedToRect(screen_x, screen_y, map_width, map_height, mapTexture);



	Update_Camera();


	// Parryable dummy!
	//EnemyAttackPattern(&enemyattacktimer,&stunnedtimer, &counter, dummy_pos, dummy_rad, &player);
	//Loop_Timer(&enemyattacktimer);
	//EnemyAttackPattern(&parryTimer, &counter, &StunCounter, dummy_pos, dummy_rad, player.pos, player.radius);
	//Loop_Timer(&parryTimer);

	//JQ Monke
	//JQMonkeAttackDemo(&parryTimer, &JQCounter, &StunCounter, JQMonke_pos, JQMonke_rad, player.pos, player.radius);

	// manfred Boss
	//Draw_Boss1();

	// Proto boss
	Draw_Boss();

	// Mobs
	Draw_Mobs();

	// Player
	Draw_Player();

	// Boss HP
	//Draw_Boss1HP();

	// UI
	Draw_UI();



	// Level Timer
	Draw_LevelTimer(levelTimer);

	if (levelState == PAUSED)
	{
		Draw_PauseMenu();
	}
	else if (levelState == GAMEOVER)
	{
		Draw_GameOverMenu();
	}
	else if (levelState == OPTIONS)
	{
		Draw_OptionsMenu();
	}
	else if (levelState == CLEARED)
	{
		if (!Update_Timer(&cleartransitiontimer))
		{
			Draw_ClearMenu();
		}
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
	Unload_Mesh();
	Unload_Player();
	Unload_Boss();
	if (mapTexture)
	{
		AEGfxTextureUnload(mapTexture);
		mapTexture = nullptr;
	}
	std::cout << "level1_unload";
}