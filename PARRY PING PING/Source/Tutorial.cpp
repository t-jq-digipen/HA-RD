#include "Tutorial.h"
#include "player.h"
#include "UI.h"

SpriteSheet WASD_sprite;
SpriteSheet space_sprite;
SpriteSheet melee_sprite;

bool active;
Timer transition_timer;
Color tutgreen, tutblack;

// Movement bools
// 0 - W
// 1 - A
// 2 - S
// 3 - D
// 4 - SPACE
bool move_press[5];
bool melee_press[3];

typedef enum Tutorial_State {
	TUTORIAL_MOVEMENT,
	TUTORIAL_MELEE,
	TUTORIAL_RANGE,
	TUTORIAL_BLOCK,
	TUTORIAL_PARRY,
	TUTORIAL_HEAVY,
	TUTORIAL_TRANSITION
}Tutorial_State;

Tutorial_State tutstate, clearedstate, nextstate;


void Reset_Tutorial()
{
	active = true;					// Set tutorial to be active
	tutstate = TUTORIAL_MOVEMENT;	// Set tutorial state to movement
	for (bool b : move_press)		// Set movement checks to false
	{
		b = false;
	}
	for (bool b : melee_press)		// Set melee checks to false
	{
		b = false;
	}
}


// MOVEMENT SEGMENT
// ----------------------------------------------------------------------------
#pragma region MOVEMENT
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

void Update_Tutorial_Movement()
{
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
		clearedstate	= TUTORIAL_MOVEMENT;
		nextstate		= TUTORIAL_MELEE;
		tutstate		= TUTORIAL_TRANSITION;
		transition_timer.current = 0.f;
	}
}

#pragma endregion MOVEMENT
// ----------------------------------------------------------------------------

// MELEE SEGMENT
// ----------------------------------------------------------------------------
#pragma region MELEE
void Update_Tutorial_Melee()
{
	for (int i = 0; i < 3; i++)
	{
		if (i <= melee_attack.combo-1)
		{
			melee_press[i] = true;
		}
		else
		{
			melee_press[i] = false;
		}
	}
	if (melee_press[2])
	{
		clearedstate	= TUTORIAL_MELEE;
		nextstate		= TUTORIAL_RANGE;
		tutstate		= TUTORIAL_TRANSITION;
		transition_timer.current = 0.f;
	}
}

#pragma endregion MELEE



void Init_Tutorial()
{
	transition_timer = Create_Timer(2.f);
	tutgreen = SetColor(0xFF08A045);
	tutblack = SetColor(0xFF101010);
	Reset_Tutorial();
}

void Load_Tutorial()
{
	WASD_sprite = SpriteSheet::SpriteSheet("Assets/UI/Tutorial/WASD.png", 2, 4);
	melee_sprite = SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Melee.png", 2, 1);
	space_sprite = SpriteSheet::SpriteSheet("Assets/UI/Tutorial/Space.png", 2, 1);
}

void Unload_Tutorial()
{
	WASD_sprite.Unload();
	melee_sprite.Unload();
	space_sprite.Unload();
}

void Update_Tutorial()
{
	if (active)
	{
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

		}
		

	}
	else
	{
		if (AEInputCheckTriggered(AEVK_T))
		{
			Reset_Tutorial();
			active = true;
		}
	}
}

AEVec2 button_size{ 50.f,50.f };
f32 button_offset = 100.f;

const f32 draw_y = 150.f;

const f32 movedraw_x = -150.f;
const f32 meleedraw_x = -100.f;

void Draw_Tutorial()
{
	if (active)
	{
		switch (tutstate)
		{
		case TUTORIAL_MOVEMENT:
		{
			DrawTextAdvanced(0.f, 400.f, pixel_font, tutblack, "MOVEMENT", false, .5f);
			for (int i = 0; i < 4; i++)
			{
				WASD_sprite.Set_Row(i);
				WASD_sprite.Set_Col(move_press[i]);

				WASD_sprite.Draw(movedraw_x + i * button_offset, draw_y, button_size.x, button_size.y);
			}
			space_sprite.Set_Row(0);
			space_sprite.Set_Col(move_press[4]);

			space_sprite.Draw(0.f, draw_y - 75.f, button_size.x * 2.f, button_size.y);
		}
		break;	// MOVEMENT

		case TUTORIAL_MELEE:
		{
			DrawTextAdvanced(0.f, 400.f, pixel_font, tutblack, "MELEE ATTACKS", false, .5f);
			for (int i = 0; i < 3; i++)
			{
				melee_sprite.Set_Row(0);
				melee_sprite.Set_Col(melee_press[i]);

				melee_sprite.Draw(meleedraw_x + i * button_offset, draw_y, button_size.x, button_size.y);
			}
		}
		break;	// MELEE

		case TUTORIAL_RANGE:
		{

		}
		break;	// RANGE

		case TUTORIAL_BLOCK:
		{

		}
		break;	// BLOCK

		case TUTORIAL_PARRY:
		{

		}
		break;	// PARRY

		case TUTORIAL_HEAVY:
		{

		}
		break;	// HEAVY

		case TUTORIAL_TRANSITION:
		{
			if (Update_Timer(&transition_timer))
			{
				switch (clearedstate)
				{
				default:
					DrawTextAdvanced(0.f, 300.f, pixel_font, tutgreen, "Good job!", false);
				}
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