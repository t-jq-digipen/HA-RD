//---------------------------------------------------------
// File:	SpriteSheet.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Tan Jun Quan
//
// Brief:	Spritesheet class implementation
//			Manages loading, unloading, and rendering of a
//			spritesheet texture.
//			Allows manipulating of current sprite index and
//			supports animation looping.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "SpriteSheet.h"

SpriteSheet::SpriteSheet() : 
	total_cols{}, total_rows{}, 
	curr_col{}, curr_row{}, 
	uv_height{}, uv_width{}, 
	looper{}, 
	texture {nullptr},
	mesh {nullptr}
{}

SpriteSheet::SpriteSheet(const char* texture_file, u32 cols, u32 rows) :
	total_cols{ cols },
	total_rows{ rows },
	curr_col{ 0 },
	curr_row{ 0 },
	uv_height{ 1.f / rows },
	uv_width{ 1.f / cols },
	looper{ 0 }
{
	texture = AEGfxTextureLoad(texture_file);

	AEGfxMeshStart();										// Make the mesh based on UV size

	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, uv_height,				// bottom left
		0.5f, -0.5f, 0xFFFFFFFF, uv_width, uv_height,			// bottom right 
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);					// top left

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, uv_width, uv_height,			// bottom right 
		0.5f, 0.5f, 0xFFFFFFFF, uv_width, 0.0f,					// top right
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);					// bottom left

	mesh = AEGfxMeshEnd();									// End mesh

}

SpriteSheet::~SpriteSheet()
{ 
	// do nothing 
}

void SpriteSheet::Set_Row(u32 row)
{
	if (row < 0 || row >= total_rows)
	{
		_CrtDbgBreak();										// Outside spritesheet range!
	}
	curr_row = row;
}

void SpriteSheet::Set_Col(u32 col)
{
	if (col < 0 || col >= total_cols)
	{
		_CrtDbgBreak();										// Outside spritesheet range!
	}
	curr_col = col;
}

void SpriteSheet::Set_Col(u32 left, u32 right, f32 currtimer)
{
	if (currtimer < 0)					// If currtimer exceeds, set it to 0, something wrong!
	{
		currtimer = 0.f;
	}

	if (currtimer >= 1.f)
	{
		currtimer = 0.99f;
	}

	if (left < 0 || right >= total_cols)					// Outside spritesheet range!
	{
		_CrtDbgBreak();
	}

	u32 rows = right + 1 - left;
	this->Set_Col((u32)(rows * currtimer) + left);
}

void SpriteSheet::Loop(u32 left, u32 right, f32 duration)
{
	if (left < 0 || right >= total_cols)					// Outside spritesheet range!
	{
		_CrtDbgBreak();
	}

	u32 rows = right + 1 - left;
	looper += dt / duration;
	if (looper > 1.f)
	{
		looper = 0;
	}
	this->Set_Col((u32)(rows * looper) + left);
}

u32 SpriteSheet::Get_Row()
{
	return curr_row;
}

u32 SpriteSheet::Get_Col()
{
	return curr_col;
}

void SpriteSheet::Draw(f32 pos_x, f32 pos_y, f32 width, f32 height, bool flip, f32 rot)
{
	if (flip)
	{
		width = -width;
	}
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

	AEGfxTextureSet(texture, uv_width * curr_col, uv_height * curr_row);

	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}