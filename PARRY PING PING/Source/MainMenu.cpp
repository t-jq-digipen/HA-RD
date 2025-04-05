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

// Font
s8 pFont;

// Colors
Color red, green, blue, black, white, hover_red;

// Window width, height
f32 width, height;

// Mouse coordinates
s32 mouseX, mouseY;

// Button spritesheets
SpriteSheet_Old playbutton, creditsbutton, exitbutton;

// Logo spritesheet
SpriteSheet_Old menulogo;

// Main menu object
MainMenu mainmenu;

// Global variables
Timer fadeTimer;   // Timer for fade-in effect
bool isFading = true;  // Flag to track fade-in state
AEGfxTexture* digipenLogo;  // Texture for the logo
// File IO
std::vector<float> MainMenu_IO;

// ----------------------------------------------------------------------------
// Loads the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Load()
{
    std::cout << "Main Menu Loaded\n";

	// Meshes
	Load_Mesh(20);

    // Font
	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

    // Button Spritesheets
	playbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
    creditsbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);
	exitbutton = Init_SpriteSheet("Assets/UI/MenuButton.png", 10, 8, 1.f);

    // Game Logo
    menulogo = Init_SpriteSheet("Assets/UI/HardLogo.png", 1, 16, 1.f);

    // DigiPen Logo
	digipenLogo = AEGfxTextureLoad("Assets/Digipen_Logo.png");

	// Read File IO
	ReadFromFile("Assets/FileIO/MainMenu.txt", MainMenu_IO);
}

// ----------------------------------------------------------------------------
// Initializes the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Initialize()
{
	std::cout << "Main Menu Initialized\n";

	// Colors
	// red = SetColor(0xFFFF6961);
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

    // Initialize spritesheet buttons
	mainmenu.spritesheet_button1 = &playbutton;
	mainmenu.spritesheet_button2 = &creditsbutton;
    mainmenu.spritesheet_button3 = &exitbutton;

    // Initialize spritesheet logo
    mainmenu.spritesheet_logo = &menulogo;

    // Button animation timer
	mainmenu.animationtimer = Create_Timer(MainMenu_IO[0]);

    // Logo animation timer
    mainmenu.animationlogotimer = Create_Timer(MainMenu_IO[0]);

    // Button hover flags
	mainmenu.isPlayClicked = mainmenu.isExitClicked = mainmenu.isCreditsClicked = 0;

    // Logo reset animation timer
    mainmenu.logoresettimer = Create_Timer(MainMenu_IO[20]);
    mainmenu.logoresettimer.current = 0.f;
}

// ----------------------------------------------------------------------------
// Updates the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Update()
{
    // Get the delta time for the current frame
    Get_DT(); // Store the delta time


    f32 totalTime = fadeTimer.duration + 1.0f + fadeTimer.duration; // 2s fade-in + 1s hold + 2s fade-out
    fadeTimer.current += dt;

    // Skips Logo when Right Shift is Pressed
    if (AEInputCheckTriggered(AEVK_RSHIFT))
    {
        isFading = false;
    }

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

        // Game Logo
        Draw_SpriteSheet_test(*mainmenu.spritesheet_logo, MainMenu_IO[1], MainMenu_IO[2], MainMenu_IO[3], MainMenu_IO[4], 0.f);
        Loop_Timer(&mainmenu.logoresettimer);
        std::cout << mainmenu.animationlogotimer.current << '\n';
        if (mainmenu.logoresettimer.current == 0.f)
        {       
            mainmenu.animationlogotimer.current = 0.f;
        }
        if (Update_Timer(&mainmenu.animationlogotimer))
        {
            Set_SpriteSheet_current(&menulogo, 0, 1, 16, mainmenu.animationlogotimer, mainmenu.animationlogotimer.duration);
        }
        else
        {
            Set_SpriteSheet_current(&menulogo, 0, 16, 16, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }
        

        if (mainmenu.isPlayClicked == 0 && mainmenu.isExitClicked == 0 && mainmenu.isCreditsClicked == 0)
        {
            mainmenu.animationtimer.current = 0.f;
        }

        // Play Button (Click to go to Hubworld)
        Draw_SpriteSheet_test(*mainmenu.spritesheet_button1, MainMenu_IO[5], MainMenu_IO[6], MainMenu_IO[7], MainMenu_IO[8], MainMenu_IO[9]);

        if (IsAreaClicked(width / 2 + MainMenu_IO[5], height / 2 - MainMenu_IO[6], MainMenu_IO[7], MainMenu_IO[8], mouseX, mouseY))
        {
            
            mainmenu.isPlayClicked = 1;

            if (Update_Timer(&mainmenu.animationtimer))
            {
                Set_SpriteSheet_current(&playbutton, 0, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);
            }

            if (AEInputCheckTriggered(AEVK_LBUTTON))
            {
                nextState = GS_HUBWORLD;
            }
        }
        else
        {
            mainmenu.isPlayClicked = 0;
            Set_SpriteSheet_current(&playbutton, 0, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }

        // Credits Button (Click to go to credits)
        Draw_SpriteSheet_test(*mainmenu.spritesheet_button2, MainMenu_IO[10], MainMenu_IO[11], MainMenu_IO[12], MainMenu_IO[13], MainMenu_IO[14]);

        if (IsAreaClicked(width / 2 + MainMenu_IO[10], height / 2 - MainMenu_IO[11], MainMenu_IO[12], MainMenu_IO[13], mouseX, mouseY))
        {

            mainmenu.isCreditsClicked = 1;

            if (Update_Timer(&mainmenu.animationtimer))
            {
                Set_SpriteSheet_current(&creditsbutton, 8, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);
            }

            if (AEInputCheckTriggered(AEVK_LBUTTON))
            {
                nextState = GS_CREDITS;
            }
        }
        else
        {
            mainmenu.isCreditsClicked = 0;
            Set_SpriteSheet_current(&creditsbutton, 8, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }

        // Exit Button (Click to exit application)
        Draw_SpriteSheet_test(*mainmenu.spritesheet_button3, MainMenu_IO[15], MainMenu_IO[16], MainMenu_IO[17], MainMenu_IO[18], MainMenu_IO[19]);

        if (IsAreaClicked(width / 2 + MainMenu_IO[15], height / 2 - MainMenu_IO[16], MainMenu_IO[17], MainMenu_IO[18], mouseX, mouseY))
        {
            mainmenu.isExitClicked = 1;

            if (Update_Timer(&mainmenu.animationtimer))
            {
                Set_SpriteSheet_current(&exitbutton, 2, 1, 8, mainmenu.animationtimer, mainmenu.animationtimer.duration);
            }

            if (AEInputCheckTriggered(AEVK_LBUTTON))
            {
                nextState = GS_QUIT;
            }
        }
        else
        {
            mainmenu.isExitClicked = 0;
            Set_SpriteSheet_current(&exitbutton, 2, 1, 1, mainmenu.animationtimer, mainmenu.animationtimer.duration);
        }
    }
}

// ----------------------------------------------------------------------------
// Draws the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Draw()
{

}

// ----------------------------------------------------------------------------
// Frees the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Free()
{

}

// ----------------------------------------------------------------------------
// Unloads the main menu elements
// ----------------------------------------------------------------------------
void MainMenu_Unload()
{
	AEGfxDestroyFont(pFont);
	Unload_Mesh();
	Unload_SpriteSheet(&playbutton);
    Unload_SpriteSheet(&creditsbutton);
	Unload_SpriteSheet(&exitbutton);
    Unload_SpriteSheet(&menulogo);
    AEGfxTextureUnload(digipenLogo);

}