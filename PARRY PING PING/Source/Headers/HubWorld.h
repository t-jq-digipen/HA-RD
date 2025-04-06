//---------------------------------------------------------
// File:	HubWorld.h
// Project: Heavy Assault : Rust & Dust
// Author:	
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 70%
//			- Dummies
//			- Base level state
//			- Interactable UI (Leaderboard shrine, Modifiers)
//			- Level transitions
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 25%
//			- Leaderboard fetching
//			- Audio
// 
//			Manfred Sim ------------------------------- 5%
//			- extern variables
//
// Brief:	Declaration of functions for Hub World state
//			and leaderboard updating.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
extern bool InTop20;
extern bool InModifiers;

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

void fetchLeaderboard(int bossID);

void FetchLeaderboardAsync();

void WaitForLeaderboardFetch();
		