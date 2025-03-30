//---------------------------------------------------------
// file:	MainMenu.cpp
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Implementation of main menu functions
//
// Copyright � 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "MainMenu.h"
#include "GameStateManager.h"

s8 pFont;
Color red, green, blue, black, white, hover_red;
f32 width, height;
s32 mouseX, mouseY;

SpriteSheet_Old playbutton, exitbutton;
MainMenu mainmenu;

// Global variables
Timer fadeTimer;   // Timer for fade-in effect
bool isFading = true;  // Flag to track fade-in state
AEGfxTexture* digipenLogo;  // Texture for the logo
// File IO
std::vector<float> MainMenu_IO;

void MainMenu_Load()
{
	// Meshes
	Load_Mesh(20);

	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	playbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 9, 8, 1.f);
	exitbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 9, 8, 1.f);

	digipenLogo = AEGfxTextureLoad("Assets/Digipen_Logo.png");

	std::cout << "Main Menu Loaded\n";

	// Read File IO
	ReadFromFile("Assets/FileIO/MainMenu.txt", MainMenu_IO);
}

void MainMenu_Initialize()
{
	std::cout << "Main Menu Initialized\n";

	// Colors
	red = SetColor(0xFFFF6961);
	// green = SetColor(0xFF88B984);  // If needed, uncomment this
	// blue = SetColor(0xFF00008B);  // If needed, uncomment this
	// black = SetColor(0xFF000000);  // If needed, uncomment this
	// white = SetColor(0xFFFFFFFF);  // If needed, uncomment this
	// hover_red = SetColor(0xFFFFA5A1);  // If needed, uncomment this

	// Window width, height
	width = (f32)AEGfxGetWindowWidth();
	height = (f32)AEGfxGetWindowHeight();

    // Initialize the fade-in timer
    fadeTimer.current = 0.0f;  // Start the timer at 0
    fadeTimer.duration = 2.0f; // 2 seconds fade-in time

	mainmenu.spritesheet_button1 = &playbutton;
	mainmenu.spritesheet_button2 = &exitbutton;
	mainmenu.animationtimer = Create_Timer(MainMenu_IO[0]);
	mainmenu.isPlayClicked = mainmenu.isExitClicked = mainmenu.isCreditsClicked = 0;
}

void MainMenu_Update()
{
    // Get the delta time for the current frame
    Get_DT(); // Store the delta time


    f32 totalTime = fadeTimer.duration + 1.0f + fadeTimer.duration; // 2s fade-in + 1s hold + 2s fade-out
    fadeTimer.current += dt;

    // Update the fade-in timer if it's active
    if (isFading)
    {
        // Always draw black background
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetTransparency(1.0f);
        DrawImage(0.f, 0.f, width, height, 0.f, nullptr);

        if (fadeTimer.current < fadeTimer.duration) // Fading in (0s to 2s)
        {
            // Calculate alpha for fade-in (0 1)
            f32 alpha = fadeTimer.current / fadeTimer.duration;

            // Draw the logo with fade-in effect
            DrawImageFading(0.f, 0.f, width, (height / 2), 0.f, digipenLogo, alpha);
        }
        else if (fadeTimer.current < fadeTimer.duration + 1.0f) // Hold for 1s (2s to 3s)
        {
            // Logo stays fully visible
            DrawImageFading(0.f, 0.f, width, height / 2, 0.f, digipenLogo, 1.0f);
        }
        else if (fadeTimer.current < totalTime+ 0.1f) // Fading out (3s to 5s)
        {
            f32 fadeOutTime = fadeTimer.current - (fadeTimer.duration + 1.0f);
            f32 alpha = 1.0f - (fadeOutTime / fadeTimer.duration); // Alpha fades from 1 0


            // Draw the logo fading out
            DrawImageFading(0.f, 0.f, width, height / 2, 0.f, digipenLogo, alpha);
        }
        else // Fade-out is done
        {
            isFading = false; // Only disable fading **after the whole process**
        }
    }


    else
    {
        // Get mouse position coordinates
        AEInputGetCursorPosition(&mouseX, &mouseY);

        // Basic way to trigger exiting the application
        if (AEInputCheckTriggered(AEVK_ESCAPE))
        {
            nextState = GS_QUIT;
        }

        // game title
        DrawTextBox(MainMenu_IO[1], MainMenu_IO[2], width, MainMenu_IO[3], MainMenu_IO[4], pFont, red, "HARD");

        if (mainmenu.isPlayClicked == 0 && mainmenu.isExitClicked == 0)
        {
            mainmenu.animationtimer.current = 0.f;
        }

        // play button (click to go to Level1 state)
        Draw_SpriteSheet_test(*mainmenu.spritesheet_button1, MainMenu_IO[5], MainMenu_IO[6], MainMenu_IO[7], MainMenu_IO[8], MainMenu_IO[9]);

        if (IsAreaClicked(width / 2 + MainMenu_IO[5], height / 2 - MainMenu_IO[6], MainMenu_IO[7], MainMenu_IO[8], mouseX, mouseY))
        {
            
            mainmenu.isPlayClicked = 1;

            // Increment animation timer by dt manually
            mainmenu.animationtimer.current += dt;

            if (mainmenu.animationtimer.current >= mainmenu.animationtimer.duration)
            {
                mainmenu.animationtimer.current = mainmenu.animationtimer.duration; // Clamp to max duration
            }

            Set_SpriteSheet_current(&playbutton, 0, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);

            if (AEInputCheckTriggered(AEVK_LBUTTON))
            {
                nextState = GS_HUBWORLD;
            }
        }
        else
        {
            mainmenu.isPlayClicked = 0;
            mainmenu.animationtimer.current = 0.f; // Reset timer when not clicked
            Set_SpriteSheet_current(&playbutton, 0, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }

        // exit button (click to go to quit state)
        Draw_SpriteSheet_test(*mainmenu.spritesheet_button2, MainMenu_IO[10], MainMenu_IO[11], MainMenu_IO[12], MainMenu_IO[13], MainMenu_IO[14]);

        if (IsAreaClicked(width / 2 + MainMenu_IO[10], height / 2 - MainMenu_IO[11], MainMenu_IO[12], MainMenu_IO[13], mouseX, mouseY))
        {
            mainmenu.isExitClicked = 1;
            // Increment animation timer by dt manually
            mainmenu.animationtimer.current += dt;

            if (mainmenu.animationtimer.current >= mainmenu.animationtimer.duration)
            {
                mainmenu.animationtimer.current = mainmenu.animationtimer.duration; // Clamp to max duration
            }

            Set_SpriteSheet_current(&exitbutton, 2, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);

            if (AEInputCheckTriggered(AEVK_LBUTTON))
            {
                nextState = GS_QUIT;
            }
        }
        else
        {
            mainmenu.isExitClicked = 0;
            mainmenu.animationtimer.current = 0.f; // Reset timer when not clicked
            Set_SpriteSheet_current(&exitbutton, 2, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }
    }
}


void MainMenu_Draw()
{

}

void MainMenu_Free()
{

}

void MainMenu_Unload()
{
	AEGfxDestroyFont(pFont);
	Unload_Mesh();
	Unload_SpriteSheet(&playbutton);
	Unload_SpriteSheet(&exitbutton);
    AEGfxTextureUnload(digipenLogo);

}