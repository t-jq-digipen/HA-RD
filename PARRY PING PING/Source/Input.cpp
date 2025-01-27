#include "Input.h"


AEVec2 Mouse;

void Input_Handle()
{
	s32 mouse_x, mouse_y;
	AEInputGetCursorPosition(&mouse_x, &mouse_y);
	AEVec2Set(&Mouse, (f32)mouse_x - (AEGfxGetWindowWidth() / 2.f), -(f32)mouse_y + (AEGfxGetWindowHeight() / 2.f));
}