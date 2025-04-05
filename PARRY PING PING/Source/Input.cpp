//---------------------------------------------------------
// File:	GameStateManager.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Tan Jun Quan
//
// Brief:	Input handler for mouse position
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "Input.h"


AEVec2 Mouse;

void Input_Handle()
{
	s32 mouse_x, mouse_y;
	AEInputGetCursorPosition(&mouse_x, &mouse_y);
	AEVec2Set(&Mouse, (f32)mouse_x - (AEGfxGetWindowWidth() / 2.f), -(f32)mouse_y + (AEGfxGetWindowHeight() / 2.f));
}