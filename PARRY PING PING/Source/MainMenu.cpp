//---------------------------------------------------------
// file:	MainMenu.cpp
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Implementation of main menu functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "MainMenu.h"
#include "GameStateManager.h"

s8 pFont;
Color red, green, blue, black, white, hover_red;
f32 width, height;
s32 mouseX, mouseY;

void MainMenu_Load()
{
	std::cout << "Main Menu Loaded\n";
}

void MainMenu_Initialize()
{
	std::cout << "Main Menu Initialized\n";

	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	// Meshes
	InitMesh(20);

	// Colors
	red = SetColor(0xFFFF6961);
	green = SetColor(0xFF88B984);
	blue = SetColor(0xFF00008B);
	black = SetColor(0xFF000000);
	white = SetColor(0xFFFFFFFF);
	hover_red = SetColor(0xFFFFA5A1);

	// Window width, height
	width = (f32)AEGfxGetWindowWidth();
	height = (f32)AEGfxGetWindowHeight();
}

void MainMenu_Update()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&mouseX, &mouseY);

	// Basic way to trigger exiting the application
	// when ESCAPE is hit or when the window is closed
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		nextState = GS_QUIT;
	}

	// game title
	DrawTextBox(-0.3f, 0.5f, width, 200.f, 3.f, pFont, red, "HARD");

	// play button (click to go to Level1 state)
	if (IsAreaClicked(width / 2 - 300.f, height / 2, 300.f, 200.f, mouseX, mouseY))
	{
		std::cout << "Click Play\n";
		DrawRect(-300.f, 0.f, 300.f, 200.f, hover_red);
		DrawTextBox(-0.5f, 0.f, 300.f, 200.f, 1.f, pFont, white, "PLAY");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_LEVEL1;
		}
	}
	else
	{
		DrawRect(-300.f, 0.f, 300.f, 200.f, green);
		DrawTextBox(-0.5f, 0.f, 300.f, 200.f, 1.f, pFont, black, "PLAY");
	}

	// exit button (click to go to quit state)
	if (IsAreaClicked(width / 2 + 300.f, height / 2, 300.f, 200.f, mouseX, mouseY))
	{
		std::cout << "Click Exit\n";
		DrawRect(300.f, 0.f, 300.f, 200.f, hover_red);
		DrawTextBox(0.25f, 0.f, 300.f, 200.f, 1.f, pFont, white, "EXIT");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_QUIT;
		}
	}
	else
	{
		DrawRect(300.f, 0.f, 300.f, 200.f, red);
		DrawTextBox(0.25f, 0.f, 300.f, 200.f, 1.f, pFont, black, "EXIT");
	}
}

void MainMenu_Draw()
{

}

void MainMenu_Free()
{

}

void MainMenu_Unload()
{
	
}