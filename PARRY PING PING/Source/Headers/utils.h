//---------------------------------------------------------
// file:	utils.h
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Declaration of utility structs and functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once

#include <iostream>
#include <iomanip>
#include <AEEngine.h>
#include "Input.h"

extern f32 dt;


// Color
// -------------------------------------------------
// 
// Color struct holding RGBA values of type float
typedef struct Color 
{
	float R, G, B, A;
} Color;

// Color set function takes hex code and returns a color struct with assigned RGBA values
Color SetColor(u32 AARRGGBB);


// Timer
// -------------------------------------------------
//
// Timer struct holding current and end value
typedef struct Timer 
{
	float current;
	float end;
	float duration;
} Timer;




Timer Create_Timer(float duration);

bool Update_Timer(Timer* timer);

void Loop_Timer(Timer* timer);


// Spritesheet
// -------------------------------------------------
//
// Anim struct holding stuff
typedef struct SpriteSheet 
{
	AEGfxTexture* texture;

	u32 rows;
	u32 cols;
	f32 uv_height;
	f32 uv_width;

	u32 current_col;
	u32 current_row;

	AEGfxVertexList* mesh;

	Timer timer;
	float duration;
} SpriteSheet;

SpriteSheet Init_SpriteSheet(const char* texture_file, u32 rows, u32 cols, float duration);

void Draw_SpriteSheet(SpriteSheet spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 uv_x, f32 uv_y, f32 rot);




// Mesh
// -------------------------------------------------
//

void Load_Mesh(int circle_parts);

void Unload_Mesh();


// Draws a rectangle
// param UnitSquareMesh - unit square mesh of 1x1
// param pos_x - x coordinate of center of rectangle
// param pos_y - y coordinate of center of rectangle
// param width - width of rectangle
// param height - height of rectangle
// param color - color of rectangle
void DrawRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color);

// Draws a text box
// param UnitSquareMesh - unit square mesh of 1x1
// param pos_x - x coordinate of center of text box
// param pos_y - y coordinate of center of text box
// param width - width of text box
// param height - height of text box
// param size - size of font
// param font - set font type
// param color - set text color
// param string - string to print
void DrawTextBox(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 size, s8 font, Color color, const char* string);

// Draws a circle
// param UnitCircleMesh - unit circle mesh of radius 1
// param pos_x - x coordinate of center of circle
// param pos_y - y coordinate of center of circle
// param radius - radius of circle
// param color - color of circle
void DrawCircle(f32 pos_x, f32 pos_y, f32 radius, Color color);

void DrawImage(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot, AEGfxTexture* image);

// Checks if two circles are intersecting
// param pos_x1 - x coordinate of center of circle1
// param pos_y1 - y coordinate of center of circle1
// param radius1 - radius of circle1
// param pos_x2 - x coordinate of center of circle2
// param pos_y2 - y coordinate of center of circle2
// param radius2 - radius of circle2
// return - true if circles intersect
bool AreCirclesIntersecting(f32 pos_x1, f32 pos_y1, f32 radius1, f32 pos_x2, f32 pos_y2, f32 radius2);

// Draws a HP bar
// param UnitSquareMesh - unit square mesh of 1x1
// param pos_x1 - left x coordinate of HP bar
// param pos_y1 - top y coordinate of HP bar
// param pos_x2 - right x coordinate of HP bar
// param pos_y2 - bottom y coordinate of HP bar
// param hp - current health
// param maxhp - max health
// param color - color of HP bar
void DrawHPBar(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color);

// Draws a segmented HP bar divided into 10 sections
// param UnitSquareMesh - unit square mesh of 1x1
// param pos_x1 - left x coordinate of HP bar
// param pos_y1 - top y coordinate of HP bar
// param pos_x2 - right x coordinate of HP bar
// param pos_y2 - bottom y coordinate of HP bar
// param hp - current health
// param maxhp - max health
// param color - color of HP bar
void DrawHPBar2(float pos_x1, float pos_y1, float pos_x2, float pos_y2, float hp, float maxhp, Color color);

// Checks if an area is clicked
// param pos_x - x coordinate of center of area
// param pos_y - y coordinate of center of area
// param width - width of area
// param height - height of area
// param click_x - x coordinate of mouse pointer
// param click_y - y coordinate of mouse pointer
int IsAreaClicked(f32 pos_x, f32 pos_y, f32 width, f32 height, s32 click_x, s32 click_y);