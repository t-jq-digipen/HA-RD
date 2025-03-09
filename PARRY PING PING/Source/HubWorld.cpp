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

//Variables for parry bot init
AEVec2 DummyMelee_pos{}, DummyRange_pos{};
EnvStruct Env_Gate{ 0.f, 550.f, 500.f, 100.f }, Env_Campfire{ -50.f, 0.f, 50.f, 50.f }, Env_Shrine{ -600.f, -200.f, 70.f, 150.f }, Env_Modifiers{-600.f, 300.f, 100.f, 200.f};
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{50.f};

//TEMPORARY
Color tempUI_white;
s8  tempUI_font;

//Fade from black transition 
Color FadeIn;
f32 FadeInSpeed;

int DummyMelee_Check;

Timer HubWorld_ClearTransitionTimer;
float HubWorld_LevelTimer;

void HubWorld_Load()
{
	Load_Player();
	Load_Boss();

	// Meshes
	Load_Mesh(20);
	std::cout << "Hub World Loaded\n";
}

void HubWorld_Initialize()
{
	std::cout << "Hub World Initialized\n";

	// Dummy enemy
	DummyMelee_pos = { 600.f, 200.f };
	DummyRange_pos = { 800.f, 200.f };
	DummyMelee_rad = 50.f;
	DummyMelee_Check = 0;

	tempUI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
	tempUI_white = SetColor(0xFFFFFFFF);

	FadeIn = SetColor(0xFF000000);
	FadeInSpeed = 0.01f;

	// Player
	Init_Player();

	// Camera
	Init_Camera();

	Init_Enemy_Indicator();

	Init_DummyBotMelee(DummyMelee_pos, DummyMelee_rad, MaxHealth);
	Init_DummyBotRange(DummyRange_pos, DummyMelee_rad, MaxHealth);


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
		//std::cout << "Playing\n";

		// Player WASD Movement
		Update_Player();

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);
		Active_DummyBotMelee();
		Active_DummyBotRange();

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&HubWorld_LevelTimer, dt);
		}

		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width, Env_Gate.Height)) {
			//super bare bones for now, will add ability to 'hit button' and start a run w proper transitions etc
			nextState = GS_LEVEL1;
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
	if (AEInputCheckTriggered(AEVK_0))
	{
		FadeIn.A = 0;
	}
	if (AEInputCheckTriggered(AEVK_9))
	{
		FadeIn.A = 1;
	}
	std::cout << '\n' << FadeIn.A << '\n';
}

void HubWorld_Draw()
{
	Update_Camera();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	//Draw Parry Bots
	Draw_DummyBotMelee();
	Draw_DummyBotRange();

	//Draw Environment
	DrawRect(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width, Env_Gate.Height, SetColor(0xFFFF6961));
	//DrawTextBox(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width * 2, Env_Gate.Height * 2, 10.f, tempUI_font, tempUI_white, "GATE");

	DrawRect(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height, SetColor(0xFFFF6961));
	//DrawTextBox(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height, 1.f, tempUI_font, tempUI_white, "CAMPFIRE");

	DrawRect(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height, SetColor(0xFFFF6961));
	//DrawTextBox(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height, 1.f, tempUI_font, tempUI_white, "SHRINE");

	DrawRect(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, SetColor(0xFFFF6961));
	//DrawTextBox(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, 1.f, tempUI_font, tempUI_white, "MODIFIERS");
	

	// Player
	Draw_Player();

	// UI
	//maybe remove? can try add system with like HUD that only shows when u are attacking or doing actions, and disappears when not doing anyth so it looks more cinematic and cool woowooowowowoowowoowooowooooyeaaaaa
	Draw_UI(0);

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

	//cool little fade in effect
	if (FadeIn.A > 0) {
		DrawRect(0.f, 0.f, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), FadeIn);
		FadeIn.A -= FadeInSpeed;
	}
	else FadeIn.A = 0.f;
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