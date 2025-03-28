//---------------------------------------------------------
// file:	Camera.cpp
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Camera function implementation for rendering
//			Recalculates position coordinates for conversion
//			between world and screen
//
// Copyright © 2020 DigiPen, All rights reserved.
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
	if (isInputDisabled) return;  // If input is disabled, do nothing
	if (LockCamera && CamPanning) return;

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

f32 World_To_Screen_x(f32 pos_x)
{
	return pos_x - cam_shift.x;
}

f32 World_To_Screen_y(f32 pos_y)
{
	return pos_y - cam_shift.y;
}

f32 Screen_To_World_x(f32 pos_x)
{
	return pos_x + cam_shift.x;
}

f32 Screen_To_World_y(f32 pos_y)
{
	return pos_y + cam_shift.y;
}

void Pan_Camera(f32 dir_hor, f32 dir_vert, f32 speed) {
	LockCamera = true;
	currVert += speed * deccel;
	currHor += speed * deccel;
	deccel *= 0.9f;

	cam_shift.x = cam_pos.x - cam_origin.x;
	cam_shift.y = cam_pos.y - cam_origin.y;
	temp_campos = cam_pos;

	// Update mouse shift
	if (currVert >= dir_vert) {
		currVert = dir_vert;
	}
	if (currHor >= dir_hor) {
		currHor = dir_hor;
	}
	AEVec2Set(&temp_campos, currHor, currVert);
	AEVec2Add(&cam_shift, &cam_shift, &mouse_shift);
	AEVec2Add(&cam_shift, &cam_shift, &temp_campos);
}
