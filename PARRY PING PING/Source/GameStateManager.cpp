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

int currentState = 0;

#include "GameStateManager.h"

void GSM_Initialize(int gameState)
{
	currentState = gameState;
}

void GSM_Update()
{
	switch (currentState)
	{
	case GS_MAIN_MENU:
		break;
	case GS_LEVEL:
		break;
	case GS_QUIT:
		break;
	default:
		break;
	}
}