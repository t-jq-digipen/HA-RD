#include "Tutorial.h"
#include "player.h"
#include "UI.h"
#include "HubWorld.h"

// Tutorial Spritesheets
SpriteSheet WASD_sprite;
SpriteSheet space_sprite;
SpriteSheet melee_sprite;
SpriteSheet arrow_sprite;
SpriteSheet reload_sprite;
SpriteSheet range_sprite;
SpriteSheet parry_sprite;

// Tutorial Checkmark Image
AEGfxTexture* checkmark;

// Tutorial Active Flag
bool tutorialActive;

// Tutorial Transition Timer
Timer transition_timer;

// Tutorial Clear Delay Timer
Timer clear_delay;

// Tutorial Colors
Color tutgreen, tutblack, tutgray, tutbg, tutwhite;

// Tutorial Arrow Pointer Vector
AEVec2 arrowdir;

// File IO
std::vector<float> Tutorial_IO;

// Movement bools
// 0 - W
// 1 - A
// 2 - S
// 3 - D
// 4 - SPACE
bool	move_press[5];
// Melee bools
// 0 - atk 1
// 1 - atk 2
// 2 - atk 3
bool	melee_press[3];
// Ranged bools
// 0 - light
// 1 - heavy
bool	ranged_press[2];
// Block bools
// 0 - block
// 1 - parried successful
bool	block_press[2];

// Tutorial States
typedef enum Tutorial_State {
	TUTORIAL_MOVEMENT,
	TUTORIAL_MELEE,
	TUTORIAL_RANGE,
	TUTORIAL_PARRY,
	TUTORIAL_TRANSITION,
	TUTORIAL_CLEAR
}Tutorial_State;

Tutorial_State tutstate, clearedstate, nextstate;

// Reset tutorial
void Reset_Tutorial()
{
	tutorialActive = true;						// Set tutorial to be active
	tutstate = TUTORIAL_MOVEMENT;		// Set tutorial state to movement
	clearedstate = TUTORIAL_TRANSITION;	// Set to transition, as long as != MOVEMENT
	nextstate = TUTORIAL_MOVEMENT;		// Set next state to movement, will be overwritten by movement state anyway

	for (int i = 0; i < 5; i++)			// Set movement checks to false
	{
		move_press[i] = false;
	}
	for (int i = 0; i < 3; i++)			// Set melee checks to false
	{
		melee_press[i] = false;
	}
	for (int i = 0; i < 2; i++)			// Set ranged checks to false
	{
		ranged_press[i] = false;
	}
	for (int i = 0; i < 2; i++)			// Set block checks to false
	{
		block_press[i] = false;
	}
}

// Create transition when player clears tutorial stage
void Cleared_Tutstage()
{
	if (!Update_Timer(&clear_delay))
	{
		tutstate = TUTORIAL_TRANSITION;
		transition_timer.current = 0.f;
	}
}

// Set next tutorial stage when player clears a tutorial stage
void Set_NextTutstage(Tutorial_State cleared, Tutorial_State next)
{
	clearedstate = cleared;			// Set cleared stage to current
	nextstate = next;				// Set next stage
	clear_delay.current = 0.f;		// Start clear delay time
}


// MOVEMENT SEGMENT
// ----------------------------------------------------------------------------
#pragma region MOVEMENT
// Helper function to check if all movement bools cleared
bool Movement_True()
{
	for (bool b : move_press)
	{
		if (!b)
		{
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// Updates movement tutorial stage
// ----------------------------------------------------------------------------
void Update_Tutorial_Movement()
{
	if (clearedstate != TUTORIAL_MOVEMENT)
	{
		// Clear bools based on WASD keys
		if (AEInputCheckTriggered(AEVK_W))
		{
			move_press[0] = true;
		}
		if (AEInputCheckTriggered(AEVK_A))
		{
			move_press[1] = true;
		}
		if (AEInputCheckTriggered(AEVK_S))
		{
			move_press[2] = true;
		}
		if (AEInputCheckTriggered(AEVK_D))
		{
			move_press[3] = true;
		}
		if (player.state == DODGE)
		{
			move_press[4] = true;
		}
		if (Movement_True())
		{
			Set_NextTutstage(TUTORIAL_MOVEMENT, TUTORIAL_MELEE);
		}
	}
	else		// Cleared
	{
		player.ammo = 0;
		Cleared_Tutstage();
	}
}

#pragma endregion MOVEMENT
// ----------------------------------------------------------------------------

// MELEE SEGMENT
// ----------------------------------------------------------------------------
#pragma region MELEE
// ----------------------------------------------------------------------------
// Updates melee tutorial stage
// ----------------------------------------------------------------------------
void Update_Tutorial_Melee()
{
	if (clearedstate != TUTORIAL_MELEE)
	{
		for (int i = 0; i < 3; i++)
		{
			// Clear the amount of melee bools based on melee combo
			if (i <= melee_attack.combo - 1)
			{
				melee_press[i] = true;
			}
			else
			{
				melee_press[i] = false;
			}
		}
		if (melee_press[2] && player.ammo)
		{
			Set_NextTutstage(TUTORIAL_MELEE, TUTORIAL_RANGE);
		}
	}
	else		// Cleared	
	{
		Cleared_Tutstage();
	}
}
#pragma endregion MELEE
// ----------------------------------------------------------------------------

// RANGE SEGMENT
// ----------------------------------------------------------------------------
#pragma region RANGE
bool Ranged_True()
{
	for (bool b : ranged_press)
	{
		if (!b)
		{
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// Updates ranged tutorial stage
// ----------------------------------------------------------------------------
void Update_Tutorial_Range()
{
	if (clearedstate != TUTORIAL_RANGE)
	{
		if (player.ammo < 3)
		{
			player.ammo = 3;
		}

		if (player.state == RANGE)
		{
			ranged_press[0] = true;
		}
		if (player.state == HEAVY)
		{
			ranged_press[1] = true;
		}

		if (Ranged_True())
		{
			Set_NextTutstage(TUTORIAL_RANGE, TUTORIAL_PARRY);
		}
	}
	else		// Cleared
	{
		Cleared_Tutstage();
	}
}
#pragma endregion RANGE
// ----------------------------------------------------------------------------

// PARRY SEGMENT
// ----------------------------------------------------------------------------

#pragma region PARRY
bool Parry_True()
{
	for (bool b : block_press)
	{
		if (!b)
		{
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// Updates parry tutorial stage
// ----------------------------------------------------------------------------
void Update_Tutorial_Parry()
{
	if (clearedstate != TUTORIAL_PARRY)
	{
		if (player.state == PARRY)
		{
			block_press[0] = true;
		}
		if (player.state == PARRIED)
		{
			block_press[1] = true;
		}

		if (Parry_True())
		{
			Set_NextTutstage(TUTORIAL_PARRY, TUTORIAL_CLEAR);
		}
	}
	else
	{
		Cleared_Tutstage();
	}
}
#pragma endregion PARRY
// ----------------------------------------------------------------------------

// Tutorial Too Far Flag
bool toofar;

// Tutorial Buttons
AEVec2 button_size;
f32 button_offset;

// Tutorial Scaling
f32 draw_y;
f32 movedraw_x;
f32 meleedraw_x;
f32 txtsize_h1, txtsize_h2, txtsize_h3, txtsize_content;
f32 arrow_x, arrow_y, arrow_gate_x, arrow_gate_y;
std::vector<f32> rangedraw_x;

// Update arrow pointer
void Update_Arrow(f32 target_pos_x, f32 target_pos_y)
{
	AEVec2 dummy{ target_pos_x, target_pos_y };

	AEVec2Sub(&arrowdir, &dummy, &player.pos);
	if (AEVec2Length(&arrowdir) > 500.f)
	{
		AEVec2Normalize(&arrowdir, &arrowdir);
		AEVec2Scale(&arrowdir, &arrowdir, 50.f);
		toofar = true;
	}
	else toofar = false;
}

// Initialize the tutorial elements
void Init_Tutorial()
{
	transition_timer = Create_Timer(Tutorial_IO[0]);		// TUTORIAL TRANSITION TIMER	2.0s
	clear_delay = Create_Timer(Tutorial_IO[1]);				// TUTORIAL CLEAR DELAY			1.0s

	toofar = false;
	tutorialActive = false;

	tutgreen	= SetColor(0xFF08A045);
	tutblack	= SetColor(0xFF101010);
	tutgray		= SetColor(0xFFD9D9D9);
	tutbg		= SetColor(0x50000000);
	tutwhite	= SetColor(0xFFFFFFFF);
	
	button_size = { Tutorial_IO[6], Tutorial_IO[7] };		// TUTORIAL BUTTON SIZE			{ 50.f, 50.f }
	button_offset = Tutorial_IO[8];							// TUTORIAL BUTTON OFFSET		100.f

	draw_y = Tutorial_IO[9];								// TUTORIAL SPRITE DRAW Y		150.f					

	movedraw_x = Tutorial_IO[10];							// TUTORIAL MOVE DRAW X			-150.f
	meleedraw_x = Tutorial_IO[11];							// TUTORIAL MELEE DRAW X		-100.f
	rangedraw_x = { Tutorial_IO[12], Tutorial_IO[13] };		// TUTORIAL RANGE DRAW X		{ -100.f, 100.f }

	txtsize_h1 = Tutorial_IO[14];							// TUTORIAL TEXT SIZE H1		0.5f
	txtsize_h2 = Tutorial_IO[15];							// TUTORIAL TEXT SIZE H2		0.35f
	txtsize_h3 = Tutorial_IO[16];							// TUTORIAL TEXT SIZE H3		0.3f
	txtsize_content = Tutorial_IO[17];						// TUTORIAL TEXT SIZE CONTENT	0.2f

	arrow_x = Tutorial_IO[66];								// TUTORIAL ARROW X				600.f
	arrow_y = Tutorial_IO[67];								// TUTORIAL ARROW Y				400.f
	arrow_gate_x = Tutorial_IO[68];							// TUTORIAL ARROW TO GATE X		0.f
	arrow_gate_y = Tutorial_IO[69];							// TUTORIAL ARROW TO GATE Y		750.f
}

// Load the tutorial elements
void Load_Tutorial()
{
	// Read File IO
	ReadFromFile("Assets/FileIO/Tutorial.txt", Tutorial_IO);

	WASD_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/WASD.png", 2, 4);
	melee_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Melee.png", 2, 1);
	space_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Space.png", 2, 1);
	arrow_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Arrow.png", 2, 1);
	reload_sprite = SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Reload.png", 2, 1);
	range_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Range.png", 2, 2);
	parry_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Parry.png", 2, 2);

	checkmark = AEGfxTextureLoad("Assets/UI/Tutorial/Checkmark.png");
}

// Unload the tutorial elements
void Unload_Tutorial()
{
	WASD_sprite.Unload();
	melee_sprite.Unload();
	space_sprite.Unload();
	arrow_sprite.Unload();
	reload_sprite.Unload();
	range_sprite.Unload();
	parry_sprite.Unload();

	AEGfxTextureUnload(checkmark);
}

// Update the tutorial elements
void Update_Tutorial()
{
	if (tutorialActive)
	{
		if (AEInputCheckTriggered(AEVK_T))
		{
			tutorialActive = false;
		}
		Update_Arrow(Tutorial_IO[2], Tutorial_IO[3]);
		switch (tutstate)
		{
		case TUTORIAL_MOVEMENT:
		{
			Update_Tutorial_Movement();
		}
		break;	// TUTORIAL_MOVEMENT

		case TUTORIAL_MELEE:
		{
			Update_Tutorial_Melee();
		}
		break;	// TUTORIAL_MELEE

		case TUTORIAL_RANGE:
		{
			Update_Tutorial_Range();
		}
		break;	// TUTORIAL_RANGE

		case TUTORIAL_PARRY:
		{
			Update_Tutorial_Parry();
		}
		break;	// TUTORIAL_PARRY

		case TUTORIAL_CLEAR:
		{
			tutorialActive = false;
		}
		break;

		} // switch tutstate
		

	}
	else
	{
		Update_Arrow(Tutorial_IO[4], Tutorial_IO[5]);
		if (AEInputCheckTriggered(AEVK_T))
		{
			Reset_Tutorial();
		}
	}
}

// Draw the checkmark
void DrawCheckmark(f32 pos_x, f32 pos_y)
{
	if (clear_delay.current < 1.f)	// Clear delay active
	{
		DrawImage(pos_x, pos_y, button_size.x, button_size.y, 0.f, checkmark);
	}
}

// Draw the arrow pointer sprite
void DrawArrow(f32 target_x, f32 target_y)
{
	arrow_sprite.Set_Row(0);
	arrow_sprite.Loop(0, 1, .5f);
	if (toofar)
	{
		f32 rot = atan2f(arrowdir.y, arrowdir.x);

		arrow_sprite.Draw(World_To_Screen_x(player.pos.x + arrowdir.x), World_To_Screen_y(player.pos.y + arrowdir.y), button_size.x, button_size.y, false, rot);
	}
	else
	{
		arrow_sprite.Draw(World_To_Screen_x(target_x), World_To_Screen_y(target_y), button_size.x, button_size.y, false, 3.f*PI/2.f);
	}
}

// Draw the tutorial elements
void Draw_Tutorial()
{
	if (tutorialActive)
	{
		DrawTextAdvanced(Tutorial_IO[70], Tutorial_IO[71], pixel_font, tutblack, "TUTORIAL", false, txtsize_h1);
		DrawTextAdvanced(Tutorial_IO[18], Tutorial_IO[19], pixel_font, tutwhite, "PRESS T TO SKIP TUTORIAL", false, txtsize_h2);
		switch (tutstate)
		{
		case TUTORIAL_MOVEMENT:
		{
			// Text
			DrawRect(Tutorial_IO[20], Tutorial_IO[21], Tutorial_IO[22], Tutorial_IO[23], tutbg);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[25], pixel_font, tutwhite, "MOVEMENT", false, txtsize_h1);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[26], pixel_font, tutgray,	"Move with WASD keys", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[27], pixel_font, tutgray,	"dodge with SPACEBAR", false, txtsize_content);

			// Images
			for (int i = 0; i < 4; i++)
			{
				WASD_sprite.Set_Row(i);
				WASD_sprite.Set_Col(move_press[i]);

				WASD_sprite.Draw(movedraw_x + i * button_offset, draw_y, button_size.x, button_size.y);
			}
			space_sprite.Set_Row(0);
			space_sprite.Set_Col(move_press[4]);

			space_sprite.Draw(0.f, draw_y - 75.f, button_size.x * 2.f, button_size.y);

			DrawCheckmark(movedraw_x + 4 * button_offset, draw_y);
		}
		break;	// MOVEMENT

		case TUTORIAL_MELEE:
		{
			// Text
			DrawRect(Tutorial_IO[28], Tutorial_IO[29], Tutorial_IO[30], Tutorial_IO[31], tutbg);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[32], pixel_font, tutwhite,	"MELEE ATTACKS", false, txtsize_h2);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[33], pixel_font, tutgray,	"Melee with LMB        ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[34], pixel_font, tutgray,	"Hit up to three times!", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[35], pixel_font, tutgray,	"Each hit restores ammo", false, txtsize_content);

			// Images
			DrawArrow(arrow_x, arrow_y);

			for (int i = 0; i < 3; i++)
			{
				melee_sprite.Set_Row(0);
				melee_sprite.Set_Col(melee_press[i]);

				melee_sprite.Draw(meleedraw_x + i * button_offset, draw_y, button_size.x, button_size.y);
			}

			DrawCheckmark(meleedraw_x + 3 * button_offset, draw_y);
		}
		break;	// MELEE

		case TUTORIAL_RANGE:
		{
			// Text
			DrawRect(Tutorial_IO[36], Tutorial_IO[37], Tutorial_IO[38], Tutorial_IO[39], tutbg);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[40], pixel_font, tutwhite,	"RANGE ATTACKS", false, txtsize_h2);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[41], pixel_font, tutgray,	"Shoot with RMB               ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[42], pixel_font, tutgray,	"Tap to shoot light projectile", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[43], pixel_font, tutgray,		"Hold to unleash a powerful   ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[44], pixel_font, tutgray,		"heavy laser that uses 3 ammo!", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[45], pixel_font, tutgray,	"Tip: Weave light projectiles ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[46], pixel_font, tutgray,	"between melee hits to attack ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[47], pixel_font, tutgray,	"much faster!                 ", false, txtsize_content);

			// Images
			DrawArrow(arrow_x, arrow_y);

			for (int i = 0; i < 2; i++)
			{
				range_sprite.Set_Row(i);
				range_sprite.Set_Col(ranged_press[i]);

				range_sprite.Draw(rangedraw_x[i], draw_y, button_size.x, button_size.y);
			}

			DrawCheckmark(rangedraw_x[1] + button_offset, draw_y);
		}
		break;	// RANGE

		case TUTORIAL_PARRY:
		{
			// Text
			DrawRect(Tutorial_IO[48], Tutorial_IO[49], Tutorial_IO[50], Tutorial_IO[51], tutbg);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[52], pixel_font, tutwhite, "PARRYING", false, txtsize_h1);

			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[53], pixel_font, tutgray,	"Block by holding E     ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[54], pixel_font, tutgray,	"to reduce damage taken ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[55], pixel_font, tutgray,		"Blocking at the right  ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[56], pixel_font, tutgray,		"time results in a parry", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[57], pixel_font, tutgray,	"which negates damage   ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[58], pixel_font, tutgray,	"and restores your ammo!", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[59], pixel_font, tutgray,	"Parrying an attack with", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[60], pixel_font, tutgray,	"an orange spark stuns  ", false, txtsize_content);
			DrawTextAdvanced(Tutorial_IO[24], Tutorial_IO[61], pixel_font, tutgray,	"the enemy!             ", false, txtsize_content);

			// Images
			DrawArrow(arrow_x, arrow_y);

			for (int i = 0; i < 2; i++)
			{
				parry_sprite.Set_Row(i);
				parry_sprite.Set_Col(block_press[i]);

				parry_sprite.Draw(rangedraw_x[i], draw_y, button_size.x, button_size.y);
			}

			DrawCheckmark(rangedraw_x[1] + button_offset, draw_y);

		}
		break;	// PARRY

		case TUTORIAL_TRANSITION:
		{
			if (Update_Timer(&transition_timer))
			{
				DrawTextAdvanced(Tutorial_IO[62], Tutorial_IO[63], pixel_font, tutgreen, "Good job!", false);
				if (nextstate == TUTORIAL_CLEAR)
				{
					DrawTextAdvanced(Tutorial_IO[64], Tutorial_IO[65], pixel_font, tutwhite, "Press T to bring up the tutorial again :)", false, txtsize_h3);
				}
			}
			else
			{
				tutstate = nextstate;
			}
		}
		break;	// TRANSITION

		case TUTORIAL_CLEAR:
		default:
			break;
		}			// switch tutstate


	}
	else if (InTop20 == false && InModifiers == false)
	{
		DrawTextAdvanced(Tutorial_IO[18], Tutorial_IO[19], pixel_font, tutwhite, "PRESS T TO OPEN TUTORIAL", false, txtsize_h2);
	}
	DrawArrow(arrow_gate_x, arrow_gate_y);	// Point to gate
}