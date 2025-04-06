//---------------------------------------------------------
// File:	Credits.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Manfred Sim
//
// Brief:	Credits function implementation for rendering
//			text on screen
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "Credits.h"
#include "utils.h"
#include "GameStateManager.h"
#include "UI.h"

// Font
s8 credits_font, credits_font2;

// Color
Color credits_bg, credits_gold, credits_red, credits_white;

// Mouse Coordinates
s32 credits_mouseX, credits_mouseY;

// Credits Vectors
std::vector<float> credits_IO;
std::vector<std::string> credits_content;

// Credits Timers
Timer credits_buttonanimationtimer, credits_delaytimer, credits_resetdelaytimer;

// Credits Offset
f32 credits_offset, credits_max_offset, credits_normal_scroll_speed, credits_fast_scroll_speed;

// Back Button
SpriteSheet_Old* spritesheet_backbutton;
SpriteSheet_Old backbutton;
int isBackClicked;

// Logo at End of Credits
AEGfxTexture* creditslogo;

void Credits_Load()
{
	// Meshes
	Load_Mesh(20);

	// Font
	credits_font = AEGfxCreateFont("Assets/buggy-font.ttf", 72);
	credits_font2 = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	// Back To Main Menu Button
	backbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 12, 8, 1.f);

	// Logo
	creditslogo = AEGfxTextureLoad("Assets/Digipen_Logo.png");

	// File IO
	ReadFromFile("Assets/FileIO/CreditsInit.txt", credits_IO);
	ReadWholeFile("Assets/FileIO/Credits.txt", credits_content);
}

void Credits_Initialize()
{
	// Colors
	credits_bg = SetColor(0xFF666666);
	credits_gold = SetColor(0xFFFFD218);
	credits_red = SetColor(0xFFFF6961);
	credits_white = SetColor(0xFFFFFFFF);

	// Delay before credits start scrolling
	credits_delaytimer = Create_Timer(credits_IO[0]);				// CREDITS DELAY TIMER = 2.f
	credits_delaytimer.current = 0.f;

	// Delay before credits reset back to start
	credits_resetdelaytimer = Create_Timer(credits_IO[1]);			// CREDITS RESET DELAY TIMER = 3.f
	credits_resetdelaytimer.current = 0.f;

	// How fast the credits scroll up
	credits_offset = 0.f;
	credits_max_offset = credits_IO[2];								// CREDITS MAX OFFSET = 5500.f
	credits_normal_scroll_speed = credits_IO[3];					// CREDITS NORMAL SCROLL SPEED = 5.f
	credits_fast_scroll_speed = credits_IO[4];						// CREDITS FAST SCROLL SPEED = 25.f

	// Spritesheet for back button
	spritesheet_backbutton = &backbutton;
	credits_buttonanimationtimer = Create_Timer(credits_IO[5]);		// CREDITS BUTTON ANIMATION TIMER = 0.5f
	isBackClicked = 0;
}

void Credits_Update()
{
	// Get mouse position coordinates
	AEInputGetCursorPosition(&credits_mouseX, &credits_mouseY);

	if (AEInputCheckTriggered(AEVK_RSHIFT) && credits_offset < credits_max_offset)
	{
		credits_delaytimer.current = credits_delaytimer.duration;
	}
	if (!Update_Timer(&credits_delaytimer))
	{
		if (credits_offset < credits_max_offset)
		{
			// Speed up credits scrolling when holding right shift
			if (AEInputCheckCurr(AEVK_RSHIFT))
			{
				credits_offset += credits_fast_scroll_speed;
			}
			else
			{
				credits_offset += credits_normal_scroll_speed;
			}
			credits_resetdelaytimer.current = 0.f;
		}
		else
		{
			if (!Update_Timer(&credits_resetdelaytimer))
			{
				credits_offset = 0.f;
				credits_delaytimer.current = 0.f;
			}
		}
	}

	// ROLL CREDITS
	DrawTextAdvanced(credits_IO[6], credits_IO[7] + credits_offset, credits_font, credits_gold, credits_content[0].c_str(), false, credits_IO[33]);
	DrawTextAdvanced(credits_IO[6], credits_IO[8] + credits_offset, credits_font, credits_red, credits_content[1].c_str(), false, credits_IO[34]);
	DrawTextAdvanced(credits_IO[6], credits_IO[9] + credits_offset, credits_font, credits_white, credits_content[2].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[10] + credits_offset, credits_font, credits_red, credits_content[3].c_str(), false, credits_IO[34]);
	DrawTextAdvanced(credits_IO[6], credits_IO[11] + credits_offset, credits_font, credits_white, credits_content[4].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[12] + credits_offset, credits_font, credits_white, credits_content[5].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[13] + credits_offset, credits_font, credits_white, credits_content[6].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[14] + credits_offset, credits_font, credits_white, credits_content[7].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[15] + credits_offset, credits_font, credits_white, credits_content[8].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[16] + credits_offset, credits_font, credits_red, credits_content[9].c_str(), false, credits_IO[34]);
	DrawTextAdvanced(credits_IO[6], credits_IO[17] + credits_offset, credits_font, credits_white, credits_content[10].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[18] + credits_offset, credits_font, credits_white, credits_content[11].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[19] + credits_offset, credits_font, credits_white, credits_content[12].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[20] + credits_offset, credits_font, credits_red, credits_content[13].c_str(), false, credits_IO[34]);
	DrawTextAdvanced(credits_IO[6], credits_IO[21] + credits_offset, credits_font, credits_white, credits_content[14].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[22] + credits_offset, credits_font, credits_red, credits_content[15].c_str(), false, credits_IO[34]);
	DrawTextAdvanced(credits_IO[6], credits_IO[23] + credits_offset, credits_font, credits_white, credits_content[16].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[24] + credits_offset, credits_font, credits_white, credits_content[17].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[25] + credits_offset, credits_font, credits_white, credits_content[18].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[26] + credits_offset, credits_font, credits_white, credits_content[19].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[27] + credits_offset, credits_font, credits_white, credits_content[20].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[28] + credits_offset, credits_font, credits_white, credits_content[21].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[29] + credits_offset, credits_font, credits_white, credits_content[22].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[30] + credits_offset, credits_font, credits_white, credits_content[23].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[31] + credits_offset, credits_font, credits_white, credits_content[24].c_str(), false, credits_IO[35]);
	DrawTextAdvanced(credits_IO[6], credits_IO[32] + credits_offset, credits_font2, credits_white, credits_content[25].c_str(), false, credits_IO[36]);

	// Logo
	DrawImage(credits_IO[6], credits_IO[37]  + credits_offset, credits_IO[38], credits_IO[39], credits_IO[40], creditslogo);

	// Background Box
	DrawRect(credits_IO[6], credits_IO[41], credits_IO[42], credits_IO[43], credits_bg);

	// Credits Bar
	DrawRect(credits_IO[44], credits_IO[45], credits_IO[46], credits_IO[47], credits_red);
	DrawRect(credits_IO[44], credits_IO[48] - (0.1f * credits_offset), credits_IO[46], credits_IO[49], credits_gold);

	// back button (click to go back to main menu)

	if (isBackClicked == 0)
	{
		credits_buttonanimationtimer.current = 0.f;
	}

	Draw_SpriteSheet_test(*spritesheet_backbutton, credits_IO[50], credits_IO[51], credits_IO[52], credits_IO[53], credits_IO[54]);

	if (IsAreaClicked(800.f + credits_IO[50], 450.f - credits_IO[51], credits_IO[52], credits_IO[53], credits_mouseX, credits_mouseY))
	{

		isBackClicked = 1;

		if (Update_Timer(&credits_buttonanimationtimer))
		{
			Set_SpriteSheet_current(&backbutton, 5, 1, 8, credits_buttonanimationtimer, credits_buttonanimationtimer.duration);
		}

		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			nextState = GS_MAIN_MENU;
		}
	}
	else
	{
		isBackClicked = 0;
		Set_SpriteSheet_current(&backbutton, 5, 1, 1, credits_buttonanimationtimer, credits_buttonanimationtimer.duration);
	}
}

void Credits_Draw()
{

}

void Credits_Free()
{

}

void Credits_Unload()
{
	AEGfxDestroyFont(credits_font);
	AEGfxDestroyFont(credits_font2);
	Unload_Mesh();
	Unload_SpriteSheet(&backbutton);
	AEGfxTextureUnload(creditslogo);
}