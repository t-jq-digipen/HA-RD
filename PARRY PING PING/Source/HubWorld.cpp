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
#include <string>
#include <cstdlib>
#include <cstdio>  // For popen()
#include <direct.h>
#include <thread>
#include <atomic>
#include <future>



// Variables for parry bot init
AEVec2 DummyMelee_pos{}, DummyRange_pos{};
EnvStruct Env_Gate{ 0.f, 550.f, 500.f, 100.f }, Env_Campfire{ -50.f, 0.f, 100.f, 100.f }, Env_Shrine{ -600.f, -200.f, 70.f, 150.f }, Env_Modifiers{ -600.f, 300.f, 100.f, 200.f };
EnvStruct Txt_Controls{ 0.f, 200.f, 200.f, 100.f }, Txt_Movement{ 0.f, 150.f, 200.f, 100.f }, Txt_Attack{ 0.f, -100.f, 200.f, 100.f };
f32 DummyMelee_rad{}, CurrHealth{}, MaxHealth{ 50.f };

// TEMPORARY - Placeholder for sprites to be added
Color hubworld_white;
s8  hubworld_font;

// Fade from black transition when level starts
Color FadeIn, FadeGate, FadeShrine;
f32 FadeInSpeed;

int DummyMelee_Check;										// Check for dummy logic

Timer HubWorld_ClearTransitionTimer;						// For Level states
float HubWorld_LevelTimer;

bool InShrine;
bool Draw_Top3;


//Leaderboard fetching
std::vector<std::string> HS_name(20);
std::vector<std::string> HS_score(20);
std::vector<std::string> HS_time(20);
std::atomic<bool> isFetchingLeaderboard = false;

void HubWorld_Load()
{
	Load_Player();											// Load Player

	Load_DummyBot();

	Load_Tutorial();

	FetchLeaderboardAsync();

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

	hubworld_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 36);		// Font and text colour
	hubworld_white = SetColor(0xFFFFFFFF);

	FadeIn = FadeGate = FadeShrine = SetColor(0xFF000000);											// For Fade-From_Black transition as level starts
	FadeInSpeed = 0.03f;
	FadeShrine.A = FadeGate.A = 0.f;

	InShrine = false;
	Draw_Top3 = true;


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

	if (AEInputCheckCurr(AEVK_1)) {
		CamPanning = true;
		Pan_Camera(0.f, 500.f, 10.f);
	}
	if (AEInputCheckCurr(AEVK_2)) {
		CamPanning = false;
	}

	if (levelState != PAUSED && levelState != GAMEOVER)
	{
		// Player WASD Movement
		Update_Player();
		player.hp = 100.f;

		DummyMelee_Check = Attacked_Melee(DummyMelee_pos);					// Check for Player-Attacking-Bot detection
		Active_DummyBotMelee();												// Update logic for dummies
		Active_DummyBotRange();

		Update_Tutorial();

		// Incrementing level timer
		//if (levelState != CLEARED)
		//{
		//	Update_LevelTimer(&HubWorld_LevelTimer);						// Update level timer when level is ongoing
		//}
		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width, Env_Gate.Height)) {
			if (AEInputCheckTriggered(AEVK_F)) 
			{
				nextState = GS_LEVEL1;
			}
		}
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE) && levelState == PLAYING)				// Pause menu when escape key is pressed!
	{
		levelState = PAUSED;
		InShrine = false;
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
	DrawRect(World_To_Screen_x(Env_Campfire.Pos.x), World_To_Screen_y(Env_Campfire.Pos.y), Env_Campfire.Width, Env_Campfire.Height, SetColor(0xFFFF6961));		// Campfire - Center of level, where player spawns when Hub World loads
	DrawTextAdvanced(Env_Campfire.Pos.x, Env_Campfire.Pos.y, hubworld_font, hubworld_white, "CAMPFIRE", true);

	DrawRect(World_To_Screen_x(Env_Modifiers.Pos.x), World_To_Screen_y(Env_Modifiers.Pos.y), Env_Modifiers.Width, Env_Modifiers.Height, SetColor(0xFFFF6961));		// Modifier Station (TO BE IMPLEMENTED!) - Modify settings for the upcoming level
	DrawTextAdvanced(Env_Modifiers.Pos.x, Env_Modifiers.Pos.y, hubworld_font, hubworld_white, "MODIFIERS [WIP]", true);

	DrawTextAdvanced(DummyMelee_pos.x - 30.f, DummyMelee_pos.y + 50.f, hubworld_font, hubworld_white, "Melee Dummy", true);
	DrawTextAdvanced(DummyRange_pos.x + 30.f, DummyRange_pos.y + 50.f, hubworld_font, hubworld_white, "Ranged Dummy", true);

#pragma region old tutorial

	// Draw Instructions
	/*if (AreRectanglesIntersecting(player.pos.x, player.pos.y, 50.f, 50.f, Env_Campfire.Pos.x, Env_Campfire.Pos.y, Env_Campfire.Width, Env_Campfire.Height))
	{
		DrawTextAdvanced(Txt_Controls.Pos.x, Txt_Controls.Pos.y, hubworld_font, hubworld_white, "CONTROLS:", true);
		DrawTextAdvanced(Txt_Movement.Pos.x, Txt_Movement.Pos.y, hubworld_font, hubworld_white, "WASD to move, Space to Dash", true);
		DrawTextAdvanced(Txt_Attack.Pos.x, Txt_Attack.Pos.y, hubworld_font, hubworld_white, "ATTACKS:", true);
		DrawTextAdvanced(Txt_Attack.Pos.x, Txt_Attack.Pos.y - 50.f, hubworld_font, hubworld_white, "Left Click - Melee Attack", true);
		DrawTextAdvanced(Txt_Attack.Pos.x, Txt_Attack.Pos.y - 100.f, hubworld_font, hubworld_white, "Right Click - Shoot (up to 3 ammo)", true);
		DrawTextAdvanced(Txt_Attack.Pos.x, Txt_Attack.Pos.y - 150.f, hubworld_font, hubworld_white, "Right Hold - *FIRE LASER (requires 3 ammo)", true);
		DrawTextAdvanced(Txt_Attack.Pos.x, Txt_Attack.Pos.y - 200.f, hubworld_font, hubworld_white, "*Laser causes player to recoil.", true);
	}*/

	/*if (AreRectanglesIntersecting(player.pos.x, player.pos.y, 50.f, player.height, DummyMelee_pos.x, DummyMelee_pos.y - 50.f, player.width, player.height) ||
		AreRectanglesIntersecting(player.pos.x, player.pos.y, 50.f, player.height, DummyRange_pos.x, DummyRange_pos.y - 50.f, player.width, player.height))
	{
		DrawTextAdvanced(DummyMelee_pos.x + 100.f, DummyMelee_pos.y + 150.f, hubworld_font, hubworld_white, "Press E to parry these attacks!", true);
		DrawTextAdvanced(DummyMelee_pos.x + 50.f, DummyMelee_pos.y + 100.f, hubworld_font, hubworld_white, "Parrying takes time to recharge, use wisely!", true);
	}*/
#pragma endregion

	//Highlight effect when approaching gate
	if (AreRectanglesIntersecting(player.pos.x, player.pos.y, 50.f, player.height, Env_Gate.Pos.x - (Env_Gate.Width / 2.f), Env_Gate.Pos.y, Env_Gate.Width, Env_Gate.Height))
	{
		Draw_FadeOut(FadeGate);

		if (AETestPointToRect(&player.pos, &Env_Gate.Pos, Env_Gate.Width, Env_Gate.Height)) {
			DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y + 100.f, hubworld_font, hubworld_white, "PRESS [F]", true);
		}
		else {
			DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y + 100.f, hubworld_font, hubworld_white, "JUST DO IT!", true);
		}	
	}
	else 
	{
		Draw_FadeIn(FadeGate);
	}
	DrawRect(World_To_Screen_x(Env_Gate.Pos.x), World_To_Screen_y(Env_Gate.Pos.y), Env_Gate.Width, Env_Gate.Height, SetColor(0xFFFF6961));			// Gate - player starts boss fight level here!
	DrawTextAdvanced(Env_Gate.Pos.x, Env_Gate.Pos.y, hubworld_font, hubworld_white, "GATE", true);


	//Highlight effect when approaching leaderboard shrine
	if (AreRectanglesIntersecting(player.pos.x, player.pos.y, player.height, player.height, Env_Shrine.Pos.x, Env_Shrine.Pos.y, Env_Shrine.Width * 3.f, Env_Shrine.Height * 1.5f))
	{
		Draw_FadeOut(FadeShrine);
		hubworld_white.A = FadeShrine.A;
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
		hubworld_white.A = 1.f;
		
		if (AEInputCheckTriggered(AEVK_F) && InShrine == false)
		{
			InShrine = true;
		}
		else if (AEInputCheckTriggered(AEVK_F) && InShrine == true)
		{
			InShrine = false;
		}
	}
	else
	{
		InShrine = false;
		Draw_FadeIn(FadeShrine);
		hubworld_white.A = FadeShrine.A;
		DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y + Env_Shrine.Height / 1.25f, hubworld_font, hubworld_white, "PRESS [F]", true);
		hubworld_white.A = 1.f;
	}
	DrawRect(World_To_Screen_x(Env_Shrine.Pos.x), World_To_Screen_y(Env_Shrine.Pos.y), Env_Shrine.Width, Env_Shrine.Height, SetColor(0xFFFF6961));		// Leaderboard Shrine (TO BE IMPLEMENTED!) - Top 3 recorded runs shown here
	DrawTextAdvanced(Env_Shrine.Pos.x, Env_Shrine.Pos.y, hubworld_font, hubworld_white, "SHRINE [WIP]", true);

	// Player
	Draw_Player();				// Draw player sprites 

	// Tutorial
	Draw_Tutorial();

	// UI
	// !!!TO DO!!! can try add system with like HUD that only shows when u are attacking or doing actions, and disappears when not doing anyth so it looks more cinematic and cool 
	Draw_UI(0);					// Draw UI - (0) added to not draw boss healthbar

	// Level Timer
	// Draw_LevelTimer(HubWorld_LevelTimer);		// Draw timer on bottom right

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
		AEInputShowCursor(1);
	}
	else if (levelState == CLEARED)
	{
		if (!Update_Timer(&HubWorld_ClearTransitionTimer))
		{
			Draw_ClearMenu();					// Draw level cleared menu when level is cleared
		}
	}

	if (levelState == OPTIONS || levelState == CLEARED || levelState == GAMEOVER || levelState == PAUSED || InShrine) {
		AEInputShowCursor(1);													// Show cursor in these screens
	}

	if (InShrine) {
		Pan_Camera(0.f, 500.f, 10.f);
		CamPanning = true;
		Draw_ShrineBoard();
	}
	else {
		CamPanning = false;
		Draw_Top3 = true;
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
	AEInputShowCursor(1);
}

void HubWorld_Unload()
{
	AEInputShowCursor(1);
	Unload_UI();																						// Free memory! Unloads UI, Meshes and Player!
	Unload_Tutorial();
	Unload_Mesh();
	Unload_Player();
	Unload_DummyBot();
	std::cout << "HubWorld_unload";
}

void Draw_FadeIn(Color& Fade) {
	FadeInSpeed = 0.03f;
	LockCamera = false;
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
	LockCamera = true;
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), Fade);			// Just adjusts alpha of a black screen drawn over the level when the level starts
	Fade.A += FadeInSpeed;
	FadeInSpeed *= 1.05f;
	if (InShrine) {
		if (Fade.A > .9f) Fade.A = .9f;
		return;
	}
	if (Fade.A > .7f) Fade.A = .7f;
}

//TEMP HIGHSCORES AND SHIII
// can probs make highscores easier to do by having a 'player profile' struct or smt that holds a name score and time
// struct can hold pos on screen to print, name, score, time, order in list? (maybe not bc alr sorted on google api), then can do array of structs or smth and just print
//FAUZAN UR STUFF IS HERE!!!!
//I might implement a cool little swipe to left/right transition but ngl idk if we got time so can just implement the google doc names teehee tyvm

void fetchLeaderboard() {
	// Get current directory
	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);
	std::string currentDir(buffer);

	// Run Python script to generate leaderboard.txt
	std::string command = "\"" + currentDir + "\\Assets\\dist\\leaderboard_helper.exe\"";
	int result = system(command.c_str());
	if (result != 0) {
		std::cerr << "Failed to run leaderboard_helper.exe" << std::endl;
		return;
	}

	// Open leaderboard.txt
	std::ifstream file("leaderboard.txt");
	if (!file) {
		std::cerr << "Failed to open leaderboard.txt" << std::endl;
		return;
	}

	std::vector<std::string> names, scores, times;
	std::string line;

	// Read leaderboard.txt line by line
	while (std::getline(file, line)) {
		std::string name, score, time;

		// Split using " - " as the delimiter
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

	// Store in game leaderboard variables
	for (size_t i = 0; i < names.size() && i < 20; i++) {
		HS_name[i] = names[i];
		HS_score[i] = scores[i];
		HS_time[i] = times[i];
	}

	std::cout << "Leaderboard fetched and stored successfully!" << std::endl;
	//// After populating the arrays
	//std::cout << "Leaderboard Entries:" << std::endl;
	//for (int i = 0; i < 20; i++) {
	//	std::cout << "Name: " << HS_name[i] << " | Score: " << HS_score[i] << " | Time: " << HS_time[i] << std::endl;
	//}
	std::cout << "Current directory: " << currentDir << std::endl;


}

std::atomic<bool> isFetchingLeaderboard = false;

void FetchLeaderboardAsync() {
	if (isFetchingLeaderboard.load()) return; // Prevent multiple fetch calls
	isFetchingLeaderboard = true;

	std::thread([]() {
		fetchLeaderboard(); // Fetch leaderboard data
		isFetchingLeaderboard = false;
		}).detach();
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
	if ((IsAreaClicked(600.f, 0.f, 100.f, 100.f, Mouse.x, Mouse.y) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		Draw_Top3 = false;
	}
	else if ((IsAreaClicked(-600.f, 0.f, 100.f, 100.f, Mouse.x, Mouse.y) && AEInputCheckTriggered(AEVK_LBUTTON))) {
		Draw_Top3 = true;
	}

	if (Draw_Top3) {
		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 3!", false);

		DrawRect(0.f, 100.f, 100.f, 100.f, SetColor(0xFF88B984));
		DrawRect(-200.f, 75.f, 100.f, 100.f, SetColor(0xFF00008B));
		DrawRect(200.f, 50.f, 100.f, 100.f, SetColor(0xFFFF6961));

		// Top 1
		DrawTextAdvanced(0.f, 75.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[0]).c_str(), false, 0.15);  // Convert std::string to const char*
		DrawTextAdvanced(0.f, 25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[0]).c_str(), false, 0.125);  // Convert std::string to const char*
		DrawTextAdvanced(0.f, 0.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[0]).c_str(), false, 0.125);  // Convert std::string to const char*

		// Top 2
		DrawTextAdvanced(-400.f, 25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[1]).c_str(), false, 0.15);  // Convert std::string to const char*
		DrawTextAdvanced(-400.f, -25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[1]).c_str(), false, 0.125);  // Convert std::string to const char*
		DrawTextAdvanced(-400.f, -50.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[1]).c_str(), false, 0.125);  // Convert std::string to const char*

		// Top 3
		DrawTextAdvanced(400.f, -25.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[2]).c_str(), false, 0.15);  // Convert std::string to const char*
		DrawTextAdvanced(400.f, -75.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[2]).c_str(), false, 0.125);  // Convert std::string to const char*
		DrawTextAdvanced(400.f, -100.f, pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[2]).c_str(), false, 0.125);  // Convert std::string to const char*

		// Show More
		DrawRect(600.f, 0.f, 100.f, 100.f, SetColor(0xFFFFFFFF));
	}
	else {
		DrawTextAdvanced(0.f, 700.f, pixel_font, SetColor(0xFFFFFFFF), "TOP 20!", false);

		for (int i = 0; i < 20; i++) {
			// Reuse the X and Y positions for each row from the existing code
			DrawTextAdvanced(-300.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_name[i]).c_str(), false, 0.2);  // Convert std::string to const char*
			DrawTextAdvanced(0.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_score[i]).c_str(), false, 0.2);  // Convert std::string to const char*
			DrawTextAdvanced(300.f, 550.f - (i * 50.f), pixel_font, SetColor(0xFFFFFFFF), sanitizeString(HS_time[i]).c_str(), false, 0.2);  // Convert std::string to const char*

		}

		// Show Less
		DrawRect(-600.f, 0.f, 100.f, 100.f, SetColor(0xFFFFFFFF));
	}
}