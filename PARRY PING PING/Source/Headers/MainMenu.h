//---------------------------------------------------------
// file:	MainMenu.h
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Declaration of main menu functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once

typedef struct MainMenu
{
	SpriteSheet* spritesheet_button1;
	SpriteSheet* spritesheet_button2;

	Timer animationtimer;
	int isPlayClicked;
	int isExitClicked;
} MainMenu;

void MainMenu_Load();

void MainMenu_Initialize();

void MainMenu_Update();

void MainMenu_Draw();

void MainMenu_Free();

void MainMenu_Unload();