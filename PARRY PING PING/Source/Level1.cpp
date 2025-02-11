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
#include "Prototype_Boss.h"
#include "GameStateManager.h"

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
	Load_Prototype_Boss();

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

	// Enemy Attack Timer
	enemyattacktimer = Create_Timer(1.f);
	stunnedtimer = Create_Timer(2.f);
	stunnedtimer.current = 0;

	// Boss
	Init_Boss1();

	// UI
	Init_UI();

	// Proto boss
	Init_Prototype_Boss();
	Init_Prototype_Indicator();

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

	if (AEInputCheckTriggered(AEVK_B))
	{
		nextState = GS_RESTART;
	}

	if (levelState != PAUSED && levelState != GAMEOVER)
	{
		std::cout << "Playing\n";
		// Boss behavior
		Update_Boss1();

		// Proto boss
		Update_Prototype_Boss();

		// Player WASD Movement
		Update_Player();

		dummy_check = Attacked_Melee(dummy_pos);

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&levelTimer, dt);
		}
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)
	{
		levelState = PAUSED;
	}
	else if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PAUSED)
	{
		levelState = PLAYING;
	}
}

void Level1_Draw()
{
	Update_Camera();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

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
	Draw_Prototype_Boss();

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

}

void Level1_Unload()
{
	Unload_Mesh();
	Unload_Player();
	Unload_Prototype_Boss();
	std::cout << "level1_unload";
}