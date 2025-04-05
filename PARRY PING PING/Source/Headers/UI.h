//---------------------------------------------------------
// file:	UI.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Declaration for all UI elements
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

extern bool isFirstInit;

// Extern font for other files
extern s8 pixel_font;

extern AEAudio levelBGM, levelSFX, lightProj, parryFX, heavyProj, sword1, sword2, sword3, dashAudio, ammoFullSFX, hubWorldMusic;
extern AEAudioGroup gameBGM, gameSFX;
extern float current_bgm_vol, prev_bgm_vol, max_bgm_vol;
extern float current_sfx_vol, prev_sfx_vol, max_sfx_vol;

// ----------------------------------------------------------------------------
// Enumeration for level states
// ----------------------------------------------------------------------------
enum LEVEL_STATE
{
	PLAYING = 0,
	PAUSED,
	GAMEOVER,
	OPTIONS,
	INQUIT,
	CLEARED
};

extern int levelState;
extern float levelTimer, levelTimer2;
extern int currentScore;
extern bool displayPointsAccumulated, isInputDisabled;
extern int pointsAccumulated;
extern Timer displayPointsTimer;
extern int currentCombo, highestCombo;

// ----------------------------------------------------------------------------
// Enumeration for score types
// ----------------------------------------------------------------------------
enum SCORE_TYPE
{
	HIT_SCORE = 0,
	BIG_HIT_SCORE,
	DODGE_SCORE,
	PARRY_SCORE,
	OUCH_SCORE,
	BLOCKED_SCORE,
	RIP_SCORE,
	TIME_BONUS_SCORE,
	PERFECT_SCORE
};

// ----------------------------------------------------------------------------
// Structure for UI screen sprites
// ----------------------------------------------------------------------------
typedef struct UIScreen
{
	SpriteSheet_Old* spritesheet_screen;
	SpriteSheet_Old* spritesheet_playbutton;
	SpriteSheet_Old* spritesheet_optionsbutton;
	SpriteSheet_Old* spritesheet_exitbutton;

	SpriteSheet_Old* spritesheet_button1;
	SpriteSheet_Old* spritesheet_button2;
	SpriteSheet_Old* spritesheet_button3;
	SpriteSheet_Old* spritesheet_button4;

	SpriteSheet_Old* spritesheet_bar1;
	SpriteSheet_Old* spritesheet_bar2;

	Timer animationtimer;
	int isPlayClicked, isSubmitClicked;
	int isOptionsClicked;
	int isExitClicked;
	int isButton1Clicked, isButton2Clicked, isButton3Clicked, isButton4Clicked;
} UIScreen;

// ----------------------------------------------------------------------------
// Draw the various UI elements
// ----------------------------------------------------------------------------
void Draw_UI(int DrawBossHP = 1);

// will neaten when more player parameters are finalized

// ----------------------------------------------------------------------------
// Initialize the various screen UI elements
// ----------------------------------------------------------------------------
void Init_UI();

// ----------------------------------------------------------------------------
// Load the various screen UI elements
// ----------------------------------------------------------------------------
void Load_UI();

// ----------------------------------------------------------------------------
// Initialize the player UI elements
// ----------------------------------------------------------------------------
void Init_Player_UI(int ammo, int maxammo, float hp, float maxhp);

// ----------------------------------------------------------------------------
// Load player UI elements
// ----------------------------------------------------------------------------
void Load_Player_UI();

// ----------------------------------------------------------------------------
// Unload player UI elements
// ----------------------------------------------------------------------------
void Unload_Player_UI();

// ----------------------------------------------------------------------------
// Update the values for player UI
// ----------------------------------------------------------------------------
void Update_Player_UI(int ammo, float hp, float parry_timer, int rage);

// ----------------------------------------------------------------------------
// Initialize the boss UI elements
// ----------------------------------------------------------------------------
void Init_Boss_UI(float hp);

// ----------------------------------------------------------------------------
// Update the boss UI elements
// ----------------------------------------------------------------------------
void Update_Boss_UI(float hp);

// ----------------------------------------------------------------------------
// Update the level timer
// ----------------------------------------------------------------------------
void Update_LevelTimer(float *time_elapsed);

// ----------------------------------------------------------------------------
// Free the UI elements
// ----------------------------------------------------------------------------
void Free_UI();

// ----------------------------------------------------------------------------
// Unload the UI elements
// ----------------------------------------------------------------------------
void Unload_UI();

// ----------------------------------------------------------------------------
// Open the pause menu when player pauses the game
// ----------------------------------------------------------------------------
void Draw_PauseMenu();

// ----------------------------------------------------------------------------
// Open the game over screen when player dies
// ----------------------------------------------------------------------------
void Draw_GameOverMenu();

// ----------------------------------------------------------------------------
// Open the options menu when player accesses options from the pause menu
// ----------------------------------------------------------------------------
void Draw_OptionsMenu();

// ----------------------------------------------------------------------------
// Open the quit menu to make player think twice about their life choices
// ----------------------------------------------------------------------------
void Draw_QuitMenu();

// ----------------------------------------------------------------------------
// Open the level complete screen when player beats the boss
// ----------------------------------------------------------------------------
void Draw_ClearMenu();

// ----------------------------------------------------------------------------
// Draw the level timer
// ----------------------------------------------------------------------------
void Draw_LevelTimer(float time_elapsed);

// ----------------------------------------------------------------------------
// Draw the level score
// ----------------------------------------------------------------------------
void Draw_LevelScore();

// ----------------------------------------------------------------------------
// Accumulate points
// ----------------------------------------------------------------------------
void Collect_Points(int value, int score_index);

// ----------------------------------------------------------------------------
// Draw points accumulated
// ----------------------------------------------------------------------------
void Draw_Points();

// ----------------------------------------------------------------------------
// Update the level score
// ----------------------------------------------------------------------------
void Update_LevelScore();

void addLeaderboardEntry(const std::string& playerName, int score, const std::string& levelTimer);

void DrawLoadingImage();