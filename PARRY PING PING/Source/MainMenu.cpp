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

SpriteSheet_Old playbutton, exitbutton;
MainMenu mainmenu;

void MainMenu_Load()
{
	// Meshes
	Load_Mesh(20);

	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	playbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);
	exitbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 7, 8, 1.f);

	std::cout << "Main Menu Loaded\n";
}

void MainMenu_Initialize()
{
	std::cout << "Main Menu Initialized\n";

	// Colors
	red = SetColor(0xFFFF6961);
	//green = SetColor(0xFF88B984);
	//blue = SetColor(0xFF00008B);
	//black = SetColor(0xFF000000);
	//white = SetColor(0xFFFFFFFF);
	//hover_red = SetColor(0xFFFFA5A1);

	// Window width, height
	width = (f32)AEGfxGetWindowWidth();
	height = (f32)AEGfxGetWindowHeight();

	mainmenu.spritesheet_button1 = &playbutton;
	mainmenu.spritesheet_button2 = &exitbutton;
	mainmenu.animationtimer = Create_Timer(0.5f);
	mainmenu.isPlayClicked = mainmenu.isExitClicked = 0;
}

void MainMenu_Update()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&mouseX, &mouseY);

	Get_DT();

	// Basic way to trigger exiting the application
	// when ESCAPE is hit or when the window is closed
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		nextState = GS_QUIT;
	}

	//temp testing for hub world
	if (AEInputCheckTriggered(AEVK_H)) {
		nextState = GS_HUBWORLD;
	}

	// game title
	DrawTextBox(-0.3f, 0.5f, width, 200.f, 3.f, pFont, red, "HARD");

	if (mainmenu.isPlayClicked == 0 && mainmenu.isExitClicked == 0)
	{
		mainmenu.animationtimer.current = 0.f;
	}

	// play button (click to go to Level1 state)
	Draw_SpriteSheet_test(*mainmenu.spritesheet_button1, -300.f, 0.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(width / 2 - 300.f, height / 2, 300.f, 200.f, mouseX, mouseY))
	{
		
		mainmenu.isPlayClicked = 1;

		if (Update_Timer(&mainmenu.animationtimer))
		{
			Set_SpriteSheet_current(&playbutton, 0, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_LEVEL1;
		}
	}
	else
	{
		mainmenu.isPlayClicked = 0;
		Set_SpriteSheet_current(&playbutton, 0, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
	}

	// exit button (click to go to quit state)
	Draw_SpriteSheet_test(*mainmenu.spritesheet_button2, 300.f, 0.f, 300.f, 200.f, 0.f);

	if (IsAreaClicked(width / 2 + 300.f, height / 2, 300.f, 200.f, mouseX, mouseY))
	{
		mainmenu.isExitClicked = 1;

		if (Update_Timer(&mainmenu.animationtimer))
		{
			Set_SpriteSheet_current(&exitbutton, 2, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_QUIT;
		}
	}
	else
	{
		mainmenu.isExitClicked = 0;
		Set_SpriteSheet_current(&exitbutton, 2, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
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
	AEGfxDestroyFont(pFont);
	Unload_Mesh();
	Unload_SpriteSheet(&playbutton);
	Unload_SpriteSheet(&exitbutton);
}