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
#include "Boss1.h"
#include "GameStateManager.h"
#include "SpriteSheet.h"
#include "player.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <direct.h>

// Level number
int levelState = 0;
float levelTimer = 0.f;

// Level soore
int currentScore = 0;
int newScore = 0;
int pointsAccumulated = 0;
bool displayPointsAccumulated = false;
Timer displayPointsTimer;

// Font, colors, mouse coordinates
s8 UI_font;
s8 pixel_font;
Color UI_red, UI_green, UI_blue, UI_black, UI_white, UI_hover_red;
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

// Level Score
char points[16];
char level_score[16];

// Crosshair
SpriteSheet crosshair;

// UI Menus
SpriteSheet_Old ssmenu_play, ssmenu_options, ssmenu_exit, ssmenu_1, ssmenu_2, ssmenu_3, ssmenu_4, ssmenu_screen, ssmenu_bar1, ssmenu_bar2;
UIScreen uiscreen;

// BGM and SFX
AEAudio levelBGM, levelSFX, lightProj, parryFX, heavyProj;
AEAudioGroup gameBGM, gameSFX;
float current_bgm_vol, prev_bgm_vol, max_bgm_vol;
float current_sfx_vol, prev_sfx_vol, max_sfx_vol;

// Initialize the various screen UI elements
void Init_UI()
{
	// Colors
	UI_red = SetColor(0xFFFF6961);
	UI_green = SetColor(0xFF88B984);
	UI_blue = SetColor(0xFF00008B);
	UI_black = SetColor(0xFF000000);
	UI_white = SetColor(0xFFFFFFFF);
	UI_hover_red = SetColor(0xFFFF6961);

	// Font
	UI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
	pixel_font = AEGfxCreateFont("Assets/buggy-font.ttf", 72);

	// Level state UI
	play_bg = SetColor(0xFF666666);
	pause_bg = SetColor(0x4D000000);
	pause_screen = SetColor(0xFFFFFFFF);

	// Boss UI
	boss_hp_green = SetColor(0xFF00FF00);
	boss_hp_yellow = SetColor(0xFFFFEF00);
	boss_hp_red = SetColor(0xFFFF0000);
	boss_hp_damage = SetColor(0xFF800000);
	boss1.hp_color = boss_hp_green;

	// Starting level state
	levelState = PLAYING;

	// Level score
	currentScore = 0;
	newScore = 0;
	pointsAccumulated = 0;
	displayPointsAccumulated = false;
	displayPointsTimer = Create_Timer(1.f);

	// Menu Buttons
	ssmenu_play = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_options = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_exit = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_1 = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_2 = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_3 = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	ssmenu_4 = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
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
	current_bgm_vol = prev_bgm_vol = current_sfx_vol = prev_sfx_vol = 2.f;			//sorry fauz this shi driving me crazy HAHAHHAHAHA
	max_bgm_vol = max_sfx_vol = 100.f;

	// Initialize volume settings when entering level
	AEAudioPlay(levelBGM, gameBGM, 1.f, 1.f, -1);
	AEAudioSetGroupVolume(gameBGM, current_bgm_vol / max_bgm_vol);
	AEAudioSetGroupVolume(gameSFX, current_sfx_vol / max_sfx_vol);
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

//void Draw_Boss1HP()
//{
//	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
//	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
//
//	// boss health logic
//	if (boss1.current_hp >= 30.f && boss1.current_hp < 60.f)
//	{
//		boss1.phase = PHASE_2;
//		boss1.hp_color = boss_hp_yellow;
//	}
//	if (boss1.current_hp >= 0.f && boss1.current_hp < 30.f)
//	{
//		boss1.phase = PHASE_3;
//		boss1.hp_color = boss_hp_red;
//	}
//	if (boss1.current_hp <= 0.f)
//	{
//		boss1.phase = DEFEAT;
//		boss1.current_hp = 0.f;
//	}
//
//	// Draw boss health bar
//	DrawRect(0.f, 400.f, 1200.f, 50.f, boss1.hp_color);
//	DrawRect((boss1.current_hp / boss1.max_hp) * 1200.f, 400.f, 1200.f, 50.f, boss_hp_damage);
//	DrawRect(800.f, 400.f, 400.f, 50.f, play_bg);
//}

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

bool isInputDisabled = false;  // Flag to disable player input

std::string playerName = "(Type Name Here)";  // Store player's typed name

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


// Open the level complete screen when player beats the boss
void Draw_ClearMenu()
{
	isInputDisabled = true;
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 600.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 2, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}


	// play button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -300.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(500.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
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

	// exit button (click to go to quit state)
	// Inside your menu or game loop
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 300.f, -50.f, 300.f, 200.f, 0.f);
	// Test with a position close to the center of the screen
	DrawTextBox(0.f, 0.f, 300.f, 50.f, 0.5f, pixel_font, { 1.f, 1.f, 1.f, 1.f }, playerName.c_str());


	// Handle text input
	HandleTextInput();


	if (IsAreaClicked(1100.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
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
			addLeaderboardEntry(playerName.c_str(), newScore);
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

// Open the game over screen when player dies
void Draw_GameOverMenu()
{
	isInputDisabled = true;

	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);

	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, 0.f, 0.f, 1200.f, 600.f, 0.f);
	Set_SpriteSheet_current(&ssmenu_screen, 1, 1, 1, Create_Timer(1.f), 1.f);

	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// play button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, -300.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(500.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
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

	// exit button (click to go to quit state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, 300.f, -50.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(1100.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
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

	DrawTextBox(0.3f, -0.9f, 200.f, 100.f, 0.8f, UI_font, UI_white, level_timer);
}

// Draw the level score
void Draw_LevelScore()
{
	snprintf(level_score, sizeof(level_score), "Score: %d", currentScore);

	DrawTextBox(0.3f, -0.7f, 200.f, 100.f, 0.8f, UI_font, UI_white, level_score);
}

// Accumulate points
void Collect_Points(int value)
{
	currentScore = newScore;
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
		if (pointsAccumulated > 0)
		{
			snprintf(points, sizeof(points), "+%d", pointsAccumulated);
			DrawTextAdvanced(player.pos.x, player.pos.y + 70.f, UI_font, UI_green, points, true, 0.7f);
		}
		else if (pointsAccumulated < 0)
		{
			snprintf(points, sizeof(points), "%d", pointsAccumulated);
			DrawTextAdvanced(player.pos.x, player.pos.y + 70.f, UI_font, UI_red, points, true, 0.7f);
		}
	}
	else
	{
		displayPointsAccumulated = false;
		newScore = currentScore + pointsAccumulated;
		pointsAccumulated = 0;
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
		currentScore += 100.f;
	}
	else
	{
		currentScore = newScore;
	}
}


void fetchLeaderboard() {
	// Call the Python script to fetch the leaderboard
	std::cout << "Fetching leaderboard..." << std::endl;
	int result = system("\\Assets\\dist\\leaderboard_helper.exe");

	if (result != 0) {
		std::cout << "Error fetching leaderboard!" << std::endl;
	}
	else {
		std::cout << "Leaderboard fetched successfully!" << std::endl;
	}
}

void addLeaderboardEntry(const std::string& playerName, int score) {
	// Prepare the command to call the Python script with parameters
	//std::string command = "py \"C:\\Users\\fauza\\OneDrive\\Desktop\\CSD1451\\AvidParryEnjoyers\\csd1451s25_team_avidparryenjoyers\\PARRY PING PING\\Source\\leaderboard_helper.py\" " + playerName + " " + std::to_string(score);

		// Get the current working directory
	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);  // Get current directory
	std::string currentDir(buffer);

	// Now construct the command to call the Python script
	std::string command = "\"\"" + currentDir + "\\Assets\\dist\\leaderboard_helper.exe\" \"" + playerName + "\" \"" + std::to_string(score) + "\"\"";




	// Call the Python script to add a new leaderboard entry
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