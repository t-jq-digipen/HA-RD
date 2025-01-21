//---------------------------------------------------------
// file:	GameStateManager.cpp
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Implementation of game state manager functions and
//			enum game states
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "GameStateManager.h"
#include "MainMenu.h"
#include "Level1.h"

int currentState = 0, previousState = 0, nextState = 0;

FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;

void GSM_Initialize(int gameState)
{
	currentState = gameState;
}

void GSM_Update()
{
	switch (currentState)
	{
	case GS_MAIN_MENU:
		fpLoad = MainMenu_Load;
		fpInitialize = MainMenu_Initialize;
		fpUpdate = MainMenu_Update;
		fpDraw = MainMenu_Draw;
		fpFree = MainMenu_Free;
		fpUnload = MainMenu_Unload;
		break;
	case GS_LEVEL1:
		fpLoad = Level1_Load;
		fpInitialize = Level1_Initialize;
		fpUpdate = Level1_Update;
		fpDraw = Level1_Draw;
		fpFree = Level1_Free;
		fpUnload = Level1_Unload;
		break;
	case GS_RESTART:
		break;
	case GS_QUIT:
		break;
	default:
		break;
	}
}