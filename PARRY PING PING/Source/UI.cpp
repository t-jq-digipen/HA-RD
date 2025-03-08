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
#include "Boss1.h"
#include "GameStateManager.h"

int levelState = 0;

s8 UI_font;
Color UI_red, UI_green, UI_blue, UI_black, UI_white, UI_hover_red;
s32 UI_mouseX, UI_mouseY;

float UI_playerhp, UI_playermaxhp;
int UI_ammo, UI_maxammo;
float UI_parryCD;
Color empty_ammo, full_ammo, health, parry_charging, parry_ready;

float UI_bosshp, UI_bossmaxhp;

Color play_bg, pause_bg, pause_screen;
Color boss_hp_red, boss_hp_yellow, boss_hp_green, boss_hp_damage;

char level_timer[16];

// will neaten when more player parameters are finalized
void Init_UI()
{
	// Colors
	UI_red = SetColor(0xFFFF6961);
	UI_green = SetColor(0xFF88B984);
	UI_blue = SetColor(0xFF00008B);
	UI_black = SetColor(0xFF000000);
	UI_white = SetColor(0xFFFFFFFF);
	UI_hover_red = SetColor(0xFFFFA5A1);

	// Font
	UI_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	// Level state UI
	play_bg = SetColor(0xFF666666);
	pause_bg = SetColor(0x4D000000);
	pause_screen = SetColor(0xFFFFFFFF);

	// Boss UI
	boss_hp_green = SetColor(0xFF00FF00);
	boss_hp_yellow = SetColor(0xFFFFEF00);
	boss_hp_red = SetColor(0xFFFF0000);
	boss_hp_damage = SetColor(0xFF800000);
	boss1.hp_color = boss_hp_green;

	// Starting level state
	levelState = PLAYING;
}

void Init_Player_UI(int ammo, int maxammo, float hp, float maxhp)
{
	// Player UI
	UI_playerhp = hp;
	UI_playermaxhp = maxhp;
	UI_ammo = ammo;
	UI_maxammo = maxammo;
	empty_ammo = SetColor(0x70B0FFD5);
	full_ammo = SetColor(0xFF24FF8B);
	health = SetColor(0xFFFF6961);
	parry_charging = SetColor(0xFF00255F);
	parry_ready = SetColor(0xFF004EFF);
}

void Update_Player_UI(int ammo, float hp, float parry_timer)
{
	// Update player hp/ammo
	UI_playerhp = hp;
	UI_ammo = ammo;
	UI_parryCD = parry_timer;
}

void Draw_Player_UI()
{
	Color parry = (UI_parryCD == 1.f) ? parry_ready : parry_charging;

	DrawHPBar(-700.f, -300.f, -500.f, -290.f, UI_parryCD, 1.f, parry);


	DrawHPBar(-700.f, -350.f, -300.f, -335.f, UI_playerhp, UI_playermaxhp, health);

	for (int ammo_index = 0; ammo_index < UI_maxammo; ammo_index++)
	{
		float posx = -675.f + ammo_index * 50.f;
		float posy = -400.f;
		if (ammo_index < UI_ammo)
		{
			DrawRect(posx, posy, 15.f, 50.f, full_ammo);
		}
		DrawRect(posx, posy, 25.f, 60.f, empty_ammo);
	}
}

void Init_Boss_UI(float hp)
{
	UI_bosshp = UI_bossmaxhp = hp;
}

void Update_Boss_UI(float hp)
{
	UI_bosshp = hp;
}

void Draw_Boss_HPbar()
{
	DrawRect(0.f, 400.f, 1200.f, 50.f, boss_hp_damage);
	Color hpcolor = boss_hp_green;
	if (UI_bosshp < 0.6f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_yellow;
	}
	if (UI_bosshp < 0.3f * UI_bossmaxhp)
	{
		hpcolor = boss_hp_red;
	}
	DrawHPBar(-600.f, 425.f, 600.f, 375.f, UI_bosshp, UI_bossmaxhp, hpcolor);
}

void Draw_UI(int DrawBossHP )
{
	Draw_Player_UI();
	if (DrawBossHP) { Draw_Boss_HPbar(); }
}




void Draw_Boss1HP()
{
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// boss health logic
	if (boss1.current_hp >= 30.f && boss1.current_hp < 60.f)
	{
		boss1.phase = PHASE_2;
		boss1.hp_color = boss_hp_yellow;
	}
	if (boss1.current_hp >= 0.f && boss1.current_hp < 30.f)
	{
		boss1.phase = PHASE_3;
		boss1.hp_color = boss_hp_red;
	}
	if (boss1.current_hp <= 0.f)
	{
		boss1.phase = DEFEAT;
		boss1.current_hp = 0.f;
	}

	// Draw boss health bar
	DrawRect(0.f, 400.f, 1200.f, 50.f, boss1.hp_color);
	DrawRect((boss1.current_hp / boss1.max_hp) * 1200.f, 400.f, 1200.f, 50.f, boss_hp_damage);
	DrawRect(800.f, 400.f, 400.f, 50.f, play_bg);
}

void Draw_PauseMenu() 
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);
	DrawRect(0.f, 0.f, 1200.f, 600.f, pause_screen);
	DrawTextBox(-0.3f, 0.25f, 800.f, 200.f, 2.f, UI_font, UI_black, "PAUSED");

	// play button (click to go to Level1 state)
	if (IsAreaClicked(500.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Play\n";
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(-0.5f, -0.1f, 300.f, 200.f, 1.f, UI_font, UI_white, "PLAY");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			levelState = PLAYING;
		}
	}
	else
	{
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_green);
		DrawTextBox(-0.5f, -0.1f, 300.f, 200.f, 1.f, UI_font, UI_black, "PLAY");
	}

	// exit button (click to go to quit state)
	if (IsAreaClicked(1100.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Exit\n";
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(0.25f, -0.1f, 300.f, 200.f, 1.f, UI_font, UI_white, "QUIT");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_QUIT;
		}
	}
	else
	{
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_red);
		DrawTextBox(0.25f, -0.1f, 300.f, 200.f, 1.f, UI_font, UI_black, "QUIT");
	}
}

void Draw_ClearMenu()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);
	DrawRect(0.f, 0.f, 1200.f, 600.f, pause_screen);
	DrawTextBox(-0.5f, 0.25f, 800.f, 200.f, 2.f, UI_font, UI_black, "YOU WIN!!!");

	// play button (click to go to Level1 state)
	if (IsAreaClicked(500.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Play\n";
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(-0.55f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_white, "REPLAY");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_RESTART;
		}
	}
	else
	{
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_green);
		DrawTextBox(-0.55f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_black, "REPLAY");
	}

	// exit button (click to go to quit state)
	if (IsAreaClicked(1100.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Exit\n";
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(0.25f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_white, "QUIT");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_QUIT;
		}
	}
	else
	{
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_red);
		DrawTextBox(0.25f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_black, "QUIT");
	}
}

void Draw_GameOverMenu()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&UI_mouseX, &UI_mouseY);

	DrawRect(0.f, 0.f, 1600.f, 900.f, pause_bg);
	DrawRect(0.f, 0.f, 1200.f, 600.f, pause_screen);
	DrawTextBox(-0.5f, 0.25f, 800.f, 200.f, 2.f, UI_font, UI_black, "GAME OVER");

	// play button (click to go to Level1 state)
	if (IsAreaClicked(500.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Play\n";
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(-0.55f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_white, "REPLAY");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_RESTART;
		}
	}
	else
	{
		DrawRect(-300.f, -50.f, 300.f, 200.f, UI_green);
		DrawTextBox(-0.55f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_black, "REPLAY");
	}

	// exit button (click to go to quit state)
	if (IsAreaClicked(1100.f, 500.f, 300.f, 200.f, UI_mouseX, UI_mouseY))
	{
		std::cout << "Click Exit\n";
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_hover_red);
		DrawTextBox(0.25f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_white, "QUIT");
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_QUIT;
		}
	}
	else
	{
		DrawRect(300.f, -50.f, 300.f, 200.f, UI_red);
		DrawTextBox(0.25f, -0.15f, 300.f, 200.f, 1.f, UI_font, UI_black, "QUIT");
	}
}

void Update_LevelTimer(float *time_elapsed, float dt)
{
	*time_elapsed += dt;
}

void Draw_LevelTimer(float time_elapsed)
{
	int minutes = static_cast<int>(time_elapsed) / 60;
	int seconds = static_cast<int>(time_elapsed) % 60;
	int milliseconds = static_cast<int>((time_elapsed - static_cast<int>(time_elapsed)) * 100);

	snprintf(level_timer, sizeof(level_timer), "%02d:%02d:%02d", minutes, seconds, milliseconds);

	DrawTextBox(0.5f, -0.9f, 200.f, 100.f, 1.f, UI_font, UI_black, level_timer);
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