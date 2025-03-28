//---------------------------------------------------------
// file:	UI.cpp
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	UI elements
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "UI.h"
#include "GameStateManager.h"
#include "SpriteSheet.h"
#include "player.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <direct.h>
#include <thread>

// Level number
int levelState = 0;
float levelTimer = 0.f;

// Level score
int currentScore = 0;
int newScore = 0;
int pointsAccumulated = 0;
bool displayPointsAccumulated = false;
Timer displayPointsTimer;
int currentCombo = 0;
int highestCombo = 0;
bool isPerfect = false;

// Font, colors, mouse coordinates
s8 UI_font;
s8 pixel_font;
Color UI_red, UI_green, UI_blue, UI_black, UI_white, UI_hover_red, UI_purple, UI_gold;
s32 UI_mouseX, UI_mouseY;

// Player UI
float UI_playerhp, UI_playermaxhp;
int UI_ammo, UI_maxammo;
float UI_parryCD;
Color empty_ammo, full_ammo, health, parry_charging, parry_ready;

// Boss UI
float UI_bosshp, UI_bossmaxhp;

// UI Colors
Color play_bg, pause_bg, pause_screen;

// Boss HP Colors
Color boss_hp_red, boss_hp_yellow, boss_hp_green, boss_hp_damage;

// Level Timer
char level_timer[16];
std::string levelTimeStr;

// Level Score
char points[16];
char level_score[16];
char score_type[16];
char combo[16];
const s32 NUM_OF_SCORE_TYPES = 9;
const char* SCORE_TYPE_DISPLAY[NUM_OF_SCORE_TYPES] = { "HIT!", "BIG HIT!", "DODGE!", "PARRY!!!", "OUCH", "BLOCKED!", "RIP", "TIME BONUS!", "PERFECT!!!"};
Color score_color;

// Crosshair
SpriteSheet crosshair;

// UI Menus
SpriteSheet_Old ssmenu_play, ssmenu_options, ssmenu_exit, ssmenu_1, ssmenu_2, ssmenu_3, ssmenu_4, ssmenu_screen, ssmenu_bar1, ssmenu_bar2;
UIScreen uiscreen;

// Loading Image
AEGfxTexture* loadingImage;

// BGM and SFX
AEAudio levelBGM, levelSFX, lightProj, parryFX, heavyProj;
AEAudioGroup gameBGM, gameSFX;
float current_bgm_vol, prev_bgm_vol, max_bgm_vol;
float current_sfx_vol, prev_sfx_vol, max_sfx_vol;

// For leaderboard Input
bool isInputDisabled; // Flag to disable player input
std::string playerName;  // Store player's typed name
bool isLoading; //Loading Indicator
float rotationAngle = 0.f; // Angle for rotation of the loading image
float rotationSpeed = 90.f; // Rotation speed (degrees per second)
Color isInputSubmitted;
bool hasSubmitted;

// Initialize the various screen UI elements
void Init_UI()
{
	// Colors
	UI_red = SetColor(0xFFFF0000);
	UI_green = SetColor(0xFF24FF8B);
	UI_blue = SetColor(0xFF00008B);
	UI_black = SetColor(0xFF000000);
	UI_white = SetColor(0xFFFFFFFF);
	UI_hover_red = SetColor(0xFFFF6961);
	UI_purple = SetColor(0xFFFF00FF);
	UI_gold = SetColor(0xFFFFD218);

	// Font
	UI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
	pixel_font = AEGfxCreateFont("Assets/buggy-font.ttf", 72);

	loadingImage = AEGfxTextureLoad("Assets/PlanetTexture.png");

	// Level state UI
	play_bg = SetColor(0xFF666666);
	pause_bg = SetColor(0x4D000000);
	pause_screen = SetColor(0xFFFFFFFF);

	// Boss UI
	boss_hp_green = SetColor(0xFF00FF00);
	boss_hp_yellow = SetColor(0xFFFFEF00);
	boss_hp_red = SetColor(0xFFFF0000);
	boss_hp_damage = SetColor(0xFF800000);

	// Starting level state
	levelState = PLAYING;

	// Level score
	currentScore = 0;
	newScore = 0;
	pointsAccumulated = 0;
	displayPointsAccumulated = false;
	displayPointsTimer = Create_Timer(1.5f);
	currentCombo = 0;
	highestCombo = 0;
	score_color = UI_green;
	isPerfect = false;

	// Menu Buttons
	ssmenu_play = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_options = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_exit = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_1 = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_2 = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_3 = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_4 = Init_SpriteSheet("Assets/UI/MenuButton.png", 8, 8, 1.f);
	ssmenu_screen = Init_SpriteSheet("Assets/UI/MenuScreen.png", 5, 1, 1.f);
	ssmenu_bar1 = Init_SpriteSheet("Assets/UI/MenuBar.png", 1, 1, 1.f);
	ssmenu_bar2 = Init_SpriteSheet("Assets/UI/MenuBar.png", 1, 1, 1.f);

	// UI Screen Spritesheets
	uiscreen.spritesheet_playbutton = &ssmenu_play;
	uiscreen.spritesheet_optionsbutton = &ssmenu_options;
	uiscreen.spritesheet_exitbutton = &ssmenu_exit;
	uiscreen.spritesheet_button1 = &ssmenu_1;
	uiscreen.spritesheet_button2 = &ssmenu_2;
	uiscreen.spritesheet_button3 = &ssmenu_3;
	uiscreen.spritesheet_button4 = &ssmenu_4;
	uiscreen.spritesheet_screen = &ssmenu_screen;
	uiscreen.spritesheet_bar1 = &ssmenu_bar1;
	uiscreen.spritesheet_bar2 = &ssmenu_bar2;
	uiscreen.animationtimer = Create_Timer(0.5f);

	// Checks for button clicks in UI screens
	uiscreen.isPlayClicked = uiscreen.isOptionsClicked = uiscreen.isExitClicked = 0;
	uiscreen.isButton1Clicked = uiscreen.isButton2Clicked = uiscreen.isButton3Clicked = uiscreen.isButton4Clicked = 0;

	// BGM and SFX
	levelBGM = AEAudioLoadMusic("Assets/boss1Theme.mp3");
	levelSFX = AEAudioLoadSound("Assets/ore.mp3");
	lightProj = AEAudioLoadSound("Assets/lightProj.mp3");
	heavyProj = AEAudioLoadSound("Assets/heavyProj.mp3");
	parryFX = AEAudioLoadSound("Assets/sword-parry.mp3");

	// BGM and SFX (Audio Groups)
	gameBGM = AEAudioCreateGroup();
	gameSFX = AEAudioCreateGroup();

	// BGM and SFX (Settings)
	max_bgm_vol = max_sfx_vol = 100.f;

	// Initialize volume settings when entering level
	AEAudioPlay(levelBGM, gameBGM, 1.f, 1.f, -1);
	AEAudioSetGroupVolume(gameBGM, current_bgm_vol / max_bgm_vol);
	AEAudioSetGroupVolume(gameSFX, current_sfx_vol / max_sfx_vol);

	// For leaderboard input
	isInputDisabled = false;
	playerName = "(Type Name Here)";
	isLoading = false;
	hasSubmitted = false;
	isInputSubmitted = UI_white;
}

// Initialize the player UI elements
void Init_Player_UI(int ammo, int maxammo, float hp, float maxhp)
{
	// Player UI
	UI_playerhp = hp;
	UI_playermaxhp = maxhp;
	UI_ammo = ammo;
	UI_maxammo = maxammo;
	empty_ammo = SetColor(0x70B0FFD5);
	full_ammo = SetColor(0xFF24FF8B);
	health = SetColor(0xFFFF6961);
	parry_charging = SetColor(0xFF00255F);
	parry_ready = SetColor(0xFF004EFF);

	// Crosshair
	crosshair = SpriteSheet("Assets/UI/Crosshair.png", 4, 1);
	crosshair.Set_Row(0);
}

// Update the values for player UI
void Update_Player_UI(int ammo, float hp, float parry_timer)
{
	// Update player hp/ammo
	UI_playerhp = hp;
	UI_ammo = ammo;
	UI_parryCD = parry_timer;
}

// Draws the player UI elements
void Draw_Player_UI()
{
	Color parry = (UI_parryCD == 1.f) ? parry_ready : parry_charging;

	DrawHPBar(-700.f, -300.f, -500.f, -290.f, UI_parryCD, 1.f, parry);


	DrawHPBar(-700.f, -350.f, -300.f, -335.f, UI_playerhp, UI_playermaxhp, health);

	for (int ammo_index = 0; ammo_index < UI_maxammo; ammo_index++)
	{
		float posx = -675.f + ammo_index * 50.f;
		float posy = -400.f;
		if (ammo_index < UI_ammo)
		{
			DrawRect(posx, posy, 15.f, 50.f, full_ammo);
		}
		DrawRect(posx, posy, 25.f, 60.f, empty_ammo);
	}

	crosshair.Set_Col(UI_ammo);
	crosshair.Draw(Mouse.x, Mouse.y, 80.f, 80.f, false);

}

// Initialize the boss UI elements
void Init_Boss_UI(float hp)
{
	UI_bosshp = UI_bossmaxhp = hp;
}

// Update the boss UI elements
void Update_Boss_UI(float hp)
{
	UI_bosshp = hp;
}

// Draw Boss HP bar
void Draw_Boss_HPbar()
{
	DrawRect(0.f, 400.f, 1200.f, 50.f, boss_hp_damage);
	Color hpcolor = boss_hp_green;
	if (UI_bosshp < 0.6f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_yellow;
	}
	if (UI_bosshp < 0.3f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_red;
	}
	DrawHPBar(-600.f, 425.f, 600.f, 375.f, UI_bosshp, UI_bossmaxhp, hpcolor);
}

// Draw the various UI elements
void Draw_UI(int DrawBossHP )
{
	Draw_Player_UI();
	if (DrawBossHP) { Draw_Boss_HPbar(); }
}

// Open the pause menu when player pauses the game
void Draw_PauseMenu() 
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);


	// Pause menu screen
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 600.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 0, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isOptionsClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// play button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -350.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(450.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isPlayClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_play, 0, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
		
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			levelState = PLAYING;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 0, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// options button (click to go to options)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_optionsbutton, 0.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(800.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isOptionsClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_options, 3, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			// Initialize volumes
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			current_bgm_vol = prev_bgm_vol;
			current_sfx_vol = prev_sfx_vol;

			levelState = OPTIONS;
		}
	}
	else
	{
		uiscreen.isOptionsClicked = 0;
		Set_SpriteSheet_current(&ssmenu_options, 3, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// exit button (click to go to quit state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 350.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(1150.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			levelState = INQUIT;
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
}

// bool isInputDisabled = false;  // Flag to disable player input

void HandleTextInput()
{
	// Check if the backspace key is pressed and the player name is not empty
	if (AEInputCheckTriggered(AEVK_BACK) && playerName.length() > 0)
	{
		// Remove the last character (backspace)
		playerName.pop_back();
	}

	// Loop through the possible printable characters (A-Z)
	for (int i = AEVK_A; i <= AEVK_Z; ++i) {
		if (AEInputCheckTriggered(i)) {
			// Append the character to the player's name
			playerName.push_back(static_cast<char>('A' + (i - AEVK_A)));
		}
	}

	// Allow digits (0-9)
	for (int i = AEVK_0; i <= AEVK_9; ++i) {
		if (AEInputCheckTriggered(i)) {
			// Append the number to the player's name
			playerName.push_back(static_cast<char>('0' + (i - AEVK_0)));
		}
	}

	// Allow the space bar to insert spaces
	if (AEInputCheckTriggered(AEVK_SPACE)) {
		playerName.push_back(' '); // Append a space to the player's name
	}
}

// Function to draw the loading image
void DrawLoadingImage()
{
	if (loadingImage)
	{
		// Draw the image with a rotation angle
		DrawImage(0.f, 0.f, 800.f, 800.f, rotationAngle, loadingImage);  // Full screen translucent background

		// Update the rotation angle (for continuous rotation)
		rotationAngle += 0.05f; // Adjust this value to control the speed of rotation (2.f = fast, 0.5f = slower)
		if (rotationAngle >= 360.f)
		{
			rotationAngle = 0.f; // Reset the angle after one full rotation
		}
	}
}

void Draw_ClearMenu()
{
	isInputDisabled = true;
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	// Draw translucent background
	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);  // Change pause_bg to be a translucent color
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 800.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 2, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0 && uiscreen.isButton1Clicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// If loading, display the rotating loading image
	if (isLoading)
	{
		DrawLoadingImage();
		return;
	}

	// submit button (click to submit score to leaderboard)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, 400.f, -125.f, 125.f, 75.f, 0.f);

	// Prevent submission if already submitted
	if (!hasSubmitted && IsAreaClicked(1200.f, 575.f, 125.f, 75.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton1Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_1, 7, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);

			isLoading = true; // Activate loading state
			hasSubmitted = true; // Set the flag to true to prevent further submissions

			// Start the async thread for leaderboard submission
			std::thread submitThread([&]()
				{
					addLeaderboardEntry(playerName.c_str(), newScore, level_timer); // Submit to leaderboard
					isLoading = false; // Disable loading state after submission
					isInputSubmitted = UI_green;
				});
			submitThread.detach(); // Detach to allow independent execution
		}
	}
	else if (hasSubmitted)
	{
		uiscreen.isButton1Clicked = 0;
		isInputDisabled = false;
		Set_SpriteSheet_current(&ssmenu_1, 7, 8, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
	else
	{
		uiscreen.isButton1Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_1, 7, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Replay button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -300.f, -300.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(500.f, 750.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isPlayClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_play, 1, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			nextState = GS_RESTART;
			isInputDisabled = false;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 1, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Display score above player input
	DrawTextBox(-0.4f, 0.3f, 300.f, 50.f, 0.6f, pixel_font, UI_green, level_score);

	DrawTextBox(-0.45f, 0.1f, 300.f, 50.f, 0.6f, pixel_font, UI_white, level_timer);

	snprintf(combo, sizeof(combo), "BEST COMBO: %d", highestCombo);

	std::cout << highestCombo << '\n';

	DrawTextBox(-0.45f, -0.1f, 300.f, 50.f, 0.6f, pixel_font, UI_white, combo);

	// Test with a position close to the center of the screen
	DrawTextBox(-0.4f, -0.3f, 300.f, 50.f, 0.5f, pixel_font, isInputSubmitted, playerName.c_str());

	// Handle text input
	HandleTextInput();

	// Exit button (click to go back to hubworld state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 300.f, -300.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(1100.f, 750.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;
		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			nextState = GS_HUBWORLD;
			isInputDisabled = false;
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
}



// Open the options menu when player accesses options from the pause menu
void Draw_OptionsMenu()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 600.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 3, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isButton1Clicked == 0 && uiscreen.isButton2Clicked == 0 && uiscreen.isButton3Clicked == 0 &&
		uiscreen.isButton4Clicked == 0 && uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// BGM bar and buttons
	
	DrawTextBox(-0.5f, 0.15f, 100.f, 100.f, 1.f, UI_font, UI_black, "BGM");

	DrawHPBar(-50.f, 75.f, 350.f, 125.f, AEClamp(current_bgm_vol, 0.f, max_bgm_vol), max_bgm_vol, UI_hover_red);
	Draw_SpriteSheet_test(*uiscreen.spritesheet_bar1, 150.f, 100.f, 425.f, 50.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_bar1, 1, 1, 1, Create_Timer(1.f), 1.f);

	std::cout << current_bgm_vol << '\n';

	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, -150.f, 100.f, 70.f, 70.f, 0.f);

	if (IsAreaClicked(650.f, 350.f, 100.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton1Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_1, 6, 5, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			// lower bgm
			current_bgm_vol -= 1.f;
			if (current_bgm_vol <= 0.f)
			{
				current_bgm_vol = 0.f;
			}
			AEAudioSetGroupVolume(gameBGM, current_bgm_vol / max_bgm_vol);
		}
	}
	else
	{
		uiscreen.isButton1Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_1, 6, 5, 5, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	Draw_SpriteSheet_test(*uiscreen.spritesheet_button2, 450.f, 100.f, 70.f, 70.f, 0.f);

	if (IsAreaClicked(1250.f, 350.f, 100.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton2Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_2, 6, 1, 4, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			// raise bgm
			current_bgm_vol += 1.f;
			if (current_bgm_vol > max_bgm_vol)
			{
				current_bgm_vol = max_bgm_vol;
			}
			AEAudioSetGroupVolume(gameBGM, current_bgm_vol / max_bgm_vol);
		}
	}
	else
	{
		uiscreen.isButton2Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_2, 6, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// SFX bar and buttons

	DrawTextBox(-0.5f, -0.15f, 100.f, 100.f, 1.f, UI_font, UI_black, "SFX");

	DrawHPBar(-50.f, -75.f, 350.f, -25.f, AEClamp(current_sfx_vol, 0.f, max_sfx_vol), max_sfx_vol, UI_hover_red);
	Draw_SpriteSheet_test(*uiscreen.spritesheet_bar2, 150.f, -50.f, 425.f, 50.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_bar2, 1, 1, 1, Create_Timer(1.f), 1.f);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_button3, -150.f, -50.f, 70.f, 70.f, 0.f);

	if (IsAreaClicked(650.f, 500.f, 100.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton3Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_3, 6, 5, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			// lower sfx
			current_sfx_vol -= 1.f;
			if (current_sfx_vol < 0.f)
			{
				current_sfx_vol = 0.f;
			}
			AEAudioSetGroupVolume(gameSFX, current_sfx_vol / max_sfx_vol);
		}
	}
	else
	{
		uiscreen.isButton3Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_3, 6, 5, 5, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	Draw_SpriteSheet_test(*uiscreen.spritesheet_button4, 450.f, -50.f, 70.f, 70.f, 0.f);

	if (IsAreaClicked(1250.f, 500.f, 100.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton4Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_4, 6, 1, 4, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			// raise sfx
			current_sfx_vol += 1.f;
			if (current_sfx_vol > max_sfx_vol)
			{
				current_sfx_vol = max_sfx_vol;
			}
			AEAudioSetGroupVolume(gameSFX, current_sfx_vol / max_sfx_vol);
		}
	}
	else
	{
		uiscreen.isButton4Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_4, 6, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// save button (click to save changes)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -350.f, -200.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(450.f, 650.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isPlayClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_play, 4, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			prev_bgm_vol = current_bgm_vol;
			prev_sfx_vol = current_sfx_vol;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 4, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// exit button (click to return to pause menu)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 350.f, -200.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(1150.f, 650.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 5, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			levelState = PAUSED;
			AEAudioSetGroupVolume(gameBGM, prev_bgm_vol / max_bgm_vol);
			AEAudioSetGroupVolume(gameSFX, prev_sfx_vol / max_sfx_vol);
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 5, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
}

// Open the quit menu screen to allow player to think twice about their life choices
void Draw_QuitMenu()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 600.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 4, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// display message
	
	if (currentState == GS_HUBWORLD)
	{
		DrawTextAdvanced(40.f, 250.f, pixel_font, UI_white, "Are you sure you want to quit?", false, 0.5f);
	}
	else
	{
		DrawTextAdvanced(40.f, 400.f, pixel_font, UI_white, "Are you sure you want to quit?", false, 0.5f);
		DrawTextAdvanced(40.f, 250.f, pixel_font, UI_white, "Your current progress will be", false, 0.5f);
		DrawTextAdvanced(40.f, 100.f, pixel_font, UI_red, "LOST!!!", false, 0.8f);
	}

	// back button (click to go back to level state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -350.f, -50.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(450.f, 500.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isPlayClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_play, 5, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			levelState = PLAYING;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 5, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// exit button (click to go to quit state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 350.f, -50.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(1150.f, 500.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			if (currentState == GS_HUBWORLD)
			{
				nextState = GS_MAIN_MENU;
			}
			else
			{
				nextState = GS_HUBWORLD;
			}
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

}

// Open the game over screen when player dies
void Draw_GameOverMenu()
{
	isInputDisabled = true;

	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 800.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 1, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0 && uiscreen.isButton1Clicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// If loading, display the rotating loading image
	if (isLoading)
	{
		DrawLoadingImage();
		return;
	}

	// submit button (click to submit score to leaderboard)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, 400.f, -75.f, 125.f, 75.f, 0.f);

	// Prevent submission if already submitted
	if (!hasSubmitted && IsAreaClicked(1200.f, 525.f, 125.f, 75.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isButton1Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_1, 7, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			isLoading = true; // Activate loading state
			hasSubmitted = true; // Set the flag to true to prevent further submissions

			// Start the async thread for leaderboard submission
			std::thread submitThread([&]()
				{
					addLeaderboardEntry(playerName.c_str(), newScore, level_timer); // Submit to leaderboard
					isLoading = false; // Disable loading state after submission
					isInputSubmitted = UI_green;
				});
			submitThread.detach(); // Detach to allow independent execution
		}
	}
	else if (hasSubmitted)
	{
		uiscreen.isButton1Clicked = 0;
		isInputDisabled = false;
		Set_SpriteSheet_current(&ssmenu_1, 7, 8, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
	else
	{
		uiscreen.isButton1Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_1, 7, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// play button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -300.f, -250.f, 200.f, 100.f, 0.f);

	if (IsAreaClicked(500.f, 700.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isPlayClicked = 1;
		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_play, 1, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			nextState = GS_RESTART;
			isInputDisabled = false;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 1, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Display score above player input
	DrawTextBox(-0.4f, 0.2f, 300.f, 50.f, 0.7f, pixel_font, UI_red, level_score);

	DrawTextBox(-0.45f, 0.f, 300.f, 50.f, 0.7f, pixel_font, UI_white, level_timer);

	// exit button (click to go to quit state)
	// Inside your menu or game loop
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 300.f, -250.f, 200.f, 100.f, 0.f);
	// Test with a position close to the center of the screen
	DrawTextBox(-0.4f, -0.2f, 300.f, 50.f, 0.5f, pixel_font, isInputSubmitted, playerName.c_str());

	// Handle text input
	HandleTextInput();

	if (IsAreaClicked(1100.f, 700.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;
		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			nextState = GS_HUBWORLD;
			isInputDisabled = false;
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
}

// Update the level timer
void Update_LevelTimer(float *time_elapsed)
{
	*time_elapsed += dt;
}

// Draw the level timer
void Draw_LevelTimer(float time_elapsed)
{
	int minutes = static_cast<int>(time_elapsed) / 60;
	int seconds = static_cast<int>(time_elapsed) % 60;
	int milliseconds = static_cast<int>((time_elapsed - static_cast<int>(time_elapsed)) * 100);

	snprintf(level_timer, sizeof(level_timer), "Time: %02d:%02d:%02d", minutes, seconds, milliseconds);

	DrawTextBox(0.3f, -0.9f, 200.f, 100.f, 0.5f, pixel_font, UI_white, level_timer);
}


// Draw the level score
void Draw_LevelScore()
{
	snprintf(level_score, sizeof(level_score), "Score: %d", currentScore);

	DrawTextBox(0.3f, -0.7f, 200.f, 100.f, 0.5f, pixel_font, UI_white, level_score);
}

// Accumulate points
void Collect_Points(int value, int score_index)
{
	currentScore = newScore;
	if (score_index == PERFECT_SCORE)
	{
		isPerfect = true;
	}
	else
	{
		snprintf(score_type, sizeof(score_type), "%s", SCORE_TYPE_DISPLAY[score_index]);
		if (score_index == OUCH_SCORE || score_index == RIP_SCORE)
		{
			score_color = UI_red;
			newScore = currentScore + pointsAccumulated;
			currentScore = newScore;
			pointsAccumulated = 0;
		}
		else
		{
			if (score_index == PARRY_SCORE)
			{
				score_color = UI_purple;
			}
			else
			{
				score_color = UI_green;
			}
			if (pointsAccumulated < 0)
			{
				currentScore = currentScore + pointsAccumulated;
				pointsAccumulated = 0;
			}
		}
	}
	pointsAccumulated += value;
	displayPointsAccumulated = true;
	displayPointsTimer.current = 0.f;
}

// Draw accumulated points
void Draw_Points()
{
	std::cout << pointsAccumulated << '\n';
	if (Update_Timer(&displayPointsTimer) && pointsAccumulated != 0)
	{
		if (isPerfect)
		{
			DrawTextAdvanced(player.pos.x, player.pos.y + 280.f, pixel_font, UI_gold, "PERFECT!!!", true, 1.f);
		}
		if (currentCombo > 1)
		{
			snprintf(combo, sizeof(combo), "COMBO %d", currentCombo);
			DrawTextAdvanced(player.pos.x, player.pos.y + 140.f, pixel_font, UI_green, combo, true, 0.5f);
			DrawTextAdvanced(player.pos.x, player.pos.y + 210.f, pixel_font, score_color, score_type, true, 0.5f);
		}
		else
		{
			DrawTextAdvanced(player.pos.x, player.pos.y + 140.f, pixel_font, score_color, score_type, true, 0.5f);
		}

		// POINTS
		if (pointsAccumulated > 0)
		{
			snprintf(points, sizeof(points), "+%d", pointsAccumulated);
			DrawTextAdvanced(player.pos.x, player.pos.y + 70.f, pixel_font, UI_green, points, true, 0.5f);
		}
		else if (pointsAccumulated < 0)
		{
			snprintf(points, sizeof(points), "%d", pointsAccumulated);
			DrawTextAdvanced(player.pos.x, player.pos.y + 70.f, pixel_font, UI_red, points, true, 0.5f);
		}
	}
	else
	{
		displayPointsAccumulated = false;
		newScore = currentScore + pointsAccumulated;
		pointsAccumulated = 0;
		if (currentCombo > highestCombo)
		{
			highestCombo = currentCombo;
		}
		currentCombo = 0;
	}
}

void Update_LevelScore()
{
	if (currentScore + 10000 < newScore)
	{
		currentScore += 1000.f;
	}
	else if (currentScore < newScore)
	{
		currentScore += 200.f;
	}
	else
	{
		currentScore = newScore;
	}
}


//void fetchLeaderboard() {
//	// Call the Python script to fetch the leaderboard
//	std::cout << "Fetching leaderboard..." << std::endl;
//	int result = system("\\Assets\\dist\\leaderboard_helper.exe");
//
//	if (result != 0) {
//		std::cout << "Error fetching leaderboard!" << std::endl;
//	}
//	else {
//		std::cout << "Leaderboard fetched successfully!" << std::endl;
//	}
//}

void addLeaderboardEntry(const std::string& name, int score, const std::string& timing) {
	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);
	std::string currentDir(buffer);

	// Construct command with levelTimer
	std::string command = "\"\"" + currentDir + "\\Assets\\dist\\leaderboard_helper.exe\" \""
		+ name + "\" \""
		+ std::to_string(score) + "\" \""
		+ timing + "\"\"";

	std::cout << "Adding leaderboard entry..." << std::endl;
	int result = system(command.c_str());

	if (result != 0) {
		std::cout << "Error adding leaderboard entry!" << std::endl;
	}
	else {
		std::cout << "Leaderboard entry added successfully!" << std::endl;
	}
}


// Draw full hp bar
// pos_x1, pos_y1 - position of top left corner
// pos_x2, pos_y2 - position of bottom right corner
// hp - current health
// maxhp - max health
void DrawHPBar(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color)
{
	// assign total width, height, and width of health bar
	float width, height, hpwidth;
	//width and height of total bar
	width = pos_x2 - pos_x1;
	height = pos_y2 - pos_y1;
	//width of current hp
	hpwidth = width * hp / maxhp;
	// draw the rectangle
	DrawRect(pos_x1 + (hpwidth / 2.f), pos_y1 + height / 2.f, hpwidth, height, color);
}

// Draw segmented hp bar
// pos_x1, pos_y1 - position of top left corner
// pos_x2, pos_y2 - position of bottom right corner
// hp - current health
// maxhp - max health
void DrawHPBar2(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color)
{
	//array of hp in 10% chunks
	float hpbar[10] = { 0.f, maxhp / 10.f, 2.f * maxhp / 10.f, 3.f * maxhp / 10.f, 4.f * maxhp / 10.f, 5.f * maxhp / 10.f, 6.f * maxhp / 10.f, 7.f * maxhp / 10.f, 8.f * maxhp / 10.f , 9.f * maxhp / 10.f }; //array 10% blocks
	//size of full bar
	float width = pos_x2 - pos_x1;
	float height = pos_y2 - pos_y1;
	//segment width and interval
	float segmentwidth = width / 10.f;
	float segmentinterval = 15.f;
	//for loop to check hp 10% chunks
	for (int i = 0; i < 10; ++i)
	{
		if (hp > (hpbar[i]) + maxhp / 20.f) //if hp>10% interval, draw 10% chunk
		{
			//DrawRect(Mesh, (pos_x1 + (segmentwidth / 2.f)) + (i * (segmentwidth)), pos_y1 + height / 2.f, segmentwidth - segmentinterval, height, color);

			DrawRect((pos_x1 + ((segmentwidth - segmentinterval) / 2.f)) + (i * (segmentwidth + segmentinterval / 10.f)), pos_y1 + height / 2.f, segmentwidth - segmentinterval, height, color);
		}
	}
}

// Free the UI elements
void Free_UI()
{
	AEAudioStopGroup(gameBGM);
	AEAudioStopGroup(gameSFX);
}

// Unload the UI elements
void Unload_UI()
{
	// Spritesheets
	Unload_SpriteSheet(&ssmenu_play);
	Unload_SpriteSheet(&ssmenu_options);
	Unload_SpriteSheet(&ssmenu_exit);
	Unload_SpriteSheet(&ssmenu_1);
	Unload_SpriteSheet(&ssmenu_2);
	Unload_SpriteSheet(&ssmenu_3);
	Unload_SpriteSheet(&ssmenu_4);
	Unload_SpriteSheet(&ssmenu_screen);
	Unload_SpriteSheet(&ssmenu_bar1);
	Unload_SpriteSheet(&ssmenu_bar2);

	// Crosshair
	crosshair.Unload();

	// Game audio
	AEAudioUnloadAudio(levelBGM);
	AEAudioUnloadAudioGroup(gameBGM);
	AEAudioUnloadAudioGroup(gameSFX);
}