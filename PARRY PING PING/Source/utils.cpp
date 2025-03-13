//---------------------------------------------------------
// file:	utils.cpp
// author:	TAN JUN QUAN, MUHAMMAD FAUZAN BIN MOHAMED FAUZY
// email:	t.junquan@digipen.edu, muhammadfauzan.b@digipen.edu
//
// brief:	Collection of utility functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include <vector>
#include <algorithm>
#include "Camera.h"


// Globals
// -------------------------------------------------
#pragma region GLOBALS
AEGfxVertexList* CircleMesh;
AEGfxVertexList* SquareMesh;
AEGfxVertexList* ImgBaseMesh;

// for dt and time slow
f32 dt;
const f32 BULLET_TIME = 0.3f;
Timer bullet_time = { 1.f,1.f,1.f };

void Get_DT()
{
	dt = (f32)AEFrameRateControllerGetFrameTime();
	dt = AEClamp(dt, 0.f, 0.1f);					// prevent weird behaviour from lag spike

	if (Update_Timer(&bullet_time))
	{
		dt *= BULLET_TIME;
	}
}

#pragma endregion

// COLOR STRUCT
#pragma region COLOR
Color SetColor(u32 HexCode)
{
	Color color;									// temp struct
	color.A = ((HexCode >> 24) & 0xFF) / 255.0f;	// Alpha
	color.R = ((HexCode >> 16) & 0xFF) / 255.0f;	// Red
	color.G = ((HexCode >> 8) & 0xFF) / 255.0f;		// Green
	color.B = (HexCode & 0xFF) / 255.0f;			// Blue

	return color;
}
#pragma endregion



// TIMER FUNCTIONS
#pragma region TIMER
Timer Create_Timer(float duration)
{
	Timer temp;
	temp.current = 1.f;
	temp.end = 1.f;
	temp.duration = duration;
	return temp;
}

//bool Update_Timer(Timer* timer)
//{
//	if (timer->current < timer->end)
//	{
//		timer->current += dt / timer->duration;
//
//		return true;
//	}
//	else
//	{
//		timer->current = timer->end;
//	}
//	return false;
//}

//void Loop_Timer(Timer* timer)
//{
//	if (timer->current < timer->end)
//	{
//		timer->current += dt / timer->duration;
//	}
//	else
//	{
//		timer->current = 0;
//	}
//}

bool Update_Timer(Timer* timer)
{
	timer->current += dt / timer->duration;

	if (timer->current < timer->end)
	{
		return true;
	}
	else
	{
		timer->current = timer->end;
	}
	return false;
}

void Loop_Timer(Timer* timer)
{
	timer->current += dt / timer->duration;
	if (timer->current > timer->end)
	{
		timer->current = 0;
	}
}

#pragma endregion



// SPRITESHEET FUNCTIONS
#pragma region SPRITESHEET

SpriteSheet_Old Init_SpriteSheet(const char* texture_file, u32 rows, u32 cols, float duration)
{
	SpriteSheet_Old temp;
	temp.texture = AEGfxTextureLoad(texture_file);
	temp.rows = rows;
	temp.cols = cols;
	temp.uv_height = 1.f / rows;
	temp.uv_width = 1.f / cols;

	temp.current_col = 0;
	temp.current_row = 0;

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, temp.uv_height, // bottom left
		0.5f, -0.5f, 0xFFFFFFFF, temp.uv_width, temp.uv_height, // bottom right 
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top left

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, temp.uv_width, temp.uv_height, // bottom right 
		0.5f, 0.5f, 0xFFFFFFFF, temp.uv_width, 0.0f,   // top right
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // bottom left

	temp.mesh = AEGfxMeshEnd();

	temp.timer = Create_Timer(duration);

	return temp;
}

void Draw_SpriteSheet(SpriteSheet_Old spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 uv_x, f32 uv_y, f32 rot)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	// box transform
	AEMtx33 Transform = { 0 };

	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// Rotate
		AEMtx33 rotm;
		AEMtx33Rot(&rotm, rot);

		// CONCATENATE
		AEMtx33Concat(&Transform, &rotm, &scale);
		AEMtx33Concat(&Transform, &tran, &Transform);
	}
	AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
	AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
	AEGfxSetTransparency(1.0f);
	AEGfxSetTransform(Transform.m);

	AEGfxTextureSet(spritesheet.texture, uv_x, uv_y);


	AEGfxMeshDraw(spritesheet.mesh, AE_GFX_MDM_TRIANGLES);
}

void Set_SpriteSheet_current(SpriteSheet_Old* spritesheet, u32 row, u32 col_start, u32 col_end, Timer timer, f32 draw_duration)
{
	spritesheet->current_row = row;
	u32 cols = col_end - col_start;
	u32 col_add = 0;
	if (draw_duration)
	{
		col_add = static_cast<int>(timer.current * timer.duration / draw_duration * cols);
	}
	if (col_add >= cols)
	{
		col_add = cols - 1;
	}
	spritesheet->current_col = col_start + col_add;
}

void Draw_SpriteSheet_test(SpriteSheet_Old spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	// box transform
	AEMtx33 Transform = { 0 };

	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// Rotate
		AEMtx33 rotm;
		AEMtx33Rot(&rotm, rot);

		// CONCATENATE
		AEMtx33Concat(&Transform, &rotm, &scale);
		AEMtx33Concat(&Transform, &tran, &Transform);
	}
	AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
	AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
	AEGfxSetTransparency(1.0f);
	AEGfxSetTransform(Transform.m);

	AEGfxTextureSet(spritesheet.texture, spritesheet.uv_width * spritesheet.current_col, spritesheet.uv_height * spritesheet.current_row);


	AEGfxMeshDraw(spritesheet.mesh, AE_GFX_MDM_TRIANGLES);
}

void Unload_SpriteSheet(SpriteSheet_Old* spritesheet)
{
	AEGfxMeshFree(spritesheet->mesh);
	AEGfxTextureUnload(spritesheet->texture);
}
#pragma endregion



// AABB HITBOX
#pragma region AABB
aabb Init_AABB(f32 pos_x, f32 pos_y, f32 width, f32 height)
{
	aabb temp;
	temp.min.x = pos_x - width / 2.f;
	temp.min.y = pos_y - height / 2.f;
	temp.max.x = pos_x + width / 2.f;
	temp.max.y = pos_y + width / 2.f;
	return temp;
}

void Update_AABB(aabb* hitbox, f32 pos_x, f32 pos_y, f32 width, f32 height)
{
	hitbox->min.x = pos_x - width / 2.f;
	hitbox->min.y = pos_y - height / 2.f;
	hitbox->max.x = pos_x + width / 2.f;
	hitbox->max.y = pos_y + height / 2.f;
}

void Draw_AABB(f32 pos_x, f32 pos_y, aabb hitbox, Color color)
{
	DrawRect(pos_x, pos_y, hitbox.max.x - hitbox.min.x, hitbox.max.y - hitbox.min.y, color);
}

#pragma endregion



// MESHES
	// UNIT SQUARE
	// Creates 1x1 square mesh with two triangles
	// Color values are all 0 to allow adding later in draw functions
	AEGfxVertexList* UnitSquareMesh(u32 HexCode) 
	{
		AEGfxMeshStart();

		AEGfxTriAdd(
			-0.5f, -0.5f, HexCode, 0.0f, 1.0f,
			0.5f, -0.5f, HexCode, 1.0f, 1.0f,
			-0.5f, 0.5f, HexCode, 0.0f, 0.0f);

		AEGfxTriAdd(
			0.5f, -0.5f, HexCode, 1.0f, 1.0f,
			0.5f, 0.5f, HexCode, 1.0f, 0.0f,
			-0.5f, 0.5f, HexCode, 0.0f, 0.0f);

		return AEGfxMeshEnd();
	}

	// UNIT CIRCLE
	// Creates a circle mesh of radius 1 with variable number of parts
	// Color values are all 0 to allow adding later in draw functions
	AEGfxVertexList* UnitCircleMesh(int parts) 
	{
		AEGfxMeshStart();
		// make like pizza slice?
		// desmos goated visualization
		// if 12 parts, means 3 slice per quarter circle
		// means first slice must be 0.083~ * 2PI...
		// how to get 0.083 using i and parts?
		// 1/12, so i+1/parts

		for (int i = 0; i < parts; i++) 
		{
			AEGfxTriAdd(
				0.f, 0.f, 0x00000000, 0.f, 0.f,																				// origin
				cosf((float)i / parts * 2.f * PI), sinf((float)i / parts * 2.f * PI), 0x00000000, 0.f, 0.f,			// first pt
				cosf((float)(i+1) / parts * 2.f * PI), sinf((float)(i+1) / parts * 2.f * PI), 0x00000000, 0.f, 0.f		// second pt after angle of "pizza slice"
			);
		}

		return AEGfxMeshEnd();
	}

	// Initializes unit square and circle mesh, takes in how many parts to create the circle mesh with
	void Load_Mesh(int circle_parts) 
	{
		CircleMesh = UnitCircleMesh(circle_parts);
		SquareMesh = UnitSquareMesh(0x00000000);
		ImgBaseMesh = UnitSquareMesh(0xFFFFFFFF);
	}

	// Frees unit square and circle meshes
	void Unload_Mesh() 
	{
		AEGfxMeshFree(SquareMesh);
		AEGfxMeshFree(CircleMesh);
		AEGfxMeshFree(ImgBaseMesh);
	}



// RENDERS
// Draw rectangle
void DrawRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color) 
{
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// box transform
	AEMtx33 Transform = { 0 };
	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// CONCATENATE
		AEMtx33Concat(&Transform, &tran, &scale);
	}
	// Add color onto blank mesh
	AEGfxSetColorToAdd(color.R, color.G, color.B, color.A);
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// DRAW :D
	AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
}

void DrawRotatedRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color, f32 angle)
{
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// box transform
	AEMtx33 Transform = { 0 };
	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		AEMtx33 rot;
		AEMtx33Rot(&rot, angle);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// CONCATENATE
		AEMtx33Concat(&Transform, &rot, &scale);
		AEMtx33Concat(&Transform, &tran, &Transform);
	}
	// Add color onto blank mesh
	AEGfxSetColorToAdd(color.R, color.G, color.B, color.A);
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// DRAW :D
	AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
}

int BossPlayerDirection(AEVec2 boss, AEVec2 player)
{
	if (boss.x > player.x)
	{
		return 1;
	}
	else if (boss.x < player.x)
	{
		return 0;
	}
	else
	{
		return 0; 
	}
}

void DrawTextBox(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 size, s8 font, Color color, const char* string)
{
	// box transform
	AEMtx33 Transform = { 0 };
	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// CONCATENATE
		AEMtx33Concat(&Transform, &tran, &scale);
	}
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// Draw Text

	AEGfxGetPrintSize(font, string, 1.f, &width, &height);
	AEGfxPrint(font, string, pos_x, pos_y, size, color.R, color.G, color.B, color.A);
}

void DrawTextAdvanced(f32 pos_x, f32 pos_y, s8 font, Color color, const char* string, bool WorldToScreen, f32 size) 
{
	f32 NormX{}, NormY{};								// Converted world to screen pos (-800, 800) to (-1, 1) for AEGfxPrint()
	f32 w, h;											// God bless the snippets
	AEGfxGetPrintSize(font, string, 1.f, &w, &h);		// Find width and height of text to recenter
	{

		if (WorldToScreen) {							// Can specify in param if text should be on screen or world
			NormX = World_To_Screen_x(pos_x) * 2.f * (1.f / f32(AEGfxGetWindowWidth()));
			NormY = World_To_Screen_y(pos_y) * 2.f * (1.f / f32(AEGfxGetWindowHeight()));
		}
		else {
			NormX = pos_x * (1.f / f32(AEGfxGetWindowWidth()));
			NormY = pos_y * (1.f / f32(AEGfxGetWindowHeight()));
		}
	}
	// Draw Text
	AEGfxPrint(font, string, (-w * size) / 2 + NormX, (-h * size) / 2 + NormY, size, color.R, color.G, color.B, color.A);
}


// Draw circle
void DrawCircle(f32 pos_x, f32 pos_y, f32 radius, Color color) 
{
	// circle transform
	AEMtx33 Transform = { 0 };

	{
		// Scale to radius provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, radius, radius);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// CONCATENATE
		AEMtx33Concat(&Transform, &tran, &scale);
	}
	// Render mode
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// Add color onto blank mesh
	AEGfxSetColorToAdd(color.R, color.G, color.B, color.A);
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// DRAW :D
	AEGfxMeshDraw(CircleMesh, AE_GFX_MDM_TRIANGLES);
}

void DrawCircleAdvanced(f32 pos_x, f32 pos_y, f32 radius_x, f32 radius_y, Color color)
{
	// circle transform
	AEMtx33 Transform = { 0 };

	{
		// Scale to radius provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, radius_x, radius_y);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// CONCATENATE
		AEMtx33Concat(&Transform, &tran, &scale);
	}
	// Render mode
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// Add color onto blank mesh
	AEGfxSetColorToAdd(color.R, color.G, color.B, color.A);
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// DRAW :D
	AEGfxMeshDraw(CircleMesh, AE_GFX_MDM_TRIANGLES);
}

// Draw Image
void DrawImage(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot, AEGfxTexture* image) 
{

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	// box transform
	AEMtx33 Transform = { 0 };

	{
		// Scale to width and height provided
		AEMtx33 scale;
		AEMtx33Scale(&scale, width, height);

		// Translate to position coordinates provided
		AEMtx33 tran;
		AEMtx33Trans(&tran, pos_x, pos_y);

		// Rotate
		AEMtx33 rotm;
		AEMtx33Rot(&rotm, rot);

		// CONCATENATE
		AEMtx33Concat(&Transform, &rotm, &scale);
		AEMtx33Concat(&Transform, &tran, &Transform);
	}

	AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
	AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
	AEGfxSetTransparency(1.0f);
	AEGfxSetTransform(Transform.m);

	AEGfxTextureSet(image, 0, 0);

	AEGfxMeshDraw(ImgBaseMesh, AE_GFX_MDM_TRIANGLES);
}

void DrawImageAttachedToRect(f32 rectPosX, f32 rectPosY, f32 rectWidth, f32 rectHeight, AEGfxTexture* image)
{
	// Calculate the center of the rectangle (use the rectangle's center as the image's position)
	f32 image_pos_x = rectPosX + rectWidth / 2.0f;
	f32 image_pos_y = rectPosY + rectHeight / 2.0f;

	// Draw the image at the center of the rectangle
	DrawImage(image_pos_x, image_pos_y, rectWidth, rectHeight, 0.0f, image); // Assuming no rotation
}

// Function to draw a boundary box (hitbox) around an object
void DrawBoundary(f32 pos_x, f32 pos_y, f32 width, f32 height)
{
	Color boundaryColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red color for the boundary

	// Draw the boundary as a rectangle
	DrawRect(pos_x, pos_y, width, height, boundaryColor);
}

void DrawObjectWithBoundary(f32 pos_x, f32 pos_y, f32 width, f32 height, AEGfxTexture* image)
{
	// Draw the image first
	DrawImage(pos_x, pos_y, width, height, 0.0f, image);

	// Draw the boundary around the image
	DrawBoundary(pos_x, pos_y, width, height);
}

// Draw boundary considering the object’s center
void DrawObjectWithBoundaryCentered(f32 pos_x, f32 pos_y, f32 width, f32 height, AEGfxTexture* image)
{
	// Draw the image
	DrawImage(pos_x, pos_y, width, height, 0.0f, image);

	// Adjust the boundary to account for the center origin
	f32 boundary_x = pos_x - (width / 2);
	f32 boundary_y = pos_y - (height / 2);

	// Draw the boundary
	DrawBoundary(boundary_x, boundary_y, width, height);
}


// Check if two circles are intersecting
bool AreCirclesIntersecting(f32 pos_x1, f32 pos_y1, f32 radius1, f32 pos_x2, f32 pos_y2, f32 radius2) 
{
	AEVec2 crc1 = { pos_x1, pos_y1 };

	AEVec2 crc2 = { pos_x2, pos_y2 };

	return AETestCircleToCircle(&crc1, radius1, &crc2, radius2);
}

// Check if two rectangles are intersecting
bool AreRectanglesIntersecting(f32 rect1_x, f32 rect1_y, f32 rect1_width, f32 rect1_height,
	f32 rect2_x, f32 rect2_y, f32 rect2_width, f32 rect2_height)
{
	// Get the edges of the first rectangle
	f32 rect1_left = rect1_x;
	f32 rect1_right = rect1_x + rect1_width;
	f32 rect1_top = rect1_y;
	f32 rect1_bottom = rect1_y + rect1_height;

	// Get the edges of the second rectangle
	f32 rect2_left = rect2_x;
	f32 rect2_right = rect2_x + rect2_width;
	f32 rect2_top = rect2_y;
	f32 rect2_bottom = rect2_y + rect2_height;

	// Check if the rectangles overlap
	bool isIntersecting = !(rect1_right < rect2_left ||    // rect1 is to the left of rect2
		rect1_left > rect2_right ||    // rect1 is to the right of rect2
		rect1_bottom < rect2_top ||    // rect1 is above rect2
		rect1_top > rect2_bottom);     // rect1 is below rect2

	return isIntersecting;
}


float CalculateDistance(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}






// Checks if an area is clicked
int IsAreaClicked(f32 pos_x, f32 pos_y, f32 width, f32 height, s32 click_x, s32 click_y)
{
	// if mouse pointer is inside the area
	if (click_x > (pos_x - width / 2) && click_x < (pos_x + width / 2) &&
		click_y > (pos_y - height / 2) && click_y < (pos_y + height / 2)) 
	{
		return 1;
	}
	return 0;
}