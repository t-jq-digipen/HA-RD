//---------------------------------------------------------
// File:	Camera.cpp
// Project: Heavy Assault : Rust & Dust
// Authors:	
//			Tan Jun Quan ------------------------------ 60%
//			- Base camera logic based on player and mouse
//			- World/Screen conversion functions
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 40%
//			- Camera panning
//			- Cinematic camera lock
//
// Brief:	Camera function declarations for rendering
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "Camera.h"
#include "player.h"
#include "UI.h"


AEVec2	cam_origin,		// Spawn of camera
		cam_pos,		// Position of camera
		cam_shift,		// Offset between camera spawn and current position
		mouse_shift,	// Offset based on mouse movement
		temp_campos;	// Stores locked position of camera

bool LockCamera = false;
bool CamPanning = false;

f32 currVert{}, currHor{}, deccel{4.f};

void Init_Camera() 
{
	// Camera starts at same position as player
	cam_origin.x = cam_pos.x = player.pos.x;
	cam_origin.y = cam_pos.y = player.pos.y;
	AEVec2Zero(&mouse_shift);
}

void Update_Camera() 
{
	if (isInputEnabled) return;  // If input is disabled, do nothing
	if (LockCamera || CamPanning) return;

	currVert = currHor = 0.f;
	deccel = 4.f;
	// Update camera position to player
	cam_pos.x = player.pos.x;
	cam_pos.y = player.pos.y;

	// Update shift amount
	
	cam_shift.x = cam_pos.x - cam_origin.x;
	cam_shift.y = cam_pos.y - cam_origin.y;

	// Update mouse shift
	AEVec2Set(&mouse_shift, Mouse.x/3.5f, Mouse.y/3.f);
	AEVec2Add(&cam_shift, &cam_shift, &mouse_shift);
}

// World space coordinates to screen space
f32 World_To_Screen_x(f32 pos_x)
{
	return pos_x - cam_shift.x;
}

f32 World_To_Screen_y(f32 pos_y)
{
	return pos_y - cam_shift.y;
}


// Screen space coordinates to world space
f32 Screen_To_World_x(f32 pos_x)
{
	return pos_x + cam_shift.x;
}

f32 Screen_To_World_y(f32 pos_y)
{
	return pos_y + cam_shift.y;
}

// Pans camera with specified distance and speed (accelerated because it looks cool)
void Pan_Camera(f32 dir_hor, f32 dir_vert, f32 speed) {
	LockCamera = true;										// Locks camera, prevents it from being controlled with mouse/ movement
	currVert += speed * deccel;								// Incr variables with acceleration values (goes from fast to slow)
	currHor += speed * deccel;
	deccel *= 0.9f;

	cam_shift.x = cam_pos.x - cam_origin.x;					// Set to starting position PRE-ADDING OFFSET			
	cam_shift.y = cam_pos.y - cam_origin.y;
	temp_campos = cam_pos;
	
	if (currVert >= dir_vert) {								// Clamp vals
		currVert = dir_vert;
	}
	if (currHor >= dir_hor) {
		currHor = dir_hor;
	}

	AEVec2Set(&temp_campos, currHor, currVert);				// Set offsets to temp position
	AEVec2Add(&cam_shift, &cam_shift, &mouse_shift);		// Add current camera positioning
	AEVec2Add(&cam_shift, &cam_shift, &temp_campos);		// Add offsets
}

// Resets the camera
void ResetCamera() {
	// Reset camera to player position
	// Reset values and deccel
	currVert = currHor = 0.f;
	deccel = 4.f;

	cam_pos.x = player.pos.x;								// Set position to lock in on player
	cam_pos.y = player.pos.y;

	// Update shift amount
	cam_shift.x = cam_pos.x - cam_origin.x;
	cam_shift.y = cam_pos.y - cam_origin.y;

	// Update mouse shift
	AEVec2Set(&mouse_shift, Mouse.x / 3.5f, Mouse.y / 3.f);
	AEVec2Add(&cam_shift, &cam_shift, &mouse_shift);
}