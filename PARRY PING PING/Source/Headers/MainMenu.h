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

// ----------------------------------------------------------------------------
// Structure for main menu elements
// ----------------------------------------------------------------------------
typedef struct MainMenu
{
	// Main Menu Button Spritesheets
	SpriteSheet_Old* spritesheet_button1;
	SpriteSheet_Old* spritesheet_button2;
	SpriteSheet_Old* spritesheet_button3;

	Timer animationtimer;		// Main menu button animation timer
	int isPlayClicked;			// Checks if play button is hovered
	int isExitClicked;			// Checks if exit button is hovered
	int isCreditsClicked;		// Checks if credits button is hovered
} MainMenu;

// ----------------------------------------------------------------------------
// Loads the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Load();

// ----------------------------------------------------------------------------
// Initializes the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Initialize();

// ----------------------------------------------------------------------------
// Updates the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Update();

// ----------------------------------------------------------------------------
// Draws the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Draw();

// ----------------------------------------------------------------------------
// Frees the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Free();

// ----------------------------------------------------------------------------
// Unloads the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Unload();