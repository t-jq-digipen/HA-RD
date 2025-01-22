//---------------------------------------------------------
// file:	player.cpp
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Player functions  |	Line number
//			- Movement			24
//			- Atack				72
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "player.h"

typedef struct Attack
{
	Timer timer;
	int combo;
} Attack;

Player player;
Attack melee_attack;
SpriteSheet sword;

// WASD movement
void Movement() {
	// player movement default 0
	player.vel = { 0,0 };
	f32 boost = 0.f;

	// change when key pressed

	if (AEInputCheckCurr(AEVK_W)) 
	{
		player.vel.y += 1.f;
	}
	if (AEInputCheckCurr(AEVK_A)) 
	{
		player.vel.x -= 1.f;
	}
	if (AEInputCheckCurr(AEVK_S)) 
	{
		player.vel.y -= 1.f;
	}
	if (AEInputCheckCurr(AEVK_D)) 
	{
		player.vel.x += 1.f;
	}

	if (AEInputCheckTriggered(AEVK_SPACE)) 
	{
		player.dodgetimer.current = 0.f;
	}
	if (Update_Timer(&player.dodgetimer))
	{
		
		boost = 1.2f*player.speed;
	}
	
	if (player.vel.x || player.vel.y) 
	{
		AEVec2Normalize(&player.vel, &player.vel);
		AEVec2Scale(&player.vel, &player.vel, (player.speed+boost)*dt);
	}

	// position update
	AEVec2Add(&player.pos, &player.vel, &player.pos);

}


// Attack hitbox
AEGfxTexture* AtkHB;
const float ATTACK_COOLDOWN[4] = { 0.f, 0.5f, 0.7f, 0.7f };
const float ATTACK_DURATION = 1.f;
const float ATTACK_START[4] = { 0.f, 0.2f, 0.2f, 0.3f };
const float ATTACK_END[4] = { 0.f, 0.4f, 0.4f, 0.7f };

const float ATTACK_ANGLE[2] = { PI/4.f, PI/3.f };
const float ATTACK_RADIUS[4] = { 250.f,250.f,250.f,250.f };

f32 atk_pos_x, atk_pos_y, atk_rot;

bool hitbox_active = false;

enum ATTACK_PATTERN
{
	ATTACK_IDLE,	// IDLE :D
	ATTACK_1,		// ATTACK 1 (right swing)
	ATTACK_2,		// ATTACK 2 (left swing)
	ATTACK_3,		// ATTACK 3 (360)
};

// Attack function
bool Attacking() 
{
	// maybe not needed? placeholder i guess
	atk_pos_x = player.pos.x;
	atk_pos_y = player.pos.y;

	// if timer is fully done updating and not idling
	if (!Update_Timer(&melee_attack.timer) && melee_attack.combo) 
	{
		// reset the combo to idle
		melee_attack.combo = ATTACK_IDLE;
		//std::cout << "combo reset\n";
	}

	// check left click input
	// check if melee_attack cooldown over + combo not finished
	if (AEInputCheckTriggered(AEVK_LBUTTON) && (melee_attack.timer.current > ATTACK_COOLDOWN[melee_attack.combo]) && (melee_attack.combo < 3))
	{
		melee_attack.timer.current = 0.f;
		melee_attack.combo++;
		//std::cout << "melee_attack " << melee_attack.combo << '\n';
		sword.timer.current = 0.f;
	}

	// update sword swing timer
	Update_Timer(&sword.timer);

	// update for player to face mouse
	{
		f32 x = Screen_To_World_x(Mouse_X)-atk_pos_x;
		f32 y = Screen_To_World_y(Mouse_Y)-atk_pos_y;
		atk_rot = atan2f(y, x) - PI / 2.f;
	}

	hitbox_active = false;
	// logic for if hitbox should be active, to be replaced with a bool return probably @raziq
	if (melee_attack.timer.current >= ATTACK_START[melee_attack.combo] && melee_attack.timer.current <= ATTACK_END[melee_attack.combo])
	{
		DrawImage(World_To_Screen_x(atk_pos_x), World_To_Screen_y(atk_pos_y), player.radius * 10.f, player.radius * 10.f, atk_rot, AtkHB);
		hitbox_active = true;
	}

	// not idling :> not sure if return will be useful
	if (melee_attack.timer.current < ATTACK_DURATION) 
	{
		return true;
	}
	return false;
}

int Attacked_Melee(AEVec2 target)
{
	// Check if:
	// - Hitbox is active
	// - Player is attacking
	// - Target is within range of current attack
	if (hitbox_active && melee_attack.combo && (AreCirclesIntersecting(player.pos.x, player.pos.y, ATTACK_RADIUS[melee_attack.combo], target.x, target.y, 25.f)))
	{
		switch (melee_attack.combo)
		{
		case 1:
		case 2:
		{
			// Check direction for first two swings
			f32 target_rot = atan2f(target.y - player.pos.y, target.x - player.pos.x) - PI / 2.f;
			if (target_rot >= atk_rot - ATTACK_ANGLE[melee_attack.combo - 1] && target_rot <= atk_rot + ATTACK_ANGLE[melee_attack.combo - 1])
			{
				std::cout << "HIT ATTACK " << melee_attack.combo << '\n';
				return melee_attack.combo;
			}
			break;
		}
		case 3:
		{
			// No need to check direction for third swing as it is 360 circle
			std::cout << "HIT ATTACK " << melee_attack.combo << '\n';
			return melee_attack.combo;
			break;
		}
		default:
		{
			break;
		}
		}
	}
	return 0;
}

void Load_Player()
{
	AtkHB = AEGfxTextureLoad("Assets/hitboxtest.png");

	sword = Init_SpriteSheet("Assets/slash.png", 4, 10, ATTACK_DURATION);
}

void Init_Player() 
{
	player.pos = { 0,0 };
	player.vel = { 0,0 };
	player.speed = 500.f;
	player.hp = 100.f;
	player.radius = 25.f;
	player.dodgetimer = Create_Timer(0.2f);
	player.color = SetColor(0xFF00008B);

	melee_attack.timer = Create_Timer(1.f);
	melee_attack.combo = 0;
}

void Update_Player() 
{
	Movement();
	if (Attacking()) {
	}
}

// Draw Player
void Draw_Player() 
{
	DrawCircle(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius, player.color);
	//DrawCircle(player.pos.x, player.pos.y, player.radius, player.color);


	sword.current_col = static_cast<int>(sword.timer.current * sword.cols);
	sword.current_row = melee_attack.combo;
	Draw_SpriteSheet(sword, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius * 10.f, player.radius * 10.f, sword.uv_width * sword.current_col, sword.uv_height * sword.current_row, atk_rot);
}

void Free_Player()
{
}

void Unload_Player()
{
	AEGfxTextureUnload(AtkHB);
	AEGfxTextureUnload(sword.texture);
	AEGfxMeshFree(sword.mesh);
}