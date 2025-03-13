//---------------------------------------------------------
// file:	Camera.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Camera function declarations for rendering
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"

// ----------------------------------------------------------------------------
// Initializes the camera origin and starting coordinates
// ----------------------------------------------------------------------------
void Init_Camera();

// ----------------------------------------------------------------------------
// Updates the camera position as well as mouse shift
// ----------------------------------------------------------------------------
void Update_Camera();

// ----------------------------------------------------------------------------
// Convert world coordinates to screen coordinates, primarily for rendering
// ----------------------------------------------------------------------------
f32 World_To_Screen_x(f32 pos_x);
f32 World_To_Screen_y(f32 pos_y);

// ----------------------------------------------------------------------------
// Convert screen coordinates to world coordinates, for interacting with
// rendered elements
// ----------------------------------------------------------------------------
f32 Screen_To_World_x(f32 pos_x);
f32 Screen_To_World_y(f32 pos_y);

