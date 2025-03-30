//---------------------------------------------------------
// file:	SpriteSheet.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Spritesheet class declaration
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once

#include "utils.h"
#include "crtdbg.h"

// SpriteSheet class
class SpriteSheet {
public:
	// ----------------------------------------------------------------------------
	// Constructors
	// -	Default constructor initializes empty values and pointers as nullptr
	// -	Parameterized constructor loads the texture, and creates a mesh based on
	//		the total cols and rows specified
	// ----------------------------------------------------------------------------
	SpriteSheet();
	SpriteSheet(const char* texture_file, u32 cols, u32 rows);
	// ----------------------------------------------------------------------------
	// Destructor
	// -	Does nothing...?
	// ----------------------------------------------------------------------------
	~SpriteSheet() {}

	// ----------------------------------------------------------------------------
	// Unloader
	// -	Frees mesh and unloads texture
	// ----------------------------------------------------------------------------
	void Unload()
	{
		AEGfxMeshFree(mesh);
		AEGfxTextureUnload(texture);
	}

	// ----------------------------------------------------------------------------
	// Sets the current row
	// ----------------------------------------------------------------------------
	void Set_Row(u32 row);

	// ----------------------------------------------------------------------------
	// Sets the current column
	// ----------------------------------------------------------------------------
	void Set_Col(u32 col);

	// ----------------------------------------------------------------------------
	// Sets the current column from a row section, based on a timer value from 0 to 1
	// ----------------------------------------------------------------------------
	void Set_Col(u32 left, u32 right, f32 currtimer);

	// ----------------------------------------------------------------------------
	// Loops through a row section over a specified duration repeatedly
	// ----------------------------------------------------------------------------
	void Loop(u32 left, u32 right, f32 duration);

	// ----------------------------------------------------------------------------
	// Returns row index
	// ----------------------------------------------------------------------------
	u32 Get_Row();

	// ----------------------------------------------------------------------------
	// Returns col index
	// ----------------------------------------------------------------------------
	u32 Get_Col();

	// ----------------------------------------------------------------------------
	// Draws the sprite at specified position and size
	// ----------------------------------------------------------------------------
	void Draw(f32 pos_x, f32 pos_y, f32 width, f32 height, bool flip = false, f32 rot = 0.f);

private:

	u32 total_cols, total_rows;		// size of spritesheet
	u32 curr_col, curr_row;			// current sprite index
	f32 uv_height, uv_width;		// UV height and width of a sprite

	AEGfxTexture* texture;			// image texture
	AEGfxVertexList* mesh;			// unit square mesh with UV values assigned

	f32 looper;						// value to loop sprite if required
};
