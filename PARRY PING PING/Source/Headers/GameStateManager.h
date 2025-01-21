//---------------------------------------------------------
// file:	GameStateManager.h
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Declaration of game state manager functions and
//			enum game states
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

enum GS_STATES
{
	GS_MAIN_MENU = 0,
	GS_RESTART,
	GS_LEVEL1,
	GS_QUIT
};

typedef void(*FP)(void);

extern int currentState, previousState, nextState;

extern FP fpLoad, fpInitialize, fpUpdate, fpDraw, fpFree, fpUnload;

void GSM_Initialize(int gameState);
void GSM_Update();