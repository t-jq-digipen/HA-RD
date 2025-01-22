#include "Input.h"


f32 Mouse_X, Mouse_Y;
AEVec2 Mouse;

void Input_Handle()
{
	s32 mouse_x, mouse_y;
	AEInputGetCursorPosition(&mouse_x, &mouse_y);
	Mouse_X = (f32)mouse_x - (AEGfxGetWindowWidth() / 2.f);
	Mouse_Y = -(f32)mouse_y + (AEGfxGetWindowHeight() / 2.f);
	AEVec2Set(&Mouse, Mouse_X, Mouse_Y);
}