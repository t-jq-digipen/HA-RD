//---------------------------------------------------------
// file:	Credits.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Credits function declarations
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void Credits_Load();			// Load Credits page
void Credits_Initialize();		// Initialize Credits elements
void Credits_Update();			// Handles logic on per frame basis, mainly used to update credits elements
void Credits_Draw();			// Draws assets for credits
void Credits_Free();			// Free memory allocated for Credits elements
void Credits_Unload();			// Unload Credits page
