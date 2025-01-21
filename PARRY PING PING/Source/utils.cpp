//---------------------------------------------------------
// file:	utils.cpp
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Collection of utility functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"



// Globals
// -------------------------------------------------
#pragma region GLOBALS
AEGfxVertexList* CircleMesh;
AEGfxVertexList* SquareMesh;
AEGfxVertexList* ImgBaseMesh;

f32 dt;
#pragma endregion

// COLOR STRUCT
	Color SetColor(u32 HexCode) 
	{
		Color color;									// temp struct
		color.A = ((HexCode >> 24) & 0xFF) / 255.0f;	// Alpha
		color.R = ((HexCode >> 16) & 0xFF) / 255.0f;	// Red
		color.G = ((HexCode >> 8) & 0xFF) / 255.0f;		// Green
		color.B = (HexCode & 0xFF) / 255.0f;			// Blue

		return color;
	}



// TIMER UPDATE
	bool Update_Timer(Timer* timer, float seconds) 
	{
		if (timer->current < timer->end) 
		{
			timer->current += dt/seconds;
			return true;
		}
		return false;
	}

	void Loop_Timer(Timer* timer, float seconds) 
	{
		std::cout << "Timer: " << timer->current << '\n';
		if (timer->current < timer->end) 
		{
			timer->current += dt / seconds;
		}
		else
		{
			timer->current = 0;
		}
	}


// SPRITESHEET INIT
	SpriteSheet Init_SpriteSheet(const char* texture_file, u32 rows, u32 cols, float duration) 
	{
		SpriteSheet temp;
		temp.texture = AEGfxTextureLoad(texture_file);
		temp.rows = rows;
		temp.cols = cols;
		temp.uv_height = 1.f / rows;
		temp.uv_width = 1.f / cols;
		std::cout << temp.uv_height << ' ' << temp.uv_width << '\n';

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

		temp.timer = { 1.f,1.f };
		temp.duration = duration;

		return temp;
	}

	void Draw_SpriteSheet(SpriteSheet spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 uv_x, f32 uv_y, f32 rot) 
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
	void InitMesh(int circle_parts) 
	{
		CircleMesh = UnitCircleMesh(circle_parts);
		SquareMesh = UnitSquareMesh(0x00000000);
		ImgBaseMesh = UnitSquareMesh(0xFFFFFFFF);
	}

	// Frees unit square and circle meshes
	void FreeMesh() 
	{
		AEGfxMeshFree(SquareMesh);
		AEGfxMeshFree(CircleMesh);
		AEGfxMeshFree(ImgBaseMesh);
	}



// RENDERS
// Draw rectangle
void DrawRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color) 
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
	// Add color onto blank mesh
	AEGfxSetColorToAdd(color.R, color.G, color.B, color.A);
	// Set transformation matrix
	AEGfxSetTransform(Transform.m);
	// DRAW :D
	AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);

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

// Check if two circles are intersecting
bool AreCirclesIntersecting(f32 pos_x1, f32 pos_y1, f32 radius1, f32 pos_x2, f32 pos_y2, f32 radius2) 
{
	AEVec2 crc1 = { pos_x1, pos_y1 };

	AEVec2 crc2 = { pos_x2, pos_y2 };

	return AETestCircleToCircle(&crc1, radius1, &crc2, radius2);
}

// draw full hp bar
// pos_x1, pos_y1 - position of top left corner
// pos_x2, pos_y2 - position of bottom right corner
// hp - current health
// maxhp - max health
void DrawHPBar(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color) 
{
	// assign total width, height, and width of health bar
	float width, height, hpwidth;
	//width and height of total bar
	width = pos_x2 - pos_x1;
	height = pos_y2 - pos_y1;
	//width of current hp
	hpwidth = width * hp / maxhp;
	// draw the rectangle
	DrawRect( pos_x1+(hpwidth/2.f), pos_y1+height/2.f, hpwidth, height, color);
}

// draw segmented hp bar
// pos_x1, pos_y1 - position of top left corner
// pos_x2, pos_y2 - position of bottom right corner
// hp - current health
// maxhp - max health
void DrawHPBar2(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color) 
{
	//array of hp in 10% chunks
	float hpbar[10] = { 0.f, maxhp / 10.f, 2.f * maxhp / 10.f, 3.f * maxhp / 10.f, 4.f * maxhp / 10.f, 5.f * maxhp / 10.f, 6.f * maxhp / 10.f, 7.f * maxhp / 10.f, 8.f * maxhp / 10.f , 9.f * maxhp / 10.f }; //array 10% blocks
	//size of full bar
	float width = pos_x2 - pos_x1;
	float height = pos_y2 - pos_y1;
	//segment width and interval
	float segmentwidth = width / 10.f;
	float segmentinterval = 15.f;
	//for loop to check hp 10% chunks
	for (int i = 0; i < 10; ++i) 
	{
		if (hp > (hpbar[i]) + maxhp / 20.f) //if hp>10% interval, draw 10% chunk
		{ 
			//DrawRect(Mesh, (pos_x1 + (segmentwidth / 2.f)) + (i * (segmentwidth)), pos_y1 + height / 2.f, segmentwidth - segmentinterval, height, color);

			DrawRect( (pos_x1 + ((segmentwidth-segmentinterval) / 2.f)) + (i * (segmentwidth+segmentinterval/10.f)), pos_y1 + height / 2.f, segmentwidth - segmentinterval, height, color);
		}
	}
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