//---------------------------------------------------------
// File:	GameStateManager.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Manfred Sim
//
// Brief:	Implementation of game state manager functions and
//			enum game states
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "GameStateManager.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "HubWorld.h"
#include "Credits.h"

// Game states
int currentState = 0, previousState = 0, nextState = 0;

// Function pointers
FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;

// Initialize the game state manager
void GSM_Initialize(int gameState)
{
	currentState = gameState;
}

// Update the game state manager
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
	case GS_LEVEL2:
		fpLoad = Level2_Load;
		fpInitialize = Level2_Initialize;
		fpUpdate = Level2_Update;
		fpDraw = Level2_Draw;
		fpFree = Level2_Free;
		fpUnload = Level2_Unload;
		break;
	case GS_HUBWORLD:
		fpLoad = HubWorld_Load;
		fpInitialize = HubWorld_Initialize;
		fpUpdate = HubWorld_Update;
		fpDraw = HubWorld_Draw;
		fpFree = HubWorld_Free;
		fpUnload = HubWorld_Unload;
		break;
	case GS_CREDITS:
		fpLoad = Credits_Load;
		fpInitialize = Credits_Initialize;
		fpUpdate = Credits_Update;
		fpDraw = Credits_Draw;
		fpFree = Credits_Free;
		fpUnload = Credits_Unload;
		break;
	case GS_RESTART:
		break;
	case GS_QUIT:
		break;
	default:
		break;
	}
}