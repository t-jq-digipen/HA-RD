//---------------------------------------------------------
// file:	player.cpp
// author:	TAN JUN QUAN
// email:	t.junquan@digipen.edu
//
// brief:	Player movement function
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "player.h"

Player player;
Attack attack;
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
	if (player.dodgetimer.current<player.dodgetimer.end) 
	{
		player.dodgetimer.current += 5.0f*dt;
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
const float ATTACK_COOLDOWN = 0.7f;
const float ATTACK_DURATION = 0.3f;
const float ATTACK_START[4] = { 0.f, 0.2f, 0.2f, 0.3f };
const float ATTACK_END[4] = { 0.f, 0.4f, 0.4f, 0.7f };

f32 atk_pos_x, atk_pos_y, atk_rot;

enum ATTACK_PATTERN
{
	ATTACK_IDLE,
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
};

// Attack function
bool Attacking() 
{
	// maybe not needed? placeholder i guess
	atk_pos_x = player.pos.x;
	atk_pos_y = player.pos.y;

	// if timer is fully done updating and there is combo active
	if (!Update_Timer(&attack.timer, 1.f) && attack.combo) 
	{
		// reset the combo
		attack.combo = ATTACK_IDLE;
		std::cout << "combo reset\n";
	}

	// check left click input
	// check if attack cooldown over + combo not finished
	if (AEInputCheckTriggered(AEVK_LBUTTON) && (attack.timer.current > ATTACK_COOLDOWN) && (attack.combo < 3)) 
	{
		attack.timer.current = 0.f;
		attack.combo++;
		std::cout << "attack " << attack.combo << '\n';
		sword.timer.current = 0.f;
	}

	// update sword swing timer
	Update_Timer(&sword.timer, 1.0f);

	// update for player to face mouse
	{
		s32 mouse_x, mouse_y;
		AEInputGetCursorPosition(&mouse_x, &mouse_y);
		f32 x = (f32)mouse_x - atk_pos_x - (AEGfxGetWindowWidth() / 2.f);
		f32 y = -(f32)mouse_y - atk_pos_y + (AEGfxGetWindowHeight() / 2.f);
		atk_rot = atan2f(y, x) - PI / 2.f;
	}

	// logic for if hitbox should be active, to be replaced with a bool return probably @raziq
	if (attack.timer.current >= ATTACK_START[attack.combo] && attack.timer.current <= ATTACK_END[attack.combo])
	{
		DrawImage(atk_pos_x, atk_pos_y, player.radius * 10.f, player.radius * 10.f, atk_rot, AtkHB);
	}

	// not idling :> not sure if return will be useful
	if (attack.timer.current < ATTACK_DURATION) 
	{
		return true;
	}
	return false;
}

void Init_Player() 
{
	player.pos = { 0,0 };
	player.vel = { 0,0 };
	player.speed = 500.f;
	player.hp = 100.f;
	player.radius = 50.f;
	player.dodgetimer.current = 1.f;
	player.dodgetimer.end = 1.f;
	player.color = SetColor(0xFF00008B);

	AtkHB = AEGfxTextureLoad("Assets/hitboxtest.png");

	attack.timer.current = 1.f;
	attack.timer.end = 1.f;
	attack.combo = 0;

	sword = Init_SpriteSheet("Assets/slash.png", 4, 10, ATTACK_DURATION);

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
	DrawCircle(player.pos.x, player.pos.y, player.radius, player.color);


	sword.current_col = static_cast<int>(sword.timer.current * sword.cols);
	sword.current_row = attack.combo;
	Draw_SpriteSheet(sword, player.pos.x, player.pos.y, player.radius * 10.f, player.radius * 10.f, sword.uv_width * sword.current_col, sword.uv_height * sword.current_row, atk_rot);
}