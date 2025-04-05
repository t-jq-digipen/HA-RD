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
//EnvStruct Env_Gate{ 0.f, 550.f, 500.f, 100.f }, Env_Campfire{ -50.f, 0.f, 100.f, 100.f }, Env_Shrine{ -600.f, -200.f, 70.f, 150.f }, Env_Modifiers{ -600.f, 300.f, 100.f, 200.f };
EnvStruct Env_Gate, Env_Gate2, Env_Campfire, Env_Shrine, Env_Modifiers;
// EnvStruct Txt_Controls{ 0.f, 200.f, 200.f, 100.f }, Txt_Movement{ 0.f, 150.f, 200.f, 100.f }, Txt_Attack{ 0.f, -100.f, 200.f, 100.f };
EnvStruct Txt_Controls, Txt_Movement, Txt_Attack;
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{ 50.f };

// TEMPORARY - Placeholder for sprites to be added
Color hubworld_white;
s8  hubworld_font;

// Spritesheets
SpriteSheet campfire_sprite;
SpriteSheet shrine_sprite;

// Fade from black transition when level starts
Color FadeIn, FadeGate, FadeShrine, FadeModifiers;
f32 FadeInSpeed;

int DummyMelee_Check;										// Check for dummy logic

Timer HubWorld_ClearTransitionTimer;						// For Level states
float HubWorld_LevelTimer;

int ShowBoss;												// Show boss 1/ 2  (0 & 1 respectively)
bool InTop20;												// For Top 20 highscores
bool Draw_Top3;												// For Top 3 screen
bool InModifiers;											// For modifier selection screen

//Leaderboard fetching
using ArrOfVecs = std::vector<std::string>[2];
ArrOfVecs HS_name;
ArrOfVecs HS_score;
ArrOfVecs HS_time;

//std::vector<std::string> HS_name(20);
//std::vector<std::string> HS_score(20);
//std::vector<std::string> HS_time(20);
std::atomic<bool> isFetchingLeaderboard = false;
// Global future to track the async fetch
std::future<void> leaderboardFetchFuture;

// File IO
std::vector<float> HubWorld_IO_elements;

AEGfxTexture* loadingImage1;
float rotationAngle1 = 0.f; // Angle for rotation of the loading image
float rotationSpeed1 = 90.f; // Rotation speed (degrees per second)

void DrawLoadingImage1() {
	Color grey = { 0.5f, 0.5f, 0.5f, 1.f };
	if (isFetchingLeaderboard.load()) {  // Only draw loading image while fetching
		// Draw the image with a rotation angle
		DrawRect(0.f, 0.f, 1600.f, 900.f, grey); // RGBA for grey color with transparency (0.5f opacity)
		DrawImage(0.f, 0.f, 1280.f, 720.f, 0.f, loadingImage1);  // Full screen translucent background
	}
}



void FetchLeaderboardAsync() {
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
void WaitForLeaderboardFetch() {
	if (leaderboardFetchFuture.valid()) {
		leaderboardFetchFuture.get(); // Wait for completion
	}
}

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

	shrine_sprite	= SpriteSheet("Assets/HubWorld/Shrine.png", 13, 1);
	campfire_sprite = SpriteSheet("Assets/HubWorld/Campfire.png", 12, 1);

	// Meshes
	Load_Mesh(20);
	std::cout << "Hub World Loaded\n";

	// Read File IO
	ReadFromFile("Assets/FileIO/HubWorld.txt", HubWorld_IO_elements);

	// Hub World Environments
	Env_Gate		= { HubWorld_IO_elements[0],  HubWorld_IO_elements[1],  HubWorld_IO_elements[2],  HubWorld_IO_elements[3] };	// ENV GATE 
	Env_Gate2		= { HubWorld_IO_elements[4],  HubWorld_IO_elements[5],  HubWorld_IO_elements[6],  HubWorld_IO_elements[7] };	// ENV GATE2
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

	HubWorld_IO_elements.clear();
}

void HubWorld_Initialize()
{
	std::cout << "Hub World Initialized\n";

	// Dummy enemy
	// Reset melee check
	DummyMelee_Check = 0;

	ShowBoss = 0;
	InTop20 = false;
	Draw_Top3 = true;
	InModifiers = false;
	player.Mod_DmgTakenIndex = 1;

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

	Init_DummyBotMelee(DummyMelee_pos, DummyMelee_rad, MaxHealth);			// Init the dummies with specified stats!
	Init_DummyBotRange(DummyRange_pos, DummyMelee_rad, MaxHealth);


	// UI
	Init_UI();																// Init UI

	// Tutorial
	Init_Tutorial();														// Init Tutorial elements

	// Reset Level Timer
	HubWorld_LevelTimer = 0.f;												// LEVEL TIMER		0.f

	// Reset Level Clear Transition Timer
	HubWorld_ClearTransitionTimer.current = 0.f;							// START CLEAR TRANSITION	0.f
}

void HubWorld_Update()
{
	// Delta time
	Get_DT();

	if (AEInputCheckTriggered(AEVK_B))
	{
		nextState = GS_RESTART;												// Press B to restart level
	}


	//REMOVE AFTER CLEANUP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (AEInputCheckCurr(AEVK_1)) {
		CamPanning = true;
		Pan_Camera(0.f, 500.f, 10.f);
	}
	if (AEInputCheckCurr(AEVK_2)) {
		CamPanning = false;
	}

	if (levelState == PLAYING)
	{
		std::cout << "BGM Volume: " << current_bgm_vol << '\n';
		std::cout << "SFX Volume: " << current_sfx_vol << '\n';
		// Player WASD Movement
		Update_Player();
		player.hp = 100.f;

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);					// Check for Player-Attacking-Bot detection
		Active_DummyBotMelee();												// Update logic for dummies
		Active_DummyBotRange();

		Update_Tutorial();

		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width/2.f, Env_Gate.Height)) {
			if (AEInputCheckTriggered(AEVK_F)) 
			{
				LockCamera = false;
				player.canAttack = true;
				nextState = GS_LEVEL1;
			}
		}
		else if (AETestPointToRect(&player.pos, &Env_Gate2.Pos, Env_Gate2.Width / 2.f, Env_Gate2.Height)) {
			if (AEInputCheckTriggered(AEVK_F))
			{
				LockCamera = false;
				player.canAttack = true;
				nextState = GS_LEVEL2;
			}
		}

		if ((AETestPointToRect(&player.pos, &Env_Shrine.Pos, Env_Shrine.Width * 5.f, Env_Shrine.Height * 2.5f)) || AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width * 4.f, Env_Gate.Height * 2.f))
		{
			LockCamera = true;
			player.canAttack = false;
		}

		if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)	// Pause menu when escape key is pressed!
		{
			levelState = PAUSED;
			InTop20 = false;
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
	if ((AreRectanglesIntersecting(player.pos.x, player.pos.y, player.height, player.height, Env_Shrine.Pos.x, Env_Shrine.Pos.y, Env_Shrine.Width, Env_Shrine.Height)) ||
			AreRectanglesIntersecting(player.pos.x, player.pos.y, 50.f, player.height, Env_Gate.Pos.x - (Env_Gate.Width / 2.f), Env_Gate.Pos.y, Env_Gate.Width, Env_Gate.Height))
	{
		LockCamera = true;
		player.canAttack = false;
	}
	else {
		LockCamera = false;
		player.canAttack = true;
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

	// Campfire
	campfire_sprite.Set_Row(0);
	campfire_sprite.Loop(0, 11, 1.f);
	campfire_sprite.Draw(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height);

	DrawTextAdvanced(DummyMelee_pos.x - 30.f, DummyMelee_pos.y + 50.f, hubworld_font, hubworld_white, "Melee Dummy", true);
	DrawTextAdvanced(DummyRange_pos.x + 30.f, DummyRange_pos.y + 50.f, hubworld_font, hubworld_white, "Ranged Dummy", true);

	//Highlight effect when approaching gate
	if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width * 4.f, Env_Gate.Height * 2.f))
	{
		Draw_FadeOut(FadeGate);
		if (AETestPointToRect(&player.pos, &Env_Modifiers.Pos, Env_Modifiers.Width * 3.f, Env_Modifiers.Height * 1.5f)) {
			DrawTextAdvanced(Env_Modifiers.Pos.x, Env_Modifiers.Pos.y + Env_Modifiers.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
			if (AEInputCheckTriggered(AEVK_F) && InModifiers == false)
			{
				InModifiers = true;
			}
			else if (AEInputCheckTriggered(AEVK_F) && InModifiers == true)
			{
				InModifiers = false;
				ResetCamera();
			}
		}
		else {
			InModifiers = false;
			Draw_FadeIn(FadeShrine);
		}

		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width/2.f, Env_Gate.Height)) {
			DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y + 100.f, hubworld_font, hubworld_white, "PRESS [F]", true);
		}
		else if (AETestPointToRect(&player.pos, &Env_Gate2.Pos, Env_Gate2.Width / 2.f, Env_Gate2.Height)) {
			DrawTextAdvanced(Env_Gate2.Pos.x, Env_Gate2.Pos.y + 100.f, hubworld_font, hubworld_white, "PRESS [F]", true);
		}
	}
	else 
	{
		Draw_FadeIn(FadeGate);
		InModifiers = false;
	}

	DrawRect(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, SetColor(0xFFFF6961));		// Modifier Station (TO BE IMPLEMENTED!) - Modify settings for the upcoming level
	DrawTextAdvanced(Env_Modifiers.Pos.x, Env_Modifiers.Pos.y, hubworld_font, hubworld_white, "MODIFIERS [WIP]", true);

	DrawRect(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width, Env_Gate.Height, SetColor(0xFFFF6961));			// Gate - player starts boss fight level here!
	DrawRect(World_To_Screen_x(Env_Gate2.Pos.x), World_To_Screen_y(Env_Gate2.Pos.y), Env_Gate2.Width, Env_Gate2.Height, SetColor(0xFFFF6961));			// Gate 2 - player starts boss fight level here!
	DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y, hubworld_font, hubworld_white, "GATE 1", true);
	DrawTextAdvanced(Env_Gate2.Pos.x, Env_Gate2.Pos.y, hubworld_font, hubworld_white, "GATE 2", true);


	//Highlight effect when approaching leaderboard shrine
	if (AETestPointToRect(&player.pos, &Env_Shrine.Pos, Env_Shrine.Width, Env_Shrine.Height))
	{
		Draw_FadeOut(FadeShrine);
		hubworld_white.A = FadeShrine.A;
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
		hubworld_white.A = 1.f;
		
		if (AEInputCheckTriggered(AEVK_F) && InTop20 == false)
		{
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
		Draw_FadeIn(FadeShrine);
		hubworld_white.A = FadeShrine.A;
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
		hubworld_white.A = 1.f;
	}

	shrine_sprite.Set_Row(0);
	shrine_sprite.Loop(0, 12, 1.f);
	shrine_sprite.Draw(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height);

	// Player
	Draw_Player();				// Draw player sprites 

	// Tutorial
	Draw_Tutorial();

	// UI
	Draw_UI(0);					// Draw UI - (0) added to not draw boss healthbar

	AEInputShowCursor(0);						// Hides cursor
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

	if (levelState == OPTIONS || levelState == CLEARED || levelState == GAMEOVER || levelState == PAUSED || levelState == INQUIT || InTop20 || InModifiers) {
		AEInputShowCursor(1);													// Show cursor in these screens
	}

	if (InTop20) {
		Pan_Camera(0.f, 500.f, 10.f);
		CamPanning = true;
		Draw_ShrineBoard();
	}
	else {
		CamPanning = false;
		Draw_Top3 = true;
	}

	if (InModifiers) {
		Pan_Camera(0.f, 500.f, 10.f);
		CamPanning = true;
		Draw_ModifiersBoard();
	}
	else {
		CamPanning = false;
		InModifiers = false;
	}

	//cool little fade in effect
	if (FadeIn.A > 0) {
		DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), FadeIn);			// Just adjusts alpha of a black screen drawn over the level when the level starts
		FadeIn.A -= FadeInSpeed;
	}
	else FadeIn.A = 0.f;																				// Clamp to 0
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);										// Set render modes for drawing of colours
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	DrawLoadingImage1();
}

void HubWorld_Free()
{
	Free_Player();																						// Free memory!
	AEInputShowCursor(1);
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
	std::cout << "HubWorld_unload";

	campfire_sprite.Unload();
	shrine_sprite.Unload();
	WaitForLeaderboardFetch();
	AEGfxTextureUnload(loadingImage1);

}

void Draw_FadeIn(Color& Fade) {
	FadeInSpeed = 0.03f;
	if (Fade.A < 0.f) {
		Fade.A = 0.f;
		return;
	}
	else{
		DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), Fade);
		Fade.A -= FadeInSpeed;
	}
}

void Draw_FadeOut(Color& Fade) {
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), Fade);			// Just adjusts alpha of a black screen drawn over the level when the level starts
	Fade.A += FadeInSpeed;
	FadeInSpeed *= 1.05f;
	if (InTop20) {
		if (Fade.A > .9f) Fade.A = .9f;
		return;
	}
	if (Fade.A > .7f) Fade.A = .7f;
}

#pragma region Shrine LeaderBoard 
//TEMP HIGHSCORES AND SHIII
// can probs make highscores easier to do by having a 'player profile' struct or smt that holds a name score and time
// struct can hold pos on screen to print, name, score, time, order in list? (maybe not bc alr sorted on google api), then can do array of structs or smth and just print
//FAUZAN UR STUFF IS HERE!!!!
//I might implement a cool little swipe to left/right transition but ngl idk if we got time so can just implement the google doc names teehee tyvm


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

	std::cout << "Leaderboard fetched and stored for Boss " << bossID + 1 << "!" << std::endl;
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

void Draw_ShrineBoard() {
	if ((IsAreaClicked(600.f, 0.f, 100.f, 100.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		Draw_Top3 = false;
	}
	else if ((IsAreaClicked(-600.f, 0.f, 100.f, 100.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		Draw_Top3 = true;
	}

	if (Draw_Top3) {
		if ((IsAreaClicked(200.f, 275.f, 75.f, 75.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
			ShowBoss = 1;
		}
		else if ((IsAreaClicked(-200.f, 275.f, 75.f, 75.f, static_cast<s32>(Mouse.x), static_cast<s32>(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
			ShowBoss = 0;
		}

		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 3!", false);

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

		DrawRect(0.f, 100.f, 100.f, 100.f, SetColor(0xFF88B984));
		DrawRect(-200.f, 75.f, 100.f, 100.f, SetColor(0xFF00008B));
		DrawRect(200.f, 50.f, 100.f, 100.f, SetColor(0xFFFF6961));

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
	else {
		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 20!", false);

		for (int i = 0; i < 20; i++) {
			// Reuse the X and Y positions for each row from the existing code
			DrawTextAdvanced(-300.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*
			DrawTextAdvanced(0.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*
			DrawTextAdvanced(300.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[ShowBoss][i]).c_str(), false, 0.2f);  // Convert std::string to const char*

		}

		// Show Less
		DrawRect(-600.f, 0.f, 100.f, 100.f, SetColor(0xFFFFFFFF));
		DrawTextAdvanced(-1200.f, 0.f, pixel_font, SetColor(0xFF000000), "<<", false, 0.3f);
	}
}

void Draw_ModifiersBoard() {
	DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "MODIFIERS", false);
	DrawTextAdvanced(0.f, 550.f, pixel_font, SetColor(0xFFFFFFFF), "NOTE: This will affect your score", false, 0.3f);

	DrawTextAdvanced(0.f, 400.f, pixel_font, SetColor(0xFFFFFFFF), "Damage Taken:", false, 0.3f);
	const char* Mod_DmgTaken[3] {"Vadim Surov", "Default", "We ball"};
	DrawTextAdvanced(0.f, 300.f, pixel_font, SetColor(0xFFFFFFFF), Mod_DmgTaken[player.Mod_DmgTakenIndex], false, 0.3f);
	DrawRect(150.f, 150.f, 50.f, 50.f, SetColor(0xFFFFFFFF));
	DrawRect(-150.f, 150.f, 50.f, 50.f, SetColor(0xFFFFFFFF));
	DrawTextAdvanced(-300.f, 300.f, pixel_font, SetColor(0xFF000000), "<<", false, 0.3f);
	DrawTextAdvanced(300.f, 300.f, pixel_font, SetColor(0xFF000000), ">>", false, 0.3f);

	//DRAW HERE AND GET INPUT TO CHOOSE INPUT

	if ((IsAreaClicked(150.f, 150.f, 50.f, 50.f, s32(Mouse.x), s32(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		player.Mod_DmgTakenIndex++;
		if (player.Mod_DmgTakenIndex > 2) player.Mod_DmgTakenIndex = 2;
	}
	else if ((IsAreaClicked(-150.f, 150.f, 50.f, 50.f, s32(Mouse.x), s32(Mouse.y)) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		player.Mod_DmgTakenIndex--;
		if (player.Mod_DmgTakenIndex < 0) player.Mod_DmgTakenIndex = 0;
	}
	switch (player.Mod_DmgTakenIndex)
	{
	case 0:
		DrawTextAdvanced(0.f, 200.f, pixel_font, SetColor(0xFFFF0000), "Reduced score!", false, 0.3f);
		break;
	default:
		break;
	case 2:
		DrawTextAdvanced(0.f, 200.f, pixel_font, SetColor(0xFF00FF00), "Bonus score!", false, 0.3f);
		break;
	}
}
#pragma endregion
