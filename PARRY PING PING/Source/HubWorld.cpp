//---------------------------------------------------------
// file:	HubWorld.cpp
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Definition of Hub World functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "player.h"
#include "Enemies.h"
#include "HubWorld.h"
#include "GameStateManager.h"

// Variables for parry bot init
AEVec2 DummyMelee_pos{}, DummyRange_pos{};
EnvStruct Env_Gate{ 0.f, 550.f, 500.f, 100.f }, Env_Campfire{ -50.f, 0.f, 50.f, 50.f }, Env_Shrine{ -600.f, -200.f, 70.f, 150.f }, Env_Modifiers{-600.f, 300.f, 100.f, 200.f};
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{50.f};

// TEMPORARY - Placeholder for sprites to be added
Color tempUI_white;
s8  tempUI_font;

// Fade from black transition when level starts
Color FadeIn;
f32 FadeInSpeed;

int DummyMelee_Check;										// Check for dummy logic

Timer HubWorld_ClearTransitionTimer;						// For Level states
float HubWorld_LevelTimer;

void HubWorld_Load()
{
	Load_Player();											// Load Player

	// Meshes
	Load_Mesh(20);
	std::cout << "Hub World Loaded\n";
}

void HubWorld_Initialize()
{
	std::cout << "Hub World Initialized\n";

	// Dummy enemy
	DummyMelee_pos = { 600.f, 200.f };							// Init positions, and variables of Melee and Ranged Projectile Practice Range Dummy
	DummyRange_pos = { 800.f, 200.f };
	DummyMelee_rad = 50.f;
	DummyMelee_Check = 0;

	tempUI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);		// Font and text colour
	tempUI_white = SetColor(0xFFFFFFFF);

	FadeIn = SetColor(0xFF000000);											// For Fade-From_Black transition as level starts
	FadeInSpeed = 0.01f;

	// Player
	Init_Player();															// Init Player

	// Camera
	Init_Camera();															// Init Camera

	Init_Enemy_Indicator();													// Init Enemy Indicators (colours)

	Init_DummyBotMelee(DummyMelee_pos, DummyMelee_rad, MaxHealth);			// Init the dummies with specified stats!
	Init_DummyBotRange(DummyRange_pos, DummyMelee_rad, MaxHealth);


	// UI
	Init_UI();																// Init UI

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
		nextState = GS_RESTART;												// Press B to restart level
	}

	if (levelState != PAUSED && levelState != GAMEOVER)
	{
		// Player WASD Movement
		Update_Player();

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);					// Check for Player-Attacking-Bot detection
		Active_DummyBotMelee();												// Update logic for dummies
		Active_DummyBotRange();

		// Incrementing level timer
		if (levelState != CLEARED)
		{
			Update_LevelTimer(&HubWorld_LevelTimer);						// Update level timer when level is ongoing
		}

		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width, Env_Gate.Height)) {			// When player reaches the 'gate', start the Level 1 boss fight
			//super bare bones for now, will add ability to 'hit button' and start a run w proper transitions etc
			nextState = GS_LEVEL1;														
		}
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)				// Pause menu when escape key is pressed!
	{
		levelState = PAUSED;
	}
	else if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PAUSED)			// Unpause!
	{
		levelState = PLAYING;
	}
}

void HubWorld_Draw()
{
	Update_Camera();															// Camera drawing logic

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);										// Set render modes for drawing of colours
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);	


	//Draw Parry Bots	
	Draw_DummyBotMelee();														// Draw (placeholder) sprites for dummies!
	Draw_DummyBotRange();

	//Draw Environment
	DrawRect(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width, Env_Gate.Height, SetColor(0xFFFF6961));			// Gate - player starts boss fight level here!
	DrawTextToWorld(Env_Gate.Pos.x, Env_Gate.Pos.y, Env_Gate.Width, Env_Gate.Height, 6.5f, tempUI_font, tempUI_white, "GATE");

	DrawRect(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height, SetColor(0xFFFF6961));		// Campfire - Center of level, where player spawns when Hub World loads
	//DrawTextBox(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height, 1.f, tempUI_font, tempUI_white, "CAMPFIRE");

	DrawRect(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height, SetColor(0xFFFF6961));		// Leaderboard Shrine (TO BE IMPLEMENTED!) - Top 3 recorded runs shown here
	//DrawTextBox(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height, 1.f, tempUI_font, tempUI_white, "SHRINE");

	DrawRect(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, SetColor(0xFFFF6961));		// Modifier Station (TO BE IMPLEMENTED!) - Modify settings for the upcoming level
	//DrawTextBox(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, 1.f, tempUI_font, tempUI_white, "MODIFIERS");
	

	// Player
	Draw_Player();				// Draw player sprites 

	// UI
	// !!!TO DO!!! can try add system with like HUD that only shows when u are attacking or doing actions, and disappears when not doing anyth so it looks more cinematic and cool 
	Draw_UI(0);					// Draw UI - (0) added to not draw boss healthbar

	// Level Timer
	Draw_LevelTimer(HubWorld_LevelTimer);		// Draw timer on bottom right

	if (levelState == PAUSED)
	{
		Draw_PauseMenu();						// Draw pause menu if game is paused
	}
	else if (levelState == GAMEOVER)
	{
		Draw_GameOverMenu();					// Draw game over screen when player dies
	}
	else if (levelState == CLEARED)
	{
		if (!Update_Timer(&HubWorld_ClearTransitionTimer))
		{
			Draw_ClearMenu();					// Draw level cleared menu when level is cleared
		}
	}

	//cool little fade in effect
	if (FadeIn.A > 0) {
		DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), FadeIn);			// Just adjusts alpha of a black screen drawn over the level when the level starts
		FadeIn.A -= FadeInSpeed;
	}
	else FadeIn.A = 0.f;																				// Clamp to 0
}

void HubWorld_Free()
{
	Free_Player();																						// Free memory!
}

void HubWorld_Unload()
{
	Unload_UI();																						// Free memory! Unloads UI, Meshes and Player!
	Unload_Mesh();
	Unload_Player();
	std::cout << "HubWorld_unload";
}