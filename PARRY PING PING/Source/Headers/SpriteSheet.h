#pragma once

#include "utils.h"

class SpriteSheet {
public:
	SpriteSheet();
	SpriteSheet(const char* texture_file, u32 cols, u32 rows);
	~SpriteSheet() {}

	void Unload()
	{
		AEGfxMeshFree(mesh);
		AEGfxTextureUnload(texture);
	}

	void Set_Row(u32 row);

	void Set_Col(u32 col);

	void Set_Col(u32 left, u32 right, f32 currtimer);

	void Loop(u32 left, u32 right, f32 duration);

	u32 Get_Row();

	u32 Get_Col();

	void Draw(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot = 0.f);

private:

	u32 total_cols, total_rows;
	u32 curr_col, curr_row;
	f32 uv_height, uv_width;

	AEGfxTexture* texture;
	AEGfxVertexList* mesh;

	f32 looper;
};
