//---------------------------------------------------------
// file:	Tutorial.h
// project: Heavy Assault : Rust & Dust
// author:	Tan Jun Quan
//			Manfred Sim
//
// brief:	Tutorial overlay declaration
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