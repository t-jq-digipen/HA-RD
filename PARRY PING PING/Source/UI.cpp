//---------------------------------------------------------
// file:	UI.cpp
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	UI elements
//
// Copyright � 2020 DigiPen, All rights reserved.
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

// First Time Init State Flag
bool isFirstInit = false;

// Level Number
int levelState = 0;

// Level Score
int currentScore;
int newScore;

// Score Modifier
f32 Modifier_ScoreScale[4]{ 0.f, 0.5f, 1.f, 1.5f };

// Level Points Logic
int pointsAccumulated;
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
int 
UI_ammo, UI_maxammo,
UI_rage,
UI_parryCD,
UI_dodgeCD;
Color empty_ammo, full_ammo, health, parry_charging, parry_ready;

// Boss UI
float UI_bosshp, UI_bossmaxhp;

// UI Colors
Color play_bg, pause_bg, pause_screen;

// Boss HP Colors
Color boss_hp_red, boss_hp_yellow, boss_hp_green, boss_hp_damage;

// Level Timer
float levelTimer = 0.f;
float levelTimer2 = 0.f;
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
float crosshair_width, crosshair_height;

// Dash and block cooldown
SpriteSheet racecar;

// HP and ammo HUD
SpriteSheet playerHUD;

// UI Menus
SpriteSheet_Old ssmenu_play, ssmenu_options, ssmenu_exit, ssmenu_1, ssmenu_2, ssmenu_3, ssmenu_4, ssmenu_screen, ssmenu_bar1, ssmenu_bar2;
UIScreen uiscreen;
float menubg_x, menubg_y, menubg_width, menubg_height;
float menu_x, menu_y, menu_width, menu_height, endmenu_height, menu_rot;

// Loading Image
AEGfxTexture* loadingImage;

// BGM and SFX
AEAudio levelBGM, levelSFX, lightProj, parryFX, heavyProj, sword1, sword2, sword3, bossLevel2, dashAudio, ammoFullSFX, hubWorldMusic;
AEAudioGroup gameBGM, gameSFX;
float current_bgm_vol, prev_bgm_vol, max_bgm_vol;
float current_sfx_vol, prev_sfx_vol, max_sfx_vol;

// For leaderboard Input
bool isInputEnabled; // Flag to disable player input
std::string playerName;  // Store player's typed name
bool isLoading; //Loading Indicator
float rotationAngle = 0.f; // Angle for rotation of the loading image
float rotationSpeed = 90.f; // Rotation speed (degrees per second)
Color isInputSubmitted;
bool hasSubmitted;
bool isNameEntered;

// File IO
std::vector<float> UI_IO;
std::vector<float> UI_IO_settings;
std::vector<float> UI_IO_score;

// Font Size
int font_size;

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

	// Level timer
	levelTimer = 0;
	levelTimer2 = 0;

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

	// UI Menus
	menubg_x = UI_IO[23];
	menubg_y = UI_IO[24];
	menubg_width = UI_IO[25];
	menubg_height = UI_IO[26];

	menu_x = UI_IO[27];
	menu_y = UI_IO[28];
	menu_width = UI_IO[29];
	menu_height = UI_IO[30];
	endmenu_height = UI_IO[31];
	menu_rot = UI_IO[32];


	// Checks for button clicks in UI screens
	uiscreen.isPlayClicked = uiscreen.isOptionsClicked = uiscreen.isExitClicked = 0;
	uiscreen.isButton1Clicked = uiscreen.isButton2Clicked = uiscreen.isButton3Clicked = uiscreen.isButton4Clicked = 0;

	// BGM and SFX (Audio Groups)
	gameBGM = AEAudioCreateGroup();
	gameSFX = AEAudioCreateGroup();

	// Initialize volume settings when entering level
	if (!isFirstInit)
	{
		current_bgm_vol = UI_IO_settings[0];
		prev_bgm_vol = current_bgm_vol;
	}
	else
	{
		current_bgm_vol = prev_bgm_vol;
	}
	
	if (!isFirstInit)
	{
		current_sfx_vol = UI_IO_settings[1];
		prev_sfx_vol = current_sfx_vol;
	}
	else
	{
		current_sfx_vol = prev_sfx_vol;
	}
	
	max_bgm_vol = UI_IO_settings[2];
	max_sfx_vol = UI_IO_settings[3];

	// Determine which BGM to play based on the current level
	if (currentState == GS_HUBWORLD)
	{
		AEAudioPlay(hubWorldMusic, gameBGM, 1.f, 1.f, -1);
	}
	else if (currentState == GS_LEVEL1)
	{
		AEAudioPlay(levelBGM, gameBGM, 1.f, 1.f, -1); // Looping BGM
	}
	else if (currentState == GS_LEVEL2)
	{
		AEAudioPlay(bossLevel2, gameBGM, 1.f, 1.f, -1);
	}

	// Set audio volumes according to respective values
	AEAudioSetGroupVolume(gameBGM, current_bgm_vol / max_bgm_vol);
	AEAudioSetGroupVolume(gameSFX, current_sfx_vol / max_sfx_vol);

	// For leaderboard input
	isInputEnabled = false;
	playerName = "BACKSPACE to Type";
	isLoading = false;
	hasSubmitted = false;
	isInputSubmitted = UI_white;
	isNameEntered = false;

	// First Time Init State Flag
	isFirstInit = true;
}

void Load_UI()
{
	// Read File IO
	ReadFromFile("Assets/FileIO/UI.txt", UI_IO);
	ReadFromFile("Assets/FileIO/Settings.txt", UI_IO_settings);
	ReadFromFile("Assets/FileIO/PlayerBestScores.txt", UI_IO_score);

	// Menu Buttons
	ssmenu_play		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_options	= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_exit		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_1		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_2		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_3		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_4		= Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	ssmenu_screen	= Init_SpriteSheet("Assets/UI/MenuScreen.png", 5, 1, 1.f);
	ssmenu_bar1		= Init_SpriteSheet("Assets/UI/MenuBar.png", 1, 1, 1.f);
	ssmenu_bar2		= Init_SpriteSheet("Assets/UI/MenuBar.png", 1, 1, 1.f);

	// UI Screen Spritesheets
	uiscreen.spritesheet_playbutton		= &ssmenu_play;
	uiscreen.spritesheet_optionsbutton	= &ssmenu_options;
	uiscreen.spritesheet_exitbutton		= &ssmenu_exit;
	uiscreen.spritesheet_button1		= &ssmenu_1;
	uiscreen.spritesheet_button2		= &ssmenu_2;
	uiscreen.spritesheet_button3		= &ssmenu_3;
	uiscreen.spritesheet_button4		= &ssmenu_4;
	uiscreen.spritesheet_screen			= &ssmenu_screen;
	uiscreen.spritesheet_bar1			= &ssmenu_bar1;
	uiscreen.spritesheet_bar2			= &ssmenu_bar2;
	uiscreen.animationtimer = Create_Timer(0.5f);

	// Font
	font_size = static_cast<int>(UI_IO[0]);
	UI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", font_size);
	pixel_font = AEGfxCreateFont("Assets/buggy-font.ttf", font_size);

	// BGM and SFX

	levelBGM = AEAudioLoadMusic("Assets/boss1Theme.mp3");
	levelSFX = AEAudioLoadSound("Assets/menuClick.mp3");
	lightProj = AEAudioLoadSound("Assets/lightProj.mp3");
	heavyProj = AEAudioLoadSound("Assets/newCharged.mp3");
	parryFX = AEAudioLoadSound("Assets/sword-parry.mp3");
	sword1 = AEAudioLoadSound("Assets/first_slash.mp3");
	sword2 = AEAudioLoadSound("Assets/second_slash.mp3");
	sword3 = AEAudioLoadSound("Assets/third_slash.mp3");
	bossLevel2 = AEAudioLoadMusic("Assets/boss2Music.mp3");
	dashAudio = AEAudioLoadSound("Assets/dashSFX.mp3");
	ammoFullSFX = AEAudioLoadSound("Assets/chargedCan.mp3");
	hubWorldMusic = AEAudioLoadMusic("Assets/hubWorldSong.mp3");

	loadingImage = AEGfxTextureLoad("Assets/PlanetTexture.png");
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


	// Loading image
	AEGfxTextureUnload(loadingImage);

	// Game audio
	AEAudioUnloadAudio(levelBGM);
	AEAudioUnloadAudio(levelSFX);
	AEAudioUnloadAudio(lightProj);
	AEAudioUnloadAudio(parryFX);
	AEAudioUnloadAudio(heavyProj);
	AEAudioUnloadAudio(sword1);
	AEAudioUnloadAudio(sword2);
	AEAudioUnloadAudio(sword3);
	AEAudioUnloadAudio(bossLevel2);
	AEAudioUnloadAudio(dashAudio);
	AEAudioUnloadAudio(ammoFullSFX);
	AEAudioUnloadAudio(hubWorldMusic);
	AEAudioUnloadAudioGroup(gameBGM);
	AEAudioUnloadAudioGroup(gameSFX);
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

}

// Load player UI elements
void Load_Player_UI()
{
	// Crosshair
	crosshair = SpriteSheet("Assets/UI/Crosshair.png", 4, 1);
	crosshair.Set_Row(0);

	// Dash and block
	racecar = SpriteSheet("Assets/UI/Racecar.png", 2, 2);

	// HUD
	playerHUD = SpriteSheet("Assets/UI/SwordHUD.png", 41, 7);
	playerHUD.Set_Row(0);
}

// Unload player UI elements
void Unload_Player_UI()
{
	// Crosshair
	crosshair.Unload();

	// Dash and block
	racecar.Unload();

	// Player HUD
	playerHUD.Unload();
}

// Update the values for player UI
void Update_Player_UI(int ammo, float hp, float parry_cd, int rage, float dodge_cd)
{
	// Update player hp/ammo
	UI_playerhp = hp;
	UI_ammo = ammo;
	UI_parryCD = (parry_cd != 1.f) ? 1 : 0;
	UI_dodgeCD = (dodge_cd != 1.f) ? 1 : 0;
	UI_rage = rage;
}

// Draws the player UI elements
void Draw_Player_UI()
{
	if (!player.canAttack) return;

	// HEALTH
    playerHUD.Set_Row(0);
    playerHUD.Set_Col(0, 40, (UI_playermaxhp - UI_playerhp)/UI_playermaxhp);
    playerHUD.Draw(0.f, 0.f, 1600.f, 900.f);

    // AMMO
    playerHUD.Set_Row(1);
    playerHUD.Set_Col(0, 3, (f32)UI_ammo / (f32)UI_maxammo);
    playerHUD.Draw(0.f, 0.f, 1600.f, 900.f);

    // RAGE
    if (UI_rage)
    {
        playerHUD.Set_Row(UI_rage + 1);
        playerHUD.Loop(0, 3, .5f);
        playerHUD.Draw(0.f, 0.f, 1600.f, 900.f);
    }

	// BLOCK
	racecar.Set_Row(0);
	racecar.Set_Col(UI_parryCD);
	racecar.Draw(Mouse.x - 50.f, Mouse.y - 50.f, 30.f, 30.f);

	// DASH
	racecar.Set_Row(1);
	racecar.Set_Col(UI_dodgeCD);
	racecar.Draw(Mouse.x + 50.f, Mouse.y - 50.f, 30.f, 30.f);

	// COOLDOWN
	crosshair.Set_Row(0);
    crosshair.Set_Col(UI_ammo);
    crosshair.Draw(Mouse.x, Mouse.y, 80.f, 80.f);
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
	DrawRect(UI_IO[15], UI_IO[16], UI_IO[17], UI_IO[18], boss_hp_damage);
	Color hpcolor = boss_hp_green;
	if (UI_bosshp < 0.6f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_yellow;
	}
	if (UI_bosshp < 0.3f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_red;
	}
	DrawHPBar(UI_IO[19], UI_IO[20], UI_IO[21], UI_IO[22], UI_bosshp, UI_bossmaxhp, hpcolor);
}

// Draw the various UI elements
void Draw_UI(int DrawBossHP)
{
	Draw_Player_UI();
	if (DrawBossHP) { Draw_Boss_HPbar(); }
}

// Open the pause menu when player pauses the game
void Draw_PauseMenu() 
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	// Draw translucent background
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), SetColor(0x99000000));
	DrawRect(menubg_x, menubg_y, menubg_width, menubg_height, pause_bg);

	// Draw pause menu sprite
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, menu_x, menu_y, menu_width, menu_height, menu_rot);
	Set_SpriteSheet_current(&ssmenu_screen, 0, 1, 1, Create_Timer(1.f), 1.f);

	// Reset animations if no button is being hovered over
	if (uiscreen.isPlayClicked == 0 && uiscreen.isOptionsClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// Play Button (Click to resume)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, UI_IO[33], UI_IO[34], UI_IO[35], UI_IO[36], UI_IO[37]);

	if (IsAreaClicked(800.f + UI_IO[33], 450.f - UI_IO[34], UI_IO[35], UI_IO[36], UI_mouseX, UI_mouseY))
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

	// Options Button (Click to open options menu)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_optionsbutton, UI_IO[38], UI_IO[39], UI_IO[40], UI_IO[41], UI_IO[42]);

	if (IsAreaClicked(800.f + UI_IO[38], 450.f - UI_IO[39], UI_IO[40], UI_IO[41], UI_mouseX, UI_mouseY))
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

	// Exit Button (Click to open quit menu)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, UI_IO[43], UI_IO[44], UI_IO[45], UI_IO[46], UI_IO[47]);

	if (IsAreaClicked(800.f + UI_IO[43], 450.f - UI_IO[44], UI_IO[45], UI_IO[46], UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			levelState = INQUIT;
		}
	}
	else
	{
		uiscreen.isExitClicked = 0;
		Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
}

// Handle text input for player name
void HandleTextInput()
{
	// Prevent input if the name has already been submitted
	if (hasSubmitted)
		return;

	// Check if the backspace key is pressed and the player name is not empty
	if (AEInputCheckTriggered(AEVK_BACK) && !playerName.empty())
	{
		// Clear the entire string
		playerName.clear();
		isNameEntered = true;
	}

	// Only allow input if the name is less than 3 characters
	if (playerName.length() < 3)
	{
		// Loop through the possible printable characters (A-Z)
		for (int i = AEVK_A; i <= AEVK_Z; ++i) {
			if (AEInputCheckTriggered(static_cast<u8>(i))) {
				// Append the character to the player's name
				playerName.push_back(static_cast<char>('A' + (i - AEVK_A)));
			}
		}

		// Allow digits (0-9)
		for (int i = AEVK_0; i <= AEVK_9; ++i) {
			if (AEInputCheckTriggered(static_cast<u8>(i))) {
				// Append the number to the player's name
				playerName.push_back(static_cast<char>('0' + (i - AEVK_0)));
			}
		}

		// Allow the space bar to insert spaces
		if (AEInputCheckTriggered(AEVK_SPACE)) {
			playerName.push_back(' '); // Append a space to the player's name
		}
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

// Open the level complete screen when player beats the boss
void Draw_ClearMenu()
{
	// Flag to turn off player attacking
	player.canAttack = false;

	// Update level highest score / best time
	if (currentState == GS_LEVEL1)
	{
		// If highest recorded score is 0 or current score is higher than highest score...
		// Then current score becomes the new highest score
		if (UI_IO_score[0] == 0.f || currentScore > static_cast<int>(UI_IO_score[0]))
		{
			WriteToFile("Assets/FileIO/PlayerBestScores.txt", "Player Level 1 Highest Score", std::to_string(currentScore));
		}

		// If best recorded time is 0.f or current time is shorter than best time...
		// Then current time becomes the new best time
		if (UI_IO_score[1] == 0.f || levelTimer < static_cast<float>(UI_IO_score[1]))
		{
			WriteToFile("Assets/FileIO/PlayerBestScores.txt", "Player Level 1 Best Time", std::to_string(levelTimer));
		}
	}
	else if (currentState == GS_LEVEL2)
	{
		// If highest recorded score is 0 or current score is higher than highest score...
		// Then current score becomes the new highest score
		if (UI_IO_score[2] == 0.f || currentScore > static_cast<int>(UI_IO_score[2]))
		{
			WriteToFile("Assets/FileIO/PlayerBestScores.txt", "Player Level 2 Highest Score", std::to_string(currentScore));
		}

		// If best recorded time is 0.f or current time is shorter than best time...
		// Then current time becomes the new best time
		if (UI_IO_score[3] == 0.f || levelTimer2 < static_cast<float>(UI_IO_score[3]))
		{
			WriteToFile("Assets/FileIO/PlayerBestScores.txt", "Player Level 2 Best Time", std::to_string(levelTimer2));
		}
	}

	// Flag to enable user input
	isInputEnabled = true;

	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	// Draw translucent background
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), SetColor(0x99000000));
	DrawRect(menubg_x, menubg_y, menubg_width, menubg_height, pause_bg);  // Change pause_bg to be a translucent color

	// Draw clear menu sprite
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, menu_x, menu_y, menu_width, endmenu_height, menu_rot);
	Set_SpriteSheet_current(&ssmenu_screen, 2, 1, 1, Create_Timer(1.f), 1.f);

	// Reset animations if no button is being hovered over
	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0 && uiscreen.isButton1Clicked == 0 && uiscreen.isButton2Clicked == 0)
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
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, UI_IO[48], UI_IO[49], UI_IO[50], UI_IO[51], UI_IO[52]);

	// Prevent submission if already submitted
	// Check if mouse is over the button area
	bool isHoveringButton = IsAreaClicked(
		800.f + UI_IO[48], 450.f - UI_IO[49],
		UI_IO[50], UI_IO[51],
		UI_mouseX, UI_mouseY
	);

	// Visual feedback (hover effect)
	if (isHoveringButton)
	{
		uiscreen.isButton1Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_1, 7, 1, 8,
				uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
	}

	// Do NOT allow clicking the button if the name is invalid
	bool isNameValid = !(playerName.empty() || playerName == "BACKSPACE to Type" || playerName == "Error!");

	// Block submission if already submitted, name is invalid, or mouse not over button
	if (!hasSubmitted && isHoveringButton && isNameValid && AEInputCheckTriggered(AEVK_LBUTTON))
	{
		AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
		isLoading = true;
		hasSubmitted = true;

		std::thread submitThread([&]()
			{
				addLeaderboardEntry(playerName.c_str(), newScore, level_timer);
				isLoading = false;
				isInputSubmitted = UI_green;
			});
		submitThread.detach();
	}
	else if (!isNameValid && isHoveringButton && AEInputCheckTriggered(AEVK_LBUTTON))
	{
		// Optional feedback if user tries to click with invalid name
		std::cerr << "Submission blocked: Invalid name '" << playerName << "'\n";
		playerName = "Error!"; // Update name to show error visually
	}
	else if (hasSubmitted)
	{
		uiscreen.isButton1Clicked = 0;
		isInputEnabled = false;
		Set_SpriteSheet_current(&ssmenu_1, 7, 8, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
	else
	{
		uiscreen.isButton1Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_1, 7, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Replay button (click to go to Level1 state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, UI_IO[53], UI_IO[54], UI_IO[55], UI_IO[56], UI_IO[57]);

	if (IsAreaClicked(800.f + UI_IO[53], 450.f - UI_IO[54], UI_IO[55], UI_IO[56], UI_mouseX, UI_mouseY))
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
			isInputEnabled = false;
			player.canAttack = true;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 1, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Next button (click to go to Level 2 state)
	if (currentState == GS_LEVEL1)
	{
		Draw_SpriteSheet_test(*uiscreen.spritesheet_button2, 0.f, -300.f, 200.f, 100.f, 0.f);

		if (IsAreaClicked(800.f, 750.f, 200.f, 100.f, UI_mouseX, UI_mouseY))
		{
			uiscreen.isButton2Clicked = 1;

			if (Update_Timer(&uiscreen.animationtimer))
			{
				Set_SpriteSheet_current(&ssmenu_2, 9, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
			}

			if (AEInputCheckTriggered(AEVK_LBUTTON))
			{
				AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
				nextState = GS_LEVEL2;
				player.canAttack = true;
				isInputEnabled = false;
			}
		}
		else
		{
			uiscreen.isButton2Clicked = 0;
			Set_SpriteSheet_current(&ssmenu_2, 9, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
	}

	// Display score above player input
	DrawTextBox(UI_IO[63], UI_IO[64], UI_IO[65], UI_IO[66], UI_IO[67], pixel_font, UI_green, level_score);

	// Display time above player input
	DrawTextBox(UI_IO[68], UI_IO[69], UI_IO[70], UI_IO[71], UI_IO[72], pixel_font, UI_white, level_timer);

	// Display highest combo above player input
	snprintf(combo, sizeof(combo), "BEST COMBO: %d", highestCombo);
	DrawTextBox(UI_IO[73], UI_IO[74], UI_IO[75], UI_IO[76], UI_IO[77], pixel_font, UI_white, combo);

	static float cursorTimer = 0.0f;
	static bool showCursor = true;

	// Display blinking cursor if player name is not more than 3 characters
	if (playerName.length() < 3)
	{
		cursorTimer += static_cast<float>(AEFrameRateControllerGetFrameTime());
		if (cursorTimer >= 0.5f)
		{
			showCursor = !showCursor;
			cursorTimer = 0.0f;
		}
	}
	else
	{
		showCursor = false; // Disable cursor when input is submitted or 3 characters are reached
	}

	// Create a display name with a blinking cursor if the input is active
	std::string displayName = playerName;
	if (playerName.length() < 3 && showCursor) // Only show cursor when active
	{
		displayName += '|'; // Use a vertical bar as the cursor
	}

	// Test with a position close to the center of the screen
	if (!isNameEntered)
	{
		DrawTextBox(UI_IO[78], UI_IO[79], UI_IO[80], UI_IO[81], UI_IO[82], pixel_font, isInputSubmitted, displayName.c_str());
	}
	else
	{
		DrawTextBox(UI_IO[167], UI_IO[79], UI_IO[80], UI_IO[81], UI_IO[82], pixel_font, isInputSubmitted, displayName.c_str());
	}

	// Handle text input
	HandleTextInput();


	// Exit button (click to go back to hubworld state)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, UI_IO[58], UI_IO[59], UI_IO[60], UI_IO[61], UI_IO[62]);

	if (IsAreaClicked(800.f + UI_IO[58], 450.f - UI_IO[59], UI_IO[60], UI_IO[61], UI_mouseX, UI_mouseY))
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
			isInputEnabled = false;
			currentScore = 0;
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

	// Draw translucent background
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), SetColor(0x99000000));
	DrawRect(menubg_x, menubg_y, menubg_width, menubg_height, pause_bg);

	// Draw options menu sprite
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, menu_x, menu_y, menu_width, menu_height, menu_rot);
	Set_SpriteSheet_current(&ssmenu_screen, 3, 1, 1, Create_Timer(1.f), 1.f);

	// Reset animations if no button is being hovered over
	if (uiscreen.isButton1Clicked == 0 && uiscreen.isButton2Clicked == 0 && uiscreen.isButton3Clicked == 0 &&
		uiscreen.isButton4Clicked == 0 && uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// BGM Label
	DrawTextBox(UI_IO[83], UI_IO[84], UI_IO[85], UI_IO[86], UI_IO[87], UI_font, UI_black, "BGM");

	// BGM Bar
	DrawHPBar(UI_IO[88], UI_IO[89], UI_IO[90], UI_IO[91], AEClamp(current_bgm_vol, 0.f, max_bgm_vol), max_bgm_vol, UI_hover_red);
	Draw_SpriteSheet_test(*uiscreen.spritesheet_bar1, UI_IO[92], UI_IO[93], UI_IO[94], UI_IO[95], UI_IO[96]);
	Set_SpriteSheet_current(&ssmenu_bar1, 1, 1, 1, Create_Timer(1.f), 1.f);

	// Lower BGM Volume Button
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, UI_IO[97], UI_IO[98], UI_IO[99], UI_IO[100], UI_IO[101]);

	if (IsAreaClicked(800.f + UI_IO[97], 450.f - UI_IO[98], UI_IO[99], UI_IO[100], UI_mouseX, UI_mouseY))
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

	// Raise BGM Volume Button
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button2, UI_IO[102], UI_IO[103], UI_IO[104], UI_IO[105], UI_IO[106]);

	if (IsAreaClicked(800.f + UI_IO[102], 450.f - UI_IO[103], UI_IO[104], UI_IO[105], UI_mouseX, UI_mouseY))
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

	// SFX Label
	DrawTextBox(UI_IO[107], UI_IO[108], UI_IO[109], UI_IO[110], UI_IO[111], UI_font, UI_black, "SFX");

	// SFX Bar
	DrawHPBar(UI_IO[112], UI_IO[113], UI_IO[114], UI_IO[115], AEClamp(current_sfx_vol, 0.f, max_sfx_vol), max_sfx_vol, UI_hover_red);
	Draw_SpriteSheet_test(*uiscreen.spritesheet_bar2, UI_IO[116], UI_IO[117], UI_IO[118], UI_IO[119], UI_IO[120]);
	Set_SpriteSheet_current(&ssmenu_bar2, 1, 1, 1, Create_Timer(1.f), 1.f);

	// Lower SFX Volume Button
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button3, UI_IO[121], UI_IO[122], UI_IO[123], UI_IO[124], UI_IO[125]);

	if (IsAreaClicked(800.f + UI_IO[121], 450.f - UI_IO[122], UI_IO[123], UI_IO[124], UI_mouseX, UI_mouseY))
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

	// Raise SFX Volume Button
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button4, UI_IO[126], UI_IO[127], UI_IO[128], UI_IO[129], UI_IO[130]);

	if (IsAreaClicked(800.f + UI_IO[126], 450.f - UI_IO[127], UI_IO[128], UI_IO[129], UI_mouseX, UI_mouseY))
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

	// Save button (Click to save changes to volume)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, UI_IO[131], UI_IO[132], UI_IO[133], UI_IO[134], UI_IO[135]);

	if (IsAreaClicked(800.f + UI_IO[131], 450.f - UI_IO[132], UI_IO[133], UI_IO[134], UI_mouseX, UI_mouseY))
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
			WriteToFile("Assets/FileIO/Settings.txt", "Current BGM Volume", std::to_string(prev_bgm_vol));
			WriteToFile("Assets/FileIO/Settings.txt", "Current SFX Volume", std::to_string(prev_sfx_vol));
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 4, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Exit Button (Click to resume)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, UI_IO[136], UI_IO[137], UI_IO[138], UI_IO[139], UI_IO[140]);

	if (IsAreaClicked(800.f + UI_IO[136], 450.f - UI_IO[137], UI_IO[138], UI_IO[139], UI_mouseX, UI_mouseY))
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

	// Draw translucent background
	DrawRect(0.f, 0.f, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()), SetColor(0x99000000));
	DrawRect(menubg_x, menubg_y, menubg_width, menubg_height, pause_bg);

	// Draw quit menu sprite
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, menu_x, menu_y, menu_width, menu_height, menu_rot);
	Set_SpriteSheet_current(&ssmenu_screen, 4, 1, 1, Create_Timer(1.f), 1.f);

	// Reset animations if no button is being hovered over
	if (uiscreen.isPlayClicked == 0 && uiscreen.isExitClicked == 0)
	{
		uiscreen.animationtimer.current = 0.f;
	}

	// Display Message (dependent on current game state)
	if (currentState == GS_HUBWORLD)
	{
		DrawTextAdvanced(UI_IO[141], UI_IO[143], pixel_font, UI_white, "Are you sure you want to quit?", false, UI_IO[145]);
	}
	else
	{
		DrawTextAdvanced(UI_IO[141], UI_IO[142], pixel_font, UI_white, "Are you sure you want to quit?", false, UI_IO[145]);
		DrawTextAdvanced(UI_IO[141], UI_IO[143], pixel_font, UI_white, "Your current progress will be", false, UI_IO[145]);
		DrawTextAdvanced(UI_IO[141], UI_IO[144], pixel_font, UI_red, "LOST!!!", false, UI_IO[146]);
	}
	if (AEInputCheckTriggered(AEVK_ESCAPE)) {
		levelState = PLAYING;
	}

	// Back Button (Click to resume)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, UI_IO[147], UI_IO[148], UI_IO[149], UI_IO[150], UI_IO[151]);

	if (IsAreaClicked(800.f + UI_IO[147], 450.f - UI_IO[148], UI_IO[149], UI_IO[150], UI_mouseX, UI_mouseY))
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

	// Exit Button (Click to return to Hubworld if in game level, else if in Hubworld return to Main Menu)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, UI_IO[152], UI_IO[153], UI_IO[154], UI_IO[155], UI_IO[156]);

	if (IsAreaClicked(800.f + UI_IO[152], 450.f - UI_IO[153], UI_IO[154], UI_IO[155], UI_mouseX, UI_mouseY))
	{
		uiscreen.isExitClicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_exit, 2, 1, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
			currentScore = 0;
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
	// Flag to enable user input
	isInputEnabled = true;

	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	// Draw translucent background
	DrawRect(menubg_x, menubg_y, menubg_width, menubg_height, pause_bg);

	// Draw game over menu sprite
	Draw_SpriteSheet_test(*uiscreen.spritesheet_screen, menu_x, menu_y, menu_width, endmenu_height, menu_rot);
	Set_SpriteSheet_current(&ssmenu_screen, 1, 1, 1, Create_Timer(1.f), 1.f);

	// Reset animations if no button is being hovered over
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
	Draw_SpriteSheet_test(*uiscreen.spritesheet_button1, UI_IO[48], UI_IO[49], UI_IO[50], UI_IO[51], UI_IO[52]);

	// Prevent submission if already submitted
	// Check if mouse is over the button area
	bool isHoveringButton = IsAreaClicked(
		800.f + UI_IO[48], 450.f - UI_IO[49],
		UI_IO[50], UI_IO[51],
		UI_mouseX, UI_mouseY
	);

	// Visual feedback (hover effect)
	if (isHoveringButton)
	{
		uiscreen.isButton1Clicked = 1;

		if (Update_Timer(&uiscreen.animationtimer))
		{
			Set_SpriteSheet_current(&ssmenu_1, 7, 1, 8,
				uiscreen.animationtimer, uiscreen.animationtimer.duration);
		}
	}

	// Do NOT allow clicking the button if the name is invalid
	bool isNameValid = !(playerName.empty() || playerName == "BACKSPACE to Type" || playerName == "Error!");

	// Block submission if already submitted, name is invalid, or mouse not over button
	if (!hasSubmitted && isHoveringButton && isNameValid && AEInputCheckTriggered(AEVK_LBUTTON))
	{
		AEAudioPlay(levelSFX, gameSFX, current_sfx_vol / max_sfx_vol, 1.f, 0);
		isLoading = true;
		hasSubmitted = true;

		std::thread submitThread([&]()
			{
				addLeaderboardEntry(playerName.c_str(), newScore, level_timer);
				isLoading = false;
				isInputSubmitted = UI_green;
			});
		submitThread.detach();
	}
	else if (!isNameValid && isHoveringButton && AEInputCheckTriggered(AEVK_LBUTTON))
	{
		// Optional feedback if user tries to click with invalid name
		std::cerr << "Submission blocked: Invalid name '" << playerName << "'\n";
		playerName = "Error!"; // Update name to show error visually
	}

	else if (hasSubmitted)
	{
		uiscreen.isButton1Clicked = 0;
		isInputEnabled = false;
		Set_SpriteSheet_current(&ssmenu_1, 7, 8, 8, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}
	else
	{
		uiscreen.isButton1Clicked = 0;
		Set_SpriteSheet_current(&ssmenu_1, 7, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Replay Button (Click to restart Level 1)
	Draw_SpriteSheet_test(*uiscreen.spritesheet_playbutton, UI_IO[53], UI_IO[54], UI_IO[55], UI_IO[56], UI_IO[57]);

	if (IsAreaClicked(800.f + UI_IO[53], 450.f - UI_IO[54], UI_IO[55], UI_IO[56], UI_mouseX, UI_mouseY))
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
			isInputEnabled = false;
		}
	}
	else
	{
		uiscreen.isPlayClicked = 0;
		Set_SpriteSheet_current(&ssmenu_play, 1, 1, 1, uiscreen.animationtimer, uiscreen.animationtimer.duration);
	}

	// Display score above player input
	DrawTextBox(UI_IO[63], UI_IO[64], UI_IO[65], UI_IO[66], UI_IO[67], pixel_font, UI_red, level_score);

	// Display time above player input
	DrawTextBox(UI_IO[68], UI_IO[69], UI_IO[70], UI_IO[71], UI_IO[72], pixel_font, UI_white, level_timer);

	// Inside your menu or game loop
	Draw_SpriteSheet_test(*uiscreen.spritesheet_exitbutton, UI_IO[58], UI_IO[59], UI_IO[60], UI_IO[61], UI_IO[62]);
	// Test with a position close to the center of the screen
	if (!isNameEntered)
	{
		DrawTextBox(UI_IO[78], UI_IO[79], UI_IO[80], UI_IO[81], UI_IO[82], pixel_font, isInputSubmitted, playerName.c_str());
	}
	else
	{
		DrawTextBox(UI_IO[167], UI_IO[79], UI_IO[80], UI_IO[81], UI_IO[82], pixel_font, isInputSubmitted, playerName.c_str());
	}

	// Handle text input
	HandleTextInput();

	// Exit Button (Click to return to Hubworld)
	if (IsAreaClicked(800.f + UI_IO[58], 450.f - UI_IO[59], UI_IO[60], UI_IO[61], UI_mouseX, UI_mouseY))
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
			isInputEnabled = false;
			currentScore = 0;
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

	DrawTextBox(UI_IO[157], UI_IO[158], UI_IO[159], UI_IO[160], UI_IO[161], pixel_font, UI_white, level_timer);
}


// Draw the level score
void Draw_LevelScore()
{
	snprintf(level_score, sizeof(level_score), "Score: %d", currentScore);

	DrawTextBox(UI_IO[162], UI_IO[163], UI_IO[164], UI_IO[165], UI_IO[166], pixel_font, UI_white, level_score);
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
	pointsAccumulated += int(value * Modifier_ScoreScale[player.Mod_DmgTakenIndex]);
	displayPointsAccumulated = true;
	displayPointsTimer.current = 0.f;
}

// Draw accumulated points
void Draw_Points()
{
	std::cout << pointsAccumulated << '\n';
	if (levelState == PLAYING || levelState == CLEARED)
	{
		// While combo is active
		if (Update_Timer(&displayPointsTimer) && pointsAccumulated != 0)
		{
			// Displayed only if player beats level without taking any damage
			if (isPerfect)
			{
				DrawTextAdvanced(player.pos.x, player.pos.y + 280.f, pixel_font, UI_gold, "PERFECT!!!", true, 1.f);
			}

			// COMBO (Score Type)
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
		// If combo ends after a set duration or when player takes damage
		// Add accumulated points to current score and reset current combo
		// Update highest combo if current combo is higher
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

}

// Updates the current level score while level is playing
void Update_LevelScore()
{
	if (currentScore + 1000000 < newScore)
	{
		currentScore += 500000;
	}
	else if (currentScore + 100000 < newScore)
	{
		currentScore += 50000;
	}
	else if (currentScore + 10000 < newScore)
	{
		currentScore += 5000;
	}
	else if (currentScore < newScore)
	{
		currentScore += 500;
	}
	else
	{
		currentScore = newScore;
	}
}

// Add new entry to leaderboard
void addLeaderboardEntry(const std::string& name, int score, const std::string& timing) {

	if (name == "BACKSPACE to Type" || name.empty()) {
		std::cerr << "Error: Please enter a valid name before submitting!" << std::endl;
		return;  // Exit early if the name is invalid
	}

	char buffer[MAX_PATH];
	if (_getcwd(buffer, MAX_PATH) == nullptr) {
		std::cerr << "Error: Failed to get current directory!" << std::endl;
		return;
	}

	// Determine leaderboard based on game state
	std::string leaderboardCategory;
	switch (currentState) {
	case GS_LEVEL1:
		leaderboardCategory = "Boss1";
		break;
	case GS_LEVEL2:
		leaderboardCategory = "Boss2";
		break;
	default:
		std::cerr << "Error: Leaderboard is only available for boss levels!" << std::endl;
		return;
	}

	std::string command = "\"\"" + std::string(buffer) + "\\Assets\\dist\\leaderboard_helper.exe\" \""
		+ leaderboardCategory + "\" \""  // Use game state to determine leaderboard
		+ name + "\" \""
		+ std::to_string(score) + "\" \""
		+ timing + "\"\"";

	std::cout << "Adding leaderboard entry for " << leaderboardCategory << "..." << std::endl;
	int result = system(command.c_str());

	if (result != 0) {
		std::cout << "Error adding leaderboard entry for " << leaderboardCategory << "!" << std::endl;
	}
	else {
		std::cout << "Leaderboard entry added successfully for " << leaderboardCategory << "!" << std::endl;
		isInputSubmitted = UI_green;
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

