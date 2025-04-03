//---------------------------------------------------------
// file:	HubWorld.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Declaration of Hub World functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void HubWorld_Load();			// Load Hub World resources
void HubWorld_Initialize();		// Initialize Hub World elements, such as practice range dummy, assets, UI etc.
void HubWorld_Update();			// Handles logic on per frame basis, mainly used to update logic for practice range dummies and logic for interactables
void HubWorld_Draw();			// Draws assets for props, player, UI etc.
void HubWorld_Free();			// Free memory allocated for Hub World elements
void HubWorld_Unload();			// Unload Hub World resources


// ----------------------------------------------------------------------------
// Controls fade to/from dark transitions for Hub World interactables
// ----------------------------------------------------------------------------
void Draw_FadeIn(Color& Fade);	// Draws a Fade from Black transition
void Draw_FadeOut(Color& Fade);	// Draws a Fade to Black transition

// ----------------------------------------------------------------------------
// Draws top 3 recorded scores
// ----------------------------------------------------------------------------
void Draw_ShrineBoard();

void Draw_ModifiersBoard();

void fetchLeaderboard();

void FetchLeaderboardAsync();
		