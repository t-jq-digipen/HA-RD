//---------------------------------------------------------
// file:	UI.h
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Declaration for all UI elements
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

enum LEVEL_STATE
{
	PLAYING = 0,
	PAUSED,
	GAMEOVER,
	OPTIONS,
	CLEARED
};

extern int levelState;

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
	int isPlayClicked;
	int isOptionsClicked;
	int isExitClicked;
	int isButton1Clicked, isButton2Clicked, isButton3Clicked, isButton4Clicked;
} UIScreen;

//1 - draw boss hp bar, 0 - dont draw
void Draw_UI(int DrawBossHP = 1);

void Update_Player_UI(int ammo, float hp, float parry_timer);

// will neaten when more player parameters are finalized

void Init_UI();

void Init_Player_UI(int ammo, int maxammo, float hp, float maxhp);

void Init_Boss_UI(float hp);

void Update_Boss_UI(float hp);

void Update_LevelTimer(float *time_elapsed, float dt);

void Unload_UI();

//void Draw_Boss1HP(float time_elapsed);

void Draw_PauseMenu();

void Draw_GameOverMenu();

void Draw_OptionsMenu();

void Draw_ClearMenu();

void Draw_LevelTimer(float time_elapsed);