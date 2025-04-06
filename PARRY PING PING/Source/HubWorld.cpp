//---------------------------------------------------------
// File:	HubWorld.cpp
// Project: Heavy Assault : Rust & Dust
// Authors:	
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 55%
//			- Interactable environments (Leaderboard, Modifiers, Ranged + Melee Parry Dummies)
//			- Camera lock effect
//			- Level selection + transition
//			- Sprites
//				- Modifiers
//				- Shrine
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 30%
//			- Leaderboard fetching
//			- Audio
//			- Map
// 
//			Tan Jun Quan ------------------------------ 10%
//			- Sprites
//				- Campfire
//				- Dummy bots
//				- Spirits
//			- Tutorial
// 
//			Manfred Sim ------------------------------- 5%
//			- File IO
//
// Brief:	Definition of Hub World functions.
//			Holds the logic for the hub world state, which
//			allows the player to interact with various
//			gameplay elements and teaches them with a
//			tutorial overlay.
//			The player can access an online leaderboard
//			linked via Google API to a google sheet, practice
//			against dummy enemies and adjust difficulty 
//			modifiers here before entering the boss fights.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "player.h"
#include "Enemies.h"
#include "HubWorld.h"
#include "GameStateManager.h"
#include "Tutorial.h"
#include "Camera.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstdio>  // For popen()
#include <direct.h>
#include <thread>
#include <atomic>
#include <future>
#include "SpriteSheet.h"


// Variables for parry bot init
AEVec2 DummyMelee_pos{}, DummyRange_pos{};
EnvStruct Env_Gate, Env_Gate2, Env_Campfire, Env_Shrine, Env_Modifiers;
EnvStruct Txt_Controls, Txt_Movement, Txt_Attack;
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{ 50.f };

// Text vars
Color hubworld_white;
s8  hubworld_font;

// Spritesheets
SpriteSheet campfire_sprite;
SpriteSheet shrine_sprite;
SpriteSheet modifiers_sprite;
SpriteSheet spirit_sprite;

// Fade from black transition when level starts
Color FadeIn, FadeGate, FadeShrine, FadeModifiers;
f32 FadeInSpeed;

int DummyMelee_Check;										// Check for dummy logic

Timer HubWorld_ClearTransitionTimer;						// For Level states
float HubWorld_LevelTimer;

// Flags and more for which UI screens to draw
int ShowBoss;												// Show boss 1/ 2  (0 & 1 respectively)
bool InTop20;												// For Top 20 highscores
bool Draw_Top3;												// For Top 3 screen
bool InModifiers;											// For modifier selection screen
bool Transition_Flag_Gate1, Transition_Flag_Gate2;
bool Transition_Start;
const char* Mod_DmgTaken[4]{ "Invulnerable", "Reduced", "Default", "We ball" };			// Difficulty names

// For displaying level best times
int minutes1, seconds1, milliseconds1;
int minutes2, seconds2, milliseconds2;
char bestTime1[16];
char bestTime2[16];

//Leaderboard fetching
using ArrOfVecs = std::vector<std::string>[2];
// Array of 2 vectors for storing highscore names, score, time
ArrOfVecs HS_name;
ArrOfVecs HS_score;
ArrOfVecs HS_time;

std::atomic<bool> isFetchingLeaderboard = false;
// Global future to track the async fetch
std::future<void> leaderboardFetchFuture;

// File IO
std::vector<float> HubWorld_IO_elements;
std::vector<float> HubWorld_IO_scores;

AEGfxTexture* loadingImage1;
float rotationAngle1 = 0.f; // Angle for rotation of the loading image
float rotationSpeed1 = 90.f; // Rotation speed (degrees per second)

f32 hubMap_height = 1536.f;
f32 hubMap_width = 3272.f;

AEGfxTexture* hubMapTexture;
AEGfxTexture* dangerzone;

#pragma region Leaderboard Loading
void DrawLoadingImage1() 
{
	Color grey = { 0.251f, 0.251f, 0.251f, 1.f };
	if (isFetchingLeaderboard.load()) 
	{   // Only draw loading image while fetching
		// Draw the image with a rotation angle
		DrawRect(0.f, 0.f, 1600.f, 900.f, grey); // RGBA for grey color with transparency (0.5f opacity)
		DrawImage(0.f, 0.f, 1280.f, 720.f, 0.f, loadingImage1);  // Full screen translucent background
	}
}

void FetchLeaderboardAsync() 
{
	if (isFetchingLeaderboard.load()) return; // Prevent multiple fetch calls
	isFetchingLeaderboard = true;

	// Launch both fetch operations asynchronously
	leaderboardFetchFuture = std::async(std::launch::async, []() {
		fetchLeaderboard(0); // Fetch leaderboard for Boss1
		isFetchingLeaderboard = false; // Set fetching to false after both are done
		fetchLeaderboard(1); // Fetch leaderboard for Boss2
		});
}

// Call this function when the game is closing
void WaitForLeaderboardFetch() 
{
	if (leaderboardFetchFuture.valid()) 
	{
		leaderboardFetchFuture.get(); // Wait for completion
	}
}
#pragma endregion


void HubWorld_Load()
{
	loadingImage1 = AEGfxTextureLoad("Assets/loadingStatus.png");

	// UI elements
	Load_UI();
	Load_Player_UI();

	// Player
	Load_Player();
	
	// Dummy Bots
	Load_DummyBot();

	// Tutorial
	Load_Tutorial();

	FetchLeaderboardAsync();

	shrine_sprite		= SpriteSheet("Assets/HubWorld/Shrine.png", 13, 1);
	campfire_sprite		= SpriteSheet("Assets/HubWorld/Campfire.png", 12, 1);
	modifiers_sprite	= SpriteSheet("Assets/HubWorld/Compuser.png", 4, 1);
	spirit_sprite		= SpriteSheet("Assets/HubWorld/Spirit.png", 12, 1);

	hubMapTexture = AEGfxTextureLoad("Assets/hubWorldMap.png");
	dangerzone = AEGfxTextureLoad("Assets/HubWorld/Danger.png");

	// Meshes
	Load_Mesh(20);

	// Read File IO
	ReadFromFile("Assets/FileIO/HubWorld.txt", HubWorld_IO_elements);
	ReadFromFile("Assets/FileIO/PlayerBestScores.txt", HubWorld_IO_scores);

	// Hub World Environments
	Env_Gate		= { HubWorld_IO_elements[0],  HubWorld_IO_elements[1],  HubWorld_IO_elements[2],  HubWorld_IO_elements[3] };	// ENV GATE		- Boss 1
	Env_Gate2		= { HubWorld_IO_elements[4],  HubWorld_IO_elements[5],  HubWorld_IO_elements[6],  HubWorld_IO_elements[7] };	// ENV GATE2	- Boss 2
	Env_Campfire	= { HubWorld_IO_elements[8],  HubWorld_IO_elements[9],  HubWorld_IO_elements[10], HubWorld_IO_elements[11] };	// ENV CAMPFIRE
	Env_Shrine		= { HubWorld_IO_elements[12], HubWorld_IO_elements[13], HubWorld_IO_elements[14], HubWorld_IO_elements[15] };	// ENV SHRINE
	Env_Modifiers	= { HubWorld_IO_elements[16], HubWorld_IO_elements[17], HubWorld_IO_elements[18], HubWorld_IO_elements[19] };	// ENV MODIFIERS

	// Hub World Text
	Txt_Controls	= { HubWorld_IO_elements[20], HubWorld_IO_elements[21], HubWorld_IO_elements[22], HubWorld_IO_elements[23] };	// TXT CONTROLS
	Txt_Movement	= { HubWorld_IO_elements[24], HubWorld_IO_elements[25], HubWorld_IO_elements[26], HubWorld_IO_elements[27] };	// TXT MOVEMENT
	Txt_Attack		= { HubWorld_IO_elements[28], HubWorld_IO_elements[29], HubWorld_IO_elements[30], HubWorld_IO_elements[31] };	// TXT ATTACK

	// Dummy enemy
	DummyMelee_pos  = { HubWorld_IO_elements[32], HubWorld_IO_elements[33] };														// MELEE DUMMY POS	{ 600.f, 200.f }									
	DummyRange_pos  = { HubWorld_IO_elements[35], HubWorld_IO_elements[36] };														// RANGE DUMMY POS	{ 800.f, 200.f }
	DummyMelee_rad  = (HubWorld_IO_elements[34]);																					// MELEE DUMMY RAD	50.f

	// Font
	hubworld_font   = AEGfxCreateFont("Assets/liberation-mono.ttf", 36);															// Font and text colour
	hubworld_white  = SetColor(0xFFFFFFFF);

	// Fade transitions
	FadeIn = FadeGate = FadeShrine = SetColor(0xFF000000);
	FadeInSpeed = HubWorld_IO_elements[37];																							// FADE IN SPEED	0.03f
	FadeShrine.A = HubWorld_IO_elements[38];																						// FADE ALPHA		0.f
	FadeGate.A = HubWorld_IO_elements[38]; 																							// FADE ALPHA		0.f

	// Clear transition timer
	HubWorld_ClearTransitionTimer = Create_Timer(HubWorld_IO_elements[39]);															// CLEAR TRANSITION TIMER	5.f


}

void HubWorld_Initialize()
{

	// UI Default flags/ settings
	ShowBoss = 0;
	InTop20 = false;
	Draw_Top3 = true;
	InModifiers = false;
	player.Mod_DmgTakenIndex = 2;
	Transition_Flag_Gate1 = Transition_Flag_Gate2 = false;
	Transition_Start = false;
	transition_xpos = 0.f;

	// Resize Vectors for highscores to hold 20 values
	for (int i = 0; i < 2; i++) {
		HS_name[i].resize(20);
		HS_score[i].resize(20);
		HS_time[i].resize(20);
	}

	// Player
	Init_Player();															// Init Player

	// Camera
	Init_Camera();															// Init Camera
	
	Init_Enemy_Indicator();													// Init Enemy Indicators (colours)

	// Dummy
	Init_DummyBotMelee(DummyMelee_pos, DummyMelee_rad, MaxHealth);			// Init the dummies with specified stats!
	Init_DummyBotRange(DummyRange_pos, DummyMelee_rad, MaxHealth);
	DummyMelee_Check = 0;													// Reset checks of melee attacks

	// UI
	Init_UI();																// Init UI

	// Tutorial
	Init_Tutorial();														// Init Tutorial elements

	// Reset Level Timer
	HubWorld_LevelTimer = 0.f;												// LEVEL TIMER		0.f

	// Reset Level Clear Transition Timer
	HubWorld_ClearTransitionTimer.current = 0.f;							// START CLEAR TRANSITION	0.f
	
	// Level 1 Best Time
	minutes1 = static_cast<int>(HubWorld_IO_scores[1]) / 60;
	seconds1 = static_cast<int>(HubWorld_IO_scores[1]) % 60;
	milliseconds1 = static_cast<int>((HubWorld_IO_scores[1] - static_cast<int>(HubWorld_IO_scores[1])) * 100);
	snprintf(bestTime1, sizeof(bestTime1), "Time: %02d:%02d:%02d", minutes1, seconds1, milliseconds1);

	// Level 2 Best Time
	minutes2 = static_cast<int>(HubWorld_IO_scores[3]) / 60;
	seconds2 = static_cast<int>(HubWorld_IO_scores[3]) % 60;
	milliseconds2 = static_cast<int>((HubWorld_IO_scores[3] - static_cast<int>(HubWorld_IO_scores[3])) * 100);
	snprintf(bestTime2, sizeof(bestTime2), "Time: %02d:%02d:%02d", minutes2, seconds2, milliseconds2);
}

void HubWorld_Update()
{
	// Delta time
	Get_DT();


	if (AEInputCheckTriggered(AEVK_B))
	{
		nextState = GS_RESTART;												// Press B to restart level
	}

	if (levelState == PLAYING)
	{
		// Player WASD Movement
		Update_Player();
		player.hp = 100.f;

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);					// Check for Player-Attacking-Bot detection
		Active_DummyBotMelee();												// Update logic for dummies
		Active_DummyBotRange();

		Update_Tutorial();													// Logic for tutorial progression

		if (InTop20 == true || InModifiers == true)
		{
			tutorialActive = false;
		}

				// Detection for entering gates to different boss fights
		// BOSS 1
		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width/2.f, Env_Gate.Height)) 
		{
			if (AEInputCheckTriggered(AEVK_F)) 
			{
				tutorialActive = false;
				LockCamera = false;
				player.canAttack = true;
				Transition_Flag_Gate1 = true;
			}
		}
		// BOSS 2
		else if (AETestPointToRect(&player.pos, &Env_Gate2.Pos, Env_Gate2.Width / 2.f, Env_Gate2.Height)) 
		{
			if (AEInputCheckTriggered(AEVK_F))
			{
				tutorialActive = false;
				LockCamera = false;
				player.canAttack = true;
				Transition_Flag_Gate2 = true;
			}
		}

		// Detection for cinematic stationary camera lock
		// For either shrine or gates area
		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width * 3.f, Env_Gate.Height * 2.f) ||
			AETestPointToRect(&player.pos, &Env_Shrine.Pos, Env_Shrine.Width, Env_Shrine.Height))
		{
			LockCamera = true;
			player.canAttack = false;
		}
		else {
			LockCamera = false;
			player.canAttack = true;
		}

		if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)	// Pause menu when escape key is pressed!
		{
			levelState = PAUSED;
			InTop20 = false;												// Forces player out of UI screens as well
			InModifiers = false;
		}
	}
	else
	{
		if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PAUSED)		// Unpause!
		{
			levelState = PLAYING;
		}
	}
}



void HubWorld_Draw()
{
	Update_Camera();															// Camera drawing logic

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);										// Set render modes for drawing of colours
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	AEGfxSetBackgroundColor(0.259f, 0.157f, 0.208f); // Beige color
	// Define map size
	Color borderColor = { 0.902f, 0.725f, 0.451f, 0.0f };

	// Offset map drawing so (0,0) is centered in the camera view
	f32 map_x = -hubMap_width / 2;
	f32 map_y = -hubMap_height / 2;

	// Convert world positions to screen positions
	f32 screen_x = World_To_Screen_x(map_x);
	f32 screen_y = World_To_Screen_y(map_y);

	// Draw map borders
	DrawRect(screen_x + hubMap_width / 2, screen_y, hubMap_width, 10, borderColor); // Bottom
	DrawRect(screen_x + hubMap_width / 2, screen_y + hubMap_height, hubMap_width, 10, borderColor); // Top
	DrawRect(screen_x, screen_y + hubMap_height / 2, 10, hubMap_height, borderColor); // Left
	DrawRect(screen_x + hubMap_width, screen_y + hubMap_height / 2, 10, hubMap_height, borderColor); // Right


	// Call the DrawImageAttachedToRect to attach the image to the center of the map rectangle
	DrawImageAttachedToRect(screen_x, screen_y, hubMap_width, hubMap_height, hubMapTexture);

#pragma region Draw Environment
	
	// Campfire
	campfire_sprite.Set_Row(0);
	campfire_sprite.Loop(0, 11, 1.f);
	campfire_sprite.Draw(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height);

	DrawImage(World_To_Screen_x(700.f), World_To_Screen_y(100.f), 500.f, 500.f, 0.f, dangerzone);

	// Parry Bots/ Dummies	
	Draw_DummyBotMelee();
	Draw_DummyBotRange();

	// Gates + Modifiers (merged)
	//Highlight effect when approaching gate
	if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width * 3.f, Env_Gate.Height * 2.f))
	{
		Draw_FadeOut(FadeGate);													// Fade to dark effect
		if (AETestPointToRect(&player.pos, &Env_Modifiers.Pos, Env_Modifiers.Width, Env_Modifiers.Height)) {			// Test col with modifier 
			DrawTextAdvanced(Env_Modifiers.Pos.x, Env_Modifiers.Pos.y + Env_Modifiers.Height / 2.f, hubworld_font, hubworld_white, "PRESS [F]", true);
			// F key controls both enter and exit states
			if (AEInputCheckTriggered(AEVK_F) && InModifiers == false)
			{
				tutorialActive = false;
				InModifiers = true;
			}
			else if (AEInputCheckTriggered(AEVK_F) && InModifiers == true)
			{
				InModifiers = false;
				ResetCamera();													// Reset camera back down to player after panning up
			}
		}
		// No col
		else {
			InModifiers = false;
			Draw_FadeIn(FadeShrine);											// Fade from dark effect
		}

		// Prompt for both gates
		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width / 2.f, Env_Gate.Height)) {
			DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y + 100.f, hubworld_font, hubworld_white, "PRESS [F]", true);
		}
		else if (AETestPointToRect(&player.pos, &Env_Gate2.Pos, Env_Gate2.Width / 2.f, Env_Gate2.Height)) {
			DrawTextAdvanced(Env_Gate2.Pos.x, Env_Gate2.Pos.y + 100.f, hubworld_font, hubworld_white, "PRESS [F]", true);
		}
	}
	else
	{
		Draw_FadeIn(FadeGate);													// Fade from dark effect
		InModifiers = false;
	}

	modifiers_sprite.Set_Row(0);
	modifiers_sprite.Loop(0, 3, 1.f);
	modifiers_sprite.Draw(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height);

	DrawRect(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width, Env_Gate.Height, SetColor(0xFFFF6961));		// Gate   - player starts boss fight level here!
	DrawRect(World_To_Screen_x(Env_Gate2.Pos.x), World_To_Screen_y(Env_Gate2.Pos.y), Env_Gate2.Width, Env_Gate2.Height, SetColor(0xFFFF6961));	// Gate 2 - player starts boss fight level here!
	DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y, hubworld_font, hubworld_white, "GATE 1", true);
	DrawTextAdvanced(Env_Gate2.Pos.x, Env_Gate2.Pos.y, hubworld_font, hubworld_white, "GATE 2", true);

	// Shrine 
	//Highlight effect when approaching leaderboard shrine
	if (AETestPointToRect(&player.pos, &Env_Shrine.Pos, Env_Shrine.Width, Env_Shrine.Height))
	{
		Draw_FadeOut(FadeShrine);												// Fade to dark effect
		hubworld_white.A = FadeShrine.A;										// Fade effect for text
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
		hubworld_white.A = 1.f;

		// F key controls both enter and exit states
		if (AEInputCheckTriggered(AEVK_F) && InTop20 == false)
		{
			tutorialActive = false;
			InTop20 = true;
		}
		else if (AEInputCheckTriggered(AEVK_F) && InTop20 == true)
		{
			InTop20 = false;
			ResetCamera();
		}
	}
	else
	{
		InTop20 = false;
		Draw_FadeIn(FadeShrine);												// Fade from dark effect
		hubworld_white.A = FadeShrine.A;										// Text fades WRT fade effect
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);		// Draw faded out text
		hubworld_white.A = 1.f;													// Reset to full alpha for other text elements
	}
	// Draw after for 'glowing' effect
	shrine_sprite.Set_Row(0);
	shrine_sprite.Loop(0, 12, 1.f);
	shrine_sprite.Draw(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height);
#pragma endregion


	// Everyting after can be drawn above the fade effect
	// Player
	Draw_Player();				// Draw player sprites 

	// Tutorial
	Draw_Tutorial();

	AEInputShowCursor(0);						// Default, Hides cursor unless specified

	// UI
	Draw_UI(0);																	// Draw UI - (0) added to not draw boss healthbar

	// State depending drawing!
	if (levelState == PAUSED)
	{
		Draw_PauseMenu();						// Draw pause menu if game is paused
	}
	else if (levelState == GAMEOVER)
	{
		Draw_GameOverMenu();					// Draw game over screen when player dies
	}
	else if (levelState == OPTIONS)
	{
		Draw_OptionsMenu();
	}
	else if (levelState == INQUIT)
	{
		Draw_QuitMenu();
	}
	else if (levelState == CLEARED)
	{
		if (!Update_Timer(&HubWorld_ClearTransitionTimer))
		{
			Draw_ClearMenu();					// Draw level cleared menu when level is cleared
		}
	}

	if (levelState == OPTIONS || levelState == CLEARED || levelState == GAMEOVER || levelState == PAUSED || levelState == INQUIT || InTop20 || InModifiers) 
	{
		LockCamera = true;
		AEInputShowCursor(1);					// Show cursor in these screens specifically
	}

	// Bool flags for drawing shrine & modifiers screens
	if (InTop20) 
	{
		Pan_Camera(0.f, 500.f, 10.f);			// Camera pan effect
		CamPanning = true;
		Draw_ShrineBoard();						// Draw Shrine board
	}
	else 
	{
		CamPanning = false;						// If flag false, reset values
		Draw_Top3 = true;
	}

	if (InModifiers) 
	{
		Pan_Camera(0.f, 500.f, 10.f);			// Camera pan effect
		CamPanning = true;
		Draw_ModifiersBoard();					// Draw Modifiers board
	}
	else 
	{
		CamPanning = false;						// If flag false, reset values
		InModifiers = false;
	}

	//cool little fade in effect on start up
	if (FadeIn.A > 0) 
	{
		DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), FadeIn);			// Just adjusts alpha of a black screen drawn over the level when the level starts
		FadeIn.A -= FadeInSpeed;
	}
	else FadeIn.A = 0.f;																				// Clamp to 0
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);																// Set render modes for drawing of colours
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	if (Transition_Flag_Gate1) 
	{
		TransitionEnter(Transition_Start);
		if (Transition_Start) nextState = GS_LEVEL1;
	}
	else if (Transition_Flag_Gate2) 
	{
		TransitionEnter(Transition_Start);
		if (Transition_Start) nextState = GS_LEVEL2;
	}

	DrawLoadingImage1();
}

void HubWorld_Free()
{
	Free_Player();																						// Free memory!
	AEInputShowCursor(1);																				// Restore cursor!
}

void HubWorld_Unload()
{

	AEInputShowCursor(1);
	Unload_UI();																						// Free memory! Unloads UI, Meshes and Player!
	Unload_Player_UI();
	Unload_Tutorial();
	Unload_Mesh();
	Unload_Player();
	Unload_DummyBot();

	campfire_sprite.Unload();
	shrine_sprite.Unload();
	modifiers_sprite.Unload();
	spirit_sprite.Unload();

	AEGfxTextureUnload(loadingImage1);
	AEGfxTextureUnload(hubMapTexture);
	AEGfxTextureUnload(dangerzone);
	WaitForLeaderboardFetch();


	HubWorld_IO_elements.clear();
}

#pragma region Fade Effects
// Fade from dark
// Simply decreases alpha of black screen drawn ontop of everything (except highlighted elements)
void Draw_FadeIn(Color& Fade) 
{
	FadeInSpeed = 0.03f;																// Uses Accel/ Deccel so reset to starting value!
	if (Fade.A < 0.f) 
	{
		Fade.A = 0.f;																					// Clamp!
		return;
	}
	else {
		DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), Fade);				// Draw black screen with varying opacity
		Fade.A -= FadeInSpeed;
	}
}

// Fade to dark
// Simply increases alpha of black screen drawn ontop of everything (except highlighted elements)
void Draw_FadeOut(Color& Fade) 
{
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), Fade);			// Just adjusts alpha of a black screen drawn over the level when the level starts
	Fade.A += FadeInSpeed;
	FadeInSpeed *= 1.05f;
	if (InTop20) 
	{
		if (Fade.A > .9f) Fade.A = .9f;															// Clamp value specifically for top 20 screen (cool effect)
		return;
	}
	if (Fade.A > .7f) Fade.A = .7f;																// Default clamp
}
#pragma endregion


#pragma region Leaderboard logic
void fetchLeaderboard(int bossID) {
	std::string bossIDStr = (bossID == 0) ? "Boss1" : "Boss2";
	std::string command = "\"Assets\\dist\\leaderboard_helper.exe\" " + bossIDStr;
	int result = system(command.c_str());
	if (result != 0) {
		std::cerr << "Failed to run leaderboard_helper.exe" << std::endl;
		return;
	}

	std::string filename = "leaderboard_" + bossIDStr + ".txt";
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Failed to open " << filename << std::endl;
		return;
	}

	std::vector<std::string> names, scores, times;
	std::string line;

	while (std::getline(file, line)) {
		std::string name, score, time;
		size_t firstDash = line.find(" - ");
		size_t secondDash = line.find(" - ", firstDash + 3);

		if (firstDash != std::string::npos && secondDash != std::string::npos) {
			name = line.substr(0, firstDash);
			score = line.substr(firstDash + 3, secondDash - (firstDash + 3));
			time = line.substr(secondDash + 3);

			// Remove "Time: " prefix if it exists
			if (time.rfind("Time: ", 0) == 0) {
				time = time.substr(6);
			}

			names.push_back(name);
			scores.push_back(score);
			times.push_back(time);
		}
	}

	file.close();

	// Assuming you have global arrays to store leaderboard data
	for (size_t i = 0; i < names.size() && i < 20; i++) {
		HS_name[bossID][i] = names[i];
		HS_score[bossID][i] = scores[i];
		HS_time[bossID][i] = times[i];
	}

}

std::string sanitizeString(const std::string& input) {
	std::string result;
	for (char c : input) {
		// Remove any non-printable characters
		if (isprint(c)) {
			result += c;
		}
	}
	return result;
}
#pragma endregion

#pragma region Draw Screens
// Draw leaderboard UI screen! (Handles both top 3 + top 20)
void Draw_ShrineBoard() {
	// If click on show more: (top 20)
	if ((IsAreaClicked(600.f, 0.f, 100.f, 100.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
	{
		Draw_Top3 = false;
	}
	// Show less (top 3)
	else if ((IsAreaClicked(-600.f, 0.f, 100.f, 100.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
	{
		Draw_Top3 = true;
	}

	//Draw top 3 
	if (Draw_Top3) {
		if ((IsAreaClicked(200.f, 275.f, 75.f, 75.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
		{
			ShowBoss = 1;
		}
		else if ((IsAreaClicked(-200.f, 275.f, 75.f, 75.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
		{
			ShowBoss = 0;
		}

		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 3!", false);

		// Show scores for respective bosses
		switch (ShowBoss)
		{
		case 0:
			DrawTextAdvanced(0.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "BOSS 1", false, 0.5f);
			DrawRect(200.f, 275.f, 75.f, 75.f, SetColor(0xFFFFFFFF));
			DrawTextAdvanced(400.f, 550.f, pixel_font, SetColor(0xFF000000), ">>", false, 0.3f);
			break;
		case 1:
			DrawTextAdvanced(0.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "BOSS 2", false, 0.5f);
			DrawRect(-200.f, 275.f, 75.f, 75.f, SetColor(0xFFFFFFFF));
			DrawTextAdvanced(-400.f, 550.f, pixel_font, SetColor(0xFF000000), "<<", false, 0.3f);
			break;
		default:
			ShowBoss = 0;
			break;
		}

		// TOP 3 SOULS/ SPIRITS
		spirit_sprite.Set_Row(0);
		spirit_sprite.Loop(0, 11, 1.f);

		spirit_sprite.Draw(0.f, 100.f, 100.f, 100.f);
		spirit_sprite.Draw(-200.f, 75.f, 100.f, 100.f);
		spirit_sprite.Draw(200.f, 50.f, 100.f, 100.f);


		// HS_name/score/time -> arr of 2 vectors, each vector holds 20 strings containing specified data: [BOSS_NUMBER][POSITION_IN_LEADERBOARD]
		// To access eg name of 5th place name for boss 1: HS_name[0][4]
		// Top 1
		DrawTextAdvanced(0.f, 75.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[ShowBoss][0]).c_str(), false, 0.15f);  // Convert std::string to const char*
		DrawTextAdvanced(0.f, 25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[ShowBoss][0]).c_str(), false, 0.125f);  // Convert std::string to const char*
		DrawTextAdvanced(0.f, 0.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[ShowBoss][0]).c_str(), false, 0.125f);  // Convert std::string to const char*

		// Top 2
		DrawTextAdvanced(-400.f, 25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[ShowBoss][1]).c_str(), false, 0.15f);  // Convert std::string to const char*
		DrawTextAdvanced(-400.f, -25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[ShowBoss][1]).c_str(), false, 0.125f);  // Convert std::string to const char*
		DrawTextAdvanced(-400.f, -50.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[ShowBoss][1]).c_str(), false, 0.125f);  // Convert std::string to const char*

		// Top 3
		DrawTextAdvanced(400.f, -25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[ShowBoss][2]).c_str(), false, 0.15f);  // Convert std::string to const char*
		DrawTextAdvanced(400.f, -75.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[ShowBoss][2]).c_str(), false, 0.125f);  // Convert std::string to const char*
		DrawTextAdvanced(400.f, -100.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[ShowBoss][2]).c_str(), false, 0.125f);  // Convert std::string to const char*

		// Show More
		DrawRect(600.f, 0.f, 100.f, 100.f, SetColor(0xFFFFFFFF));
		DrawTextAdvanced(1200.f, 0.f, pixel_font, SetColor(0xFF000000), ">>", false, 0.3f);
	}
	// Draw top 20
	else {
		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 20!", false);
		DrawTextAdvanced(-400.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "-NAME-", false, 0.2f);
		DrawTextAdvanced(-100.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "-SCORE-", false, 0.2f);
		DrawTextAdvanced(300.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "-TIME-", false, 0.2f);

		// Draws out neat list of 20 names, score, timing for each specific boss!
		for (int i = 0; i < 20; i++) 
		{
			DrawTextAdvanced(-400.f, 500.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*
			DrawTextAdvanced(-100.f, 500.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*
			DrawTextAdvanced(300.f, 500.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*

		}

		// Show Less
		DrawRect(-600.f, 0.f, 100.f, 100.f, SetColor(0xFFFFFFFF));
		DrawTextAdvanced(-1200.f, 0.f, pixel_font, SetColor(0xFF000000), "<<", false, 0.3f);
	}
}

// Draw Modifiers UI screen!
void Draw_ModifiersBoard() 
{
	// UI Screens for factors that adjust gameplay
	DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "MODIFIERS", false);
	DrawTextAdvanced(0.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "NOTE: This will affect your score", false, 0.3f);

	// DAMAGE TAKEN MODIFIER
	// Input for selecting 'difficulty'
	if ((IsAreaClicked(175.f, 150.f, 50.f, 50.f, s32(Mouse.x), s32(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
	{
		player.Mod_DmgTakenIndex++;
		if (player.Mod_DmgTakenIndex > 3) player.Mod_DmgTakenIndex = 3;
	}
	else if ((IsAreaClicked(-175.f, 150.f, 50.f, 50.f, s32(Mouse.x), s32(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) 
	{
		player.Mod_DmgTakenIndex--;
		if (player.Mod_DmgTakenIndex < 0) player.Mod_DmgTakenIndex = 0;
	}
	// Draw extra text details!
	switch (player.Mod_DmgTakenIndex)
	{
	case 0:
		DrawTextAdvanced(0.f, 200.f, pixel_font, SetColor(0xFFFF0000), "NO SCORE!", false, 0.3f);
		DrawTextAdvanced(0.f, 150.f, pixel_font, SetColor(0x50000000), "...we won't judge...", false, 0.2f);
		break;
	case 1:
		DrawTextAdvanced(0.f, 200.f, pixel_font, SetColor(0xFFFF0000), "Reduced score!", false, 0.3f);
		break;
	default:
		break;
	case 3:
		DrawTextAdvanced(0.f, 200.f, pixel_font, SetColor(0xFF00FF00), "Bonus score!", false, 0.3f);
		break;
	}
	// Draw!
	DrawTextAdvanced(0.f, 400.f, pixel_font, SetColor(0xFFFFFFFF), "Damage Taken:", false, 0.3f);
	DrawTextAdvanced(0.f, 300.f, pixel_font, SetColor(0xFFFFFFFF), Mod_DmgTaken[player.Mod_DmgTakenIndex], false, 0.3f);
	DrawRect(175.f, 150.f, 50.f, 50.f, SetColor(0xFFFFFFFF));
	DrawRect(-175.f, 150.f, 50.f, 50.f, SetColor(0xFFFFFFFF));
	DrawTextAdvanced(-350.f, 300.f, pixel_font, SetColor(0xFF000000), "<<", false, 0.3f);
	DrawTextAdvanced(350.f, 300.f, pixel_font, SetColor(0xFF000000), ">>", false, 0.3f);
}
#pragma endregion
