#include "Tutorial.h"
#include "player.h"
#include "UI.h"

SpriteSheet WASD_sprite;
SpriteSheet space_sprite;
SpriteSheet melee_sprite;
SpriteSheet arrow_sprite;
SpriteSheet reload_sprite;
SpriteSheet range_sprite;
SpriteSheet parry_sprite;

AEGfxTexture* checkmark;

bool active;
Timer transition_timer;
Timer clear_delay;
Color tutgreen, tutblack, tutgray, tutbg, tutwhite;

AEVec2 arrowdir;

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
		
typedef enum Tutorial_State {
	TUTORIAL_MOVEMENT,
	TUTORIAL_MELEE,
	TUTORIAL_RANGE,
	TUTORIAL_PARRY,
	TUTORIAL_TRANSITION,
	TUTORIAL_CLEAR
}Tutorial_State;

Tutorial_State tutstate, clearedstate, nextstate;


void Reset_Tutorial()
{
	active = true;						// Set tutorial to be active
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

void Cleared_Tutstage()
{
	if (!Update_Timer(&clear_delay))
	{
		tutstate = TUTORIAL_TRANSITION;
		transition_timer.current = 0.f;
	}
}

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
// ----------------------------------------------------------------------------
// Updates ranged tutorial stage
// ----------------------------------------------------------------------------
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


bool toofar;

void Update_Arrow()
{
	AEVec2 dummy{ 600.f, 200.f };

	AEVec2Sub(&arrowdir, &dummy, &player.pos);
	if (AEVec2Length(&arrowdir) > 500.f)
	{
		AEVec2Normalize(&arrowdir, &arrowdir);
		AEVec2Scale(&arrowdir, &arrowdir, 50.f);
		toofar = true;
	}
	else toofar = false;
}

void Init_Tutorial()
{
	transition_timer = Create_Timer(2.f);
	clear_delay = Create_Timer(1.f);

	toofar = false;

	tutgreen	= SetColor(0xFF08A045);
	tutblack	= SetColor(0xFF101010);
	tutgray		= SetColor(0xFFD9D9D9);
	tutbg		= SetColor(0x50000000);
	tutwhite	= SetColor(0xFFFFFFFF);
	Reset_Tutorial();
}

void Load_Tutorial()
{
	WASD_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/WASD.png", 2, 4);
	melee_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Melee.png", 2, 1);
	space_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Space.png", 2, 1);
	arrow_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Arrow.png", 2, 1);
	reload_sprite = SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Reload.png", 2, 1);
	range_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Range.png", 2, 2);
	parry_sprite =	SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Parry.png", 2, 2);

	checkmark = AEGfxTextureLoad("Assets/UI/Tutorial/Checkmark.png");
}

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

void Update_Tutorial()
{
	if (active)
	{
		if (AEInputCheckTriggered(AEVK_T))
		{
			active = false;
		}
		Update_Arrow();
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
			active = false;
		}
		break;

		} // switch tutstate
		

	}
	else
	{
		if (AEInputCheckTriggered(AEVK_T))
		{
			Reset_Tutorial();
		}
	}
}

AEVec2 button_size{ 50.f,50.f };
f32 button_offset = 100.f;

const f32 draw_y = 150.f;

const f32 movedraw_x = -150.f;
const f32 meleedraw_x = -100.f;
const f32 rangedraw_x[2] = { -100.f, 100.f };

void DrawCheckmark(f32 pos_x, f32 pos_y)
{
	if (clear_delay.current < 1.f)	// Clear delay active
	{
		DrawImage(pos_x, pos_y, button_size.x, button_size.y, 0.f, checkmark);
	}
}

void DrawArrow()
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
		arrow_sprite.Draw(World_To_Screen_x(600.f), World_To_Screen_y(400.f), button_size.x, button_size.y, false, 3.f*PI/2.f);
	}
}

void Draw_Tutorial()
{
	if (active)
	{
		DrawTextAdvanced(0.f, 700.f, pixel_font, tutblack, "PRESS T TO SKIP TUTORIAL", false, .5f);
		switch (tutstate)
		{
		case TUTORIAL_MOVEMENT:
		{
			// Text
			DrawRect(-400.f, 150.f, 350.f, 200.f, tutbg);

			DrawTextAdvanced(-800.f, 400.f, pixel_font, tutwhite,	"MOVEMENT", false, .5f);

			DrawTextAdvanced(-800.f, 200.f, pixel_font, tutgray,	"Move with WASD keys", false, .2f);
			DrawTextAdvanced(-800.f, 150.f, pixel_font, tutgray,	"dodge with SPACEBAR", false, .2f);

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
			DrawRect(-400.f, 125.f, 350.f, 250.f, tutbg);

			DrawTextAdvanced(-800.f, 400.f, pixel_font, tutwhite,	"MELEE ATTACKS", false, .35f);

			DrawTextAdvanced(-800.f, 200.f, pixel_font, tutgray,	"Melee with LMB        ", false, .2f);
			DrawTextAdvanced(-800.f, 125.f, pixel_font, tutgray,	"Hit up to three times!", false, .2f);
			DrawTextAdvanced(-800.f, 75.f, pixel_font, tutgray,		"Each hit restores ammo", false, .2f);

			// Images
			DrawArrow();

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
			DrawRect(-400.f, 50.f, 450.f, 400.f, tutbg);

			DrawTextAdvanced(-800.f, 400.f, pixel_font, tutwhite,	"RANGE ATTACKS", false, .35f);

			DrawTextAdvanced(-800.f, 200.f, pixel_font, tutgray,	"Shoot with RMB               ", false, .2f);
			DrawTextAdvanced(-800.f, 125.f, pixel_font, tutgray,	"Tap to shoot light projectile", false, .2f);
			DrawTextAdvanced(-800.f, 50.f, pixel_font, tutgray,		"Hold to unleash a powerful   ", false, .2f);
			DrawTextAdvanced(-800.f, 0.f, pixel_font, tutgray,		"heavy laser that uses 3 ammo!", false, .2f);
			DrawTextAdvanced(-800.f, -75.f, pixel_font, tutgray,	"Tip: Weave light projectiles ", false, .2f);
			DrawTextAdvanced(-800.f, -125.f, pixel_font, tutgray,	"between melee hits to attack ", false, .2f);
			DrawTextAdvanced(-800.f, -175.f, pixel_font, tutgray,	"much faster!                 ", false, .2f);

			// Images
			DrawArrow();

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
			DrawRect(-400.f, 50.f, 350.f, 400.f, tutbg);

			DrawTextAdvanced(-800.f, 400.f, pixel_font, tutwhite, "PARRYING", false, .5f);

			DrawTextAdvanced(-800.f, 200.f, pixel_font, tutgray,	"Block by holding E     ", false, .2f);
			DrawTextAdvanced(-800.f, 150.f, pixel_font, tutgray,	"to reduce damage taken ", false, .2f);
			DrawTextAdvanced(-800.f, 75.f, pixel_font, tutgray,		"Blocking at the right  ", false, .2f);
			DrawTextAdvanced(-800.f, 25.f, pixel_font, tutgray,		"time results in a parry", false, .2f);
			DrawTextAdvanced(-800.f, -25.f, pixel_font, tutgray,	"which negates damage   ", false, .2f);
			DrawTextAdvanced(-800.f, -75.f, pixel_font, tutgray,	"and restores your ammo!", false, .2f);
			DrawTextAdvanced(-800.f, -150.f, pixel_font, tutgray,	"Parrying an attack with", false, .2f);
			DrawTextAdvanced(-800.f, -200.f, pixel_font, tutgray,	"an orange spark stuns  ", false, .2f);
			DrawTextAdvanced(-800.f, -250.f, pixel_font, tutgray,	"the enemy!             ", false, .2f);

			// Images
			DrawArrow();

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
				DrawTextAdvanced(0.f, 300.f, pixel_font, tutgreen, "Good job!", false);
				
			}
			else
			{
				tutstate = nextstate;
			}
		}
		break;	// TRANSITION
		default:
			break;
		}		// switch tutstate


	}
}