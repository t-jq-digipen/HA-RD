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
AEVec2 DummyMelee_pos{};
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{100.f};

//Timer DummyMelee_AttackTimer;
//Timer DummyMelee_StunTimer;

//to be moved into enemy struct btw!
//int DummyMelee_Counter{};
//f32 StunDummyMelee_Counter{ 0.f };

int DummyMelee_Check;

Timer HubWorld_ClearTransitionTimer;
float HubWorld_LevelTimer;

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

	// Dummy enemy
	DummyMelee_pos = { 300.f, 200.f };
	DummyMelee_rad = 50.f;
	DummyMelee_Check = 0;

	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	Init_DummyBotMelee(DummyMelee_pos, DummyMelee_rad, MaxHealth);

	// Enemy Attack Timer
	/*DummyMelee_AttackTimer = Create_Timer(1.f);
	DummyMelee_StunTimer = Create_Timer(2.f);
	DummyMelee_StunTimer.current = 0;*/

	// UI
	Init_UI();

	// Incrementing Level Timer
	HubWorld_LevelTimer = 0.f;

	// Level Clear Transition Timer
	HubWorld_ClearTransitionTimer = Create_Timer(5.f);
	HubWorld_ClearTransitionTimer.current = 0;
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

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);
		Active_DummyBotMelee();

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&HubWorld_LevelTimer, dt);
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
	//EnemyAttackPattern(&DummyMelee_AttackTimer,&DummyMelee_StunTimer, &DummyMelee_Counter, DummyMelee_pos, DummyMelee_rad, &player);
	//Loop_Timer(&DummyMelee_AttackTimer);
	//EnemyAttackPattern(&parryTimer, &DummyMelee_Counter, &StunDummyMelee_Counter, DummyMelee_pos, DummyMelee_rad, player.pos, player.radius);
	//Loop_Timer(&parryTimer);

	// Player
	Draw_Player();

	// Boss HP
	//Draw_Boss1HP();

	// UI
	//maybe remove? can try add system with like HUD that only shows when u are attacking or doing actions, and disappears when not doing anyth so it looks more cinematic and cool woowooowowowoowowoowooowooooyeaaaaa
	Draw_UI();

	// Level Timer
	Draw_LevelTimer(HubWorld_LevelTimer);

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
		if (!Update_Timer(&HubWorld_ClearTransitionTimer))
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