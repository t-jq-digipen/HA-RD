#include "Camera.h"
#include "player.h"


AEVec2 cam_origin, cam_pos, cam_shift, mouse_shift;

void Init_Camera() 
{
	cam_origin.x = cam_pos.x = player.pos.x;
	cam_origin.y = cam_pos.y = player.pos.y;
	AEVec2Zero(&mouse_shift);
}

void Update_Camera() 
{
	// Update camera position to player
	cam_pos.x = player.pos.x;
	cam_pos.y = player.pos.y;

	// Update shift amount
	cam_shift.x = cam_pos.x - cam_origin.x;
	cam_shift.y = cam_pos.y - cam_origin.y;

	// Update mouse shift
	AEVec2Set(&mouse_shift, Mouse_X/2.f, Mouse_Y/2.f);
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