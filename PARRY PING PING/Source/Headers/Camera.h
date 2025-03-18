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

// ----------------------------------------------------------------------------
// Just true or false to stop camera panning with mouse (default to true!)
// ----------------------------------------------------------------------------
extern bool LockCamera;

// ----------------------------------------------------------------------------
// Pans camera to direction, (-1: Left/ Down, 1: Right/ Up)
// Distance: Desired distance to pan, Speed: Units travelled per frame
// ----------------------------------------------------------------------------
//void Pan_Camera(s8 dir_vert, s8 dir_hor, f32 distance, f32 speed);