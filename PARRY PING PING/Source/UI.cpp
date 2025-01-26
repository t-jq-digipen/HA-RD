//---------------------------------------------------------
// file:	UI.cpp
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	UI elements
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "UI.h"

float UI_hp, UI_maxhp;
int UI_ammo, UI_maxammo;
Color empty, full, health;

void Draw_UI()
{
	DrawHPBar(-700.f, -350.f,-300.f,-335.f,UI_hp,UI_maxhp,health);

	for (int ammo_index = 0; ammo_index < UI_maxammo; ammo_index++)
	{
		float posx = -675.f + ammo_index * 50.f;
		float posy = -400.f;
		if (ammo_index < UI_ammo)
		{
			DrawRect(posx, posy, 15.f, 50.f, full);
		}
		DrawRect(posx, posy, 25.f, 60.f, empty);
	}
}

void Update_UI(int ammo, float hp)
{
	UI_hp = hp;
	UI_ammo = ammo;
}

// will neaten when more player parameters are finalized
void Init_UI(int ammo, int maxammo, float hp, float maxhp)
{
	UI_hp = hp;
	UI_maxhp = maxhp;
	UI_ammo = ammo;
	UI_maxammo = maxammo;
	empty = SetColor(0x70B0FFD5);
	full = SetColor(0xFF24FF8B);
	health = SetColor(0xFFFF6961);
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
	DrawRect(pos_x1 + (hpwidth / 2.f), pos_y1 + height / 2.f, hpwidth, height, color);
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

			DrawRect((pos_x1 + ((segmentwidth - segmentinterval) / 2.f)) + (i * (segmentwidth + segmentinterval / 10.f)), pos_y1 + height / 2.f, segmentwidth - segmentinterval, height, color);
		}
	}
}