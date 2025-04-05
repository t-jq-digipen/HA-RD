//---------------------------------------------------------
// File:	GameStateManager.h
// Project: Heavy Assault : Rust & Dust
// Author:	Manfred Sim
//
// Brief:	Declaration of game state manager functions and
//			enum game states
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

// ----------------------------------------------------------------------------
// Enumeration for game state types
// ----------------------------------------------------------------------------
enum GS_STATES
{
	GS_MAIN_MENU = 0,
	GS_RESTART,
	GS_LEVEL1,
	GS_LEVEL2,
	GS_HUBWORLD,
	GS_CREDITS,
	GS_QUIT
};

// ----------------------------------------------------------------------------
// Function pointer
// ----------------------------------------------------------------------------
typedef void(*FP)(void);

// ----------------------------------------------------------------------------
// Game states
// ----------------------------------------------------------------------------
extern int currentState, previousState, nextState;

// ----------------------------------------------------------------------------
// Function pointers for respective game state
// ----------------------------------------------------------------------------
extern FP fpLoad, fpInitialize, fpUpdate, fpDraw, fpFree, fpUnload;

// ----------------------------------------------------------------------------
// Initialize the game state manager
// ----------------------------------------------------------------------------
void GSM_Initialize(int gameState);

// ----------------------------------------------------------------------------
// Update the game state manager
// ----------------------------------------------------------------------------
void GSM_Update();