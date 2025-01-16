//---------------------------------------------------------
// file:	Main.cpp
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	UI Port solo project
//			Damage player in red circle
//			Heal player in green circle
//			Render healthbar on top, with segmented bar
//			underneath it for every 10%
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include <AEEngine.h>
#include "utils.h"
#include "player.h"



// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	int gGameRunning = 1;

	// Initialization of your own variables go here

		// Using custom window procedure
		AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

		// Changing the window title
		AESysSetWindowTitle("UI Port Solo");

		// reset the system modules
		AESysReset();

		// Load the font here 
		s8 pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
		
		// Colors
			Color red = SetColor(0xFFFF6961);
			Color green = SetColor(0xFF88B984);
			Color blue = SetColor(0xFF00008B);
			Color black = SetColor(0xFF000000);

		// Meshes
			InitMesh(20);

		// Player
		  Init_Player();

		// Dummy enemy
			AEVec2 dummy_pos = { 0.f, 200.f };
			f32 dummy_rad = 50.f;

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();

		// Basic way to trigger exiting the application
		// when ESCAPE is hit or when the window is closed
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;

		// UPDATE
		// ---------------------------------------------------------------------------
		

		// Delta time!
		f32 dt = (f32)AEFrameRateControllerGetFrameTime();

		// Mouse coords!
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		// Player WASD Movement
		Update_Player(dt);

		// RENDER
		// ---------------------------------------------------------------------------

		AEGfxSetBackgroundColor(0.4f, 0.4f, 0.4f);

		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		// Dummy
		DrawCircle(dummy_pos.x, dummy_pos.y, dummy_rad, red);

		// Player
		Draw_Player();




		// Informing the system about the loop's end
		AESysFrameEnd();

	}


	// free the system
	AEGfxDestroyFont(pFont);
	FreeMesh();
	AESysExit();
}