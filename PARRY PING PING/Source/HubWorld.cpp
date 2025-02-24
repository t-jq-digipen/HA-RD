//---------------------------------------------------------
// file:	HubWorld.cpp
// author:	ASHFER
// email:	m.binafzalkhan@digipen.edu
//
// brief:	Implementation of Hub World functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "player.h"
#include "Enemies.h"
#include "HubWorld.h"
#include "GameStateManager.h"

//removable in future implementations! for parry bot stats
AEVec2 dummy_pos{};
f32 dummy_rad{}, CurrHealth{}, MaxHealth{};

Timer enemyattacktimer;
Timer stunnedtimer;

//to be moved into enemy struct btw!
int counter{};
f32 StunCounter{ 0.f };

int dummy_check;

Timer cleartransitiontimer;
float levelTimer;

void HubWorld_Load()
{
	Load_Player();
	Load_Prototype_Boss();

	// Meshes
	Load_Mesh(20);
	std::cout << "Hub World Loaded\n";
}

void HubWorld_Initialize()
{
	std::cout << "Hub World Initialized\n";

	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	Init_DummyBotMelee(dummy_pos, dummy_rad, MaxHealth);

	// Enemy Attack Timer
	enemyattacktimer = Create_Timer(1.f);
	stunnedtimer = Create_Timer(2.f);
	stunnedtimer.current = 0;

	// UI
	Init_UI();

	// Dummy enemy
	dummy_pos = { 300.f, 200.f };
	dummy_rad = 25.f;
	dummy_check = 0;

	// Incrementing Level Timer
	levelTimer = 0.f;

	// Level Clear Transition Timer
	cleartransitiontimer = Create_Timer(5.f);
	cleartransitiontimer.current = 0;
}

void HubWorld_Update()
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

void HubWorld_Draw()
{
	Update_Camera();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// Parryable dummy!
	//EnemyAttackPattern(&enemyattacktimer,&stunnedtimer, &counter, dummy_pos, dummy_rad, &player);
	//Loop_Timer(&enemyattacktimer);
	//EnemyAttackPattern(&parryTimer, &counter, &StunCounter, dummy_pos, dummy_rad, player.pos, player.radius);
	//Loop_Timer(&parryTimer);

	// Player
	Draw_Player();

	// Boss HP
	//Draw_Boss1HP();

	// UI
	//maybe remove? can try add system with like HUD that only shows when u are attacking or doing actions, and disappears when not doing anyth so it looks more cinematic and cool woowooowowowoowowoowooowooooyeaaaaa
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

void HubWorld_Free()
{

}

void HubWorld_Unload()
{
	Unload_Mesh();
	Unload_Player();
	std::cout << "HubWorld_unload";
}