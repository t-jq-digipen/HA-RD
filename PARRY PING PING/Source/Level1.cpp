//---------------------------------------------------------
// file:	Level1.cpp
// author:	MANFRED SIM, MUHAMMAD FAUZAN BIN MOHAMED FAUZY
// email:	manfred.sim@digipen.edu, muhammadfauzan.b@digipen.edu
//
// brief:	Implementation of Level 1 functions
//
// Copyright � 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "UI.h"
#include "player.h"
#include "Enemies.h"
#include "Level1.h"
#include "Prototype_Boss.h"
#include "GameStateManager.h"
#include "mobs.h"


AEGfxTexture* mapTexture = nullptr;

Timer cleartransitiontimer;
Timer gameovertransitiontimer;

AEVec2 dummy_pos;
f32 dummy_rad;

Timer enemyattacktimer;
Timer stunnedtimer;

int dummy_check;

bool showBossHitboxes = true;

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
		printf("Failed to load map texture!\n");
	}
	std::cout << "Level 1 Loaded\n";

	// Read File IO
	ReadFromFile("Assets/FileIO/Level1.txt", L1_IO);
}

void Level1_Initialize()
{
	std::cout << "Level 1 Initialized\n";


	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	// Incrementing Level Timer
	levelTimer = L1_IO[0];

	// Level Score
	currentScore = 0;
	displayPointsAccumulated = false;

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

	// Mobs
	Init_Mobs();

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

	// Incrementing Level Timer
	levelTimer = 0.f;

	// Level Score
	currentScore = 0;
	displayPointsAccumulated = false;

	// Set Camera to Player
	LockCamera = false;
	player.canAttack = true;

	// Level Clear Transition Timer
	cleartransitiontimer = Create_Timer(5.f);
	cleartransitiontimer.current = 0.f;

	// Game Over Transition Timer
	gameovertransitiontimer = Create_Timer(2.f);
	gameovertransitiontimer.current = 0.f;
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
		AEInputShowCursor(0);
		//std::cout << "Playing\n";

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

		std::cout << "BGM Volume: " << current_bgm_vol << '\n';
		std::cout << "SFX Volume: " << current_sfx_vol << '\n';
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

	// Mobs
	Draw_Mobs();

	if (showBossHitboxes)
	{
		Draw_Boss_Hitboxes();
		//std::cout << "Hitboxes drawn \n";
	}
	else
	{
		//std::cout << "Hitboxes not drawn\n";
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
		if (player.hp > 0.f)
		{
			Draw_Player();
		}
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
	std::cout << "level1_unload";
}