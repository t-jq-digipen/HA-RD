//---------------------------------------------------------
// File:	Main.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Manfred Sim
//
// Brief:	Main entry point for HA:RD game, initializing
//			system components, managing gamestates, and
//			running the game loop.
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include <AEEngine.h>
#include "utils.h"
#include "Input.h"
#include "player.h"
#include "GameStateManager.h"


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
		AESysInit(hInstance, nCmdShow, 1600, 900, 0, 60, false, NULL);

		// Changing the window title
		AESysSetWindowTitle("HA:RD");

		// reset the system modules
		AESysReset();

		// Initialize main menu state
		GSM_Initialize(GS_MAIN_MENU);

	// Game Loop
	while (gGameRunning)
	{

		// Game State Manager
		// ---------------------------------------------------------------------------

		while (currentState != GS_QUIT)
		{
			if (currentState != GS_RESTART)
			{
				GSM_Update();
				fpLoad();
			}
			else
			{
				nextState = previousState;
				currentState = previousState;
			}
			fpInitialize();
			while (nextState == currentState)
			{
				// Informing the system about the loop's start
				AESysFrameStart();

				Input_Handle();

				AEGfxSetBackgroundColor(0.4f, 0.4f, 0.4f);

				AEGfxSetRenderMode(AE_GFX_RM_COLOR);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);

				fpUpdate();
				fpDraw();

				// Informing the system about the loop's end
				AESysFrameEnd();
			}
			fpFree();
			if (nextState != GS_RESTART)
			{
				fpUnload();
			}
			previousState = currentState;
			currentState = nextState;
		}
		gGameRunning = 0;
	}
	// free the system
	AESysExit();
}