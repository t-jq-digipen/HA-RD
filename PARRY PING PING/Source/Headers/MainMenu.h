//---------------------------------------------------------
// File:	MainMenu.h
// Project: Heavy Assault : Rust & Dust
// Author:	Manfred Sim
//
// Brief:	Declaration of main menu state functions.
//          Player may enter the game, credits page, or quit
//          the application from this screen.
//			Holds a main menu struct which manages the
//			various buttons in the main menu.
//
// Copyright © 2025 DigiPen, All rights reserved.
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
	SpriteSheet_Old* spritesheet_logo;

	Timer animationtimer;		// Main menu button animation timer
	Timer animationlogotimer;	// Main menu logo animation timer
	Timer logoresettimer;		// Main menu logo reset timer
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