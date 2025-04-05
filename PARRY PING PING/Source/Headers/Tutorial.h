//---------------------------------------------------------
// File:	Tutorial.h
// Project: Heavy Assault : Rust & Dust
// Author:	Tan Jun Quan
//
// Brief:	Tutorial overlay declaration
//			Manages and guides player through a series of
//			gameplay mechanics, including movement, attacks,
//			and parrying.
//			The overlay shows the player what buttons to
//			press or where to go.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"
#include "SpriteSheet.h"


// Tutorial Active Flag
extern bool tutorialActive;

// ----------------------------------------------------------------------------
// Initialize the tutorial elements
// ----------------------------------------------------------------------------
void Init_Tutorial();

// ----------------------------------------------------------------------------
// Load the tutorial elements
// ----------------------------------------------------------------------------
void Load_Tutorial();

// ----------------------------------------------------------------------------
// Unload the tutorial elements
// ----------------------------------------------------------------------------
void Unload_Tutorial();

// ----------------------------------------------------------------------------
// Update the tutorial elements
// ----------------------------------------------------------------------------
void Update_Tutorial();

// ----------------------------------------------------------------------------
// Draw the tutorial elements
// ----------------------------------------------------------------------------
void Draw_Tutorial();