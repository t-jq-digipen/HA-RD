//---------------------------------------------------------
// File:	utils.h
// Project: Heavy Assault : Rust & Dust
// Authors:	
//			Tan Jun Quan ------------------------------ 50%
//			- Dt manager
//			- Timer system
//			- Color struct
//			- Basic spritesheet system
//			- Base meshes
//			- Basic shape drawing
//			- Basic image drawing
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- ??%
// 
//			Muhammad Raziq bin Mohd Rafig ------------- ??%
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- ??%
// 
//			Manfred Sim ------------------------------- ??%
//			- File IO system
// 
// Brief:	Collection of utility functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <AEEngine.h>
#include "Input.h"
#include <vector>

extern f32 dt;

void Get_DT();

// Color
// -------------------------------------------------

// ----------------------------------------------------------------------------
// Color struct holding RGBA values of type float
// ----------------------------------------------------------------------------
typedef struct Color 
{
	float R, G, B, A;
} Color;

// ----------------------------------------------------------------------------
// Color set function takes hex code and returns a color struct with assigned RGBA values
// ----------------------------------------------------------------------------
Color SetColor(u32 AARRGGBB);


// Timer
// -------------------------------------------------

// ----------------------------------------------------------------------------
// Timer struct holding current and end value
// ----------------------------------------------------------------------------
typedef struct Timer 
{
	float current;
	float end;
	float duration;
} Timer;

extern Timer bullet_time;

Timer Create_Timer(float duration);

bool Update_Timer(Timer* timer);

void Loop_Timer(Timer* timer);


// Spritesheet (OUTDATED)
// -------------------------------------------------
//
// Anim struct holding stuff
typedef struct SpriteSheet_Old 
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
} SpriteSheet_Old;

SpriteSheet_Old Init_SpriteSheet(const char* texture_file, u32 rows, u32 cols, float duration);

void Draw_SpriteSheet(SpriteSheet_Old spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 uv_x, f32 uv_y, f32 rot);

void Set_SpriteSheet_current(SpriteSheet_Old* spritesheet, u32 row, u32 col_start, u32 col_end, Timer timer, f32 draw_duration);

void Draw_SpriteSheet_test(SpriteSheet_Old spritesheet, f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot);

void Unload_SpriteSheet(SpriteSheet_Old* spritesheet);


// AABB hitbox
// -------------------------------------------------

// ----------------------------------------------------------------------------
// AABB struct holding min and max values
// ----------------------------------------------------------------------------
typedef struct aabb
{
	AEVec2 min;
	AEVec2 max;
} aabb;

// ----------------------------------------------------------------------------
// Initialize AABB struct
// ----------------------------------------------------------------------------
aabb Init_AABB(f32 pos_x, f32 pos_y, f32 width, f32 height);

// ----------------------------------------------------------------------------
// Update AABB hitbox
// ----------------------------------------------------------------------------
void Update_AABB(aabb* hitbox, f32 pos_x, f32 pos_y, f32 width, f32 height);

// ----------------------------------------------------------------------------
// Draw the AABB hitbox (for debugging)
// ----------------------------------------------------------------------------
void Draw_AABB(f32 pos_x, f32 pos_y, aabb hitbox, Color color);


// Mesh
// -------------------------------------------------
//

// ----------------------------------------------------------------------------
// Load the base meshes, unit square and circle
// ----------------------------------------------------------------------------
void Load_Mesh(int circle_parts);

// ----------------------------------------------------------------------------
// Unloads the base meshes
// ----------------------------------------------------------------------------
void Unload_Mesh();


// ----------------------------------------------------------------------------
// Draws a rectangle
// param pos_x - x coordinate of center of rectangle
// param pos_y - y coordinate of center of rectangle
// param width - width of rectangle
// param height - height of rectangle
// param color - color of rectangle
// ----------------------------------------------------------------------------
void DrawRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color);

// ----------------------------------------------------------------------------
// Draws a rotated rectangle
// param pos_x - x coordinate of center of rectangle
// param pos_y - y coordinate of center of rectangle
// param width - width of rectangle
// param height - height of rectangle
// param color - color of rectangle
// param angle - angle of rotation
// ----------------------------------------------------------------------------
void DrawRotatedRect(f32 pos_x, f32 pos_y, f32 width, f32 height, Color color, f32 angle);

// ----------------------------------------------------------------------------
// Determines if playeris to the left or right of the boss (1 if left, 0 if right)
// param boss - position of boss
// param player - position of player
// ----------------------------------------------------------------------------
int BossPlayerDirection(AEVec2 boss, AEVec2 player);

// ----------------------------------------------------------------------------
// Draws a text box
// param pos_x - x coordinate of center of text box
// param pos_y - y coordinate of center of text box
// param width - width of text box
// param height - height of text box
// param size - size of font
// param font - set font type
// param color - set text color
// param string - string to print
// ----------------------------------------------------------------------------
void DrawTextBox(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 size, s8 font, Color color, const char* string);

// ----------------------------------------------------------------------------
// Takes in world coords and draws text at desired position (not scaled to 
// param pos_x - x coordinate of center of text box
// param pos_y - y coordinate of center of text box
// param width - width of text box
// param height - height of text box
// param font - set font type
// param color - set text color
// param string - string to print
// param WorldToScreen - true or false, to print text on screen directly or as world object (defaults to on screen)
// param size - scaling of text (defaults to 1)
// ----------------------------------------------------------------------------
void DrawTextAdvanced(f32 world_pos_x, f32 world_pos_y, s8 font, Color color, const char* string, bool WorldToScreen = false, f32 size = 1.f);

// ----------------------------------------------------------------------------
// Draws a circle
// param pos_x - x coordinate of center of circle
// param pos_y - y coordinate of center of circle
// param radius - radius of circle
// param color - color of circle
// ----------------------------------------------------------------------------
void DrawCircle(f32 pos_x, f32 pos_y, f32 radius, Color color);

void DrawCircleAdvanced(f32 pos_x, f32 pos_y, f32 radius_x, f32 radius_y, Color color);

void DrawImageAttachedToRect(f32 rectPosX, f32 rectPosY, f32 rectWidth, f32 rectHeight, AEGfxTexture* image);

// ----------------------------------------------------------------------------
// Draws an image
// param pos_x - x coordinate of center of image
// param pos_y - y coordinate of center of image
// param width - width of image
// param height - height of image
// param rot - rotation of image
// ----------------------------------------------------------------------------
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

// Struct to store coords and dimensions of props to place down in environments
// Pos - x & y coordinates
// Width & Height - dimensions (can just use width in place of circle rad)
typedef struct EnvStruct {
	AEVec2 Pos;
	f32 Width;
	f32 Height;
	//aabb Hitbox;		//for future implementation if want to add coll/ destructable objects etc
}EnvStruct;

/*!
 * \brief Calculates the distance between two points (x1, y1) and (x2, y2).
 *
 * \param x1 The x-coordinate of the first point.
 * \param y1 The y-coordinate of the first point.
 * \param x2 The x-coordinate of the second point.
 * \param y2 The y-coordinate of the second point.
 * \return The distance between the two points.
 */
float CalculateDistance(float x1, float y1, float x2, float y2);

/*!
 * \brief Checks for a collision using the Separating Axis Theorem (SAT) for two convex polygons.
 *
 * \param corners1 Array of corners (vertices) of the first polygon.
 * \param corners2 Array of corners (vertices) of the second polygon.
 * \return True if a collision is detected, false otherwise.
 */
//bool CheckCollisionSAT(const AEVec2* corners1, const AEVec2* corners2);

/*!
 * \brief Rotates a point around a given center by a specified angle.
 *
 * \param point The point to be rotated.
 * \param center The center around which the point will be rotated.
 * \param angle The angle in radians by which to rotate the point.
 * \return The rotated point.
 */
//AEVec2 RotatePoint(AEVec2 point, AEVec2 center, float angle);

/*!
 * \brief Projects a point onto a specified axis.
 *
 * \param point The point to project.
 * \param axis The axis onto which the point will be projected.
 * \return The projection of the point onto the axis.
 */
//float ProjectOntoAxis(const AEVec2& point, const AEVec2& axis);

/*!
 * \brief Checks if two rectangles are intersecting.
 *
 * \param rect1_x The x-coordinate of the first rectangle's top-left corner.
 * \param rect1_y The y-coordinate of the first rectangle's top-left corner.
 * \param rect1_width The width of the first rectangle.
 * \param rect1_height The height of the first rectangle.
 * \param rect2_x The x-coordinate of the second rectangle's top-left corner.
 * \param rect2_y The y-coordinate of the second rectangle's top-left corner.
 * \param rect2_width The width of the second rectangle.
 * \param rect2_height The height of the second rectangle.
 * \return True if the rectangles are intersecting, false otherwise.
 */
bool AreRectanglesIntersecting(f32 rect1_x, f32 rect1_y, f32 rect1_width, f32 rect1_height, f32 rect2_x, f32 rect2_y, f32 rect2_width, f32 rect2_height);

void DrawImageFading(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 rot, AEGfxTexture* image, f32 alpha);

f32 Lerp(f32 start, f32 end, f32 t);

void DrawBackgroundFade(f32 pos_x, f32 pos_y, f32 width, f32 height, f32 fadeFactor);

// ----------------------------------------------------------------------------
// Reads from text file
// param filename - name of text file
// param vector - vector to store values from text file
// ----------------------------------------------------------------------------
void ReadFromFile(const char* filename, std::vector<float>& vector);

// ----------------------------------------------------------------------------
// Reads all lines from text file
// param filename - name of text file
// param vector - vector to store lines from text file
// ----------------------------------------------------------------------------
void ReadWholeFile(const char* filename, std::vector<std::string>& vector);

// ----------------------------------------------------------------------------
// Updates a value in text file
// param filename - name of text file
// param name - label of value to be changed
// param value - new value
// ----------------------------------------------------------------------------
void WriteToFile(const char* filename, const std::string& name, const std::string& value);

// ----------------------------------------------------------------------------
// Draws cool swipe transition (From clear -> blocked out)
// param flag - flag to alter once transition finishes playing
// ----------------------------------------------------------------------------
void TransitionEnter(bool& flag);

// ----------------------------------------------------------------------------
// Draws cool swipe transition (From blocked out -> clear)
// param flag - flag to alter once transition finishes playing
// ----------------------------------------------------------------------------
void TransitionExit(bool& flag);

// ----------------------------------------------------------------------------
// Extern so can be modified in other source files.
// ----------------------------------------------------------------------------
extern f32 transition_xpos;