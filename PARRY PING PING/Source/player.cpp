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
const float ATTACK_COOLDOWN = 0.5f;
const float ATTACK_DURATION = 0.3f;

f32 atk_pos_x, atk_pos_y, atk_rot;

// Attack function
bool Attacking() 
{

	atk_pos_x = player.pos.x;
	atk_pos_y = player.pos.y;
	{
		s32 mouse_x, mouse_y;
		AEInputGetCursorPosition(&mouse_x, &mouse_y);
		f32 x = (f32)mouse_x - atk_pos_x - (AEGfxGetWindowWidth() / 2.f);
		f32 y = -(f32)mouse_y - atk_pos_y + (AEGfxGetWindowHeight() / 2.f);
		atk_rot = atan2f(y, x) - PI / 2.f;
	}
	if (!Update_Timer(&attack.timer, 1.f) && attack.combo) 
	{
		attack.combo = 0;
		std::cout << "combo reset\n";
	}
	if (AEInputCheckTriggered(AEVK_LBUTTON) && (attack.timer.current > ATTACK_COOLDOWN) && (attack.combo < 3)) 
	{
		attack.timer.current = 0.f;
		attack.combo++;
		std::cout << "attack " << attack.combo << '\n';
	}

	if (attack.timer.current < ATTACK_DURATION) 
	{
		DrawImage(atk_pos_x, atk_pos_y, player.radius * 10.f , player.radius * 10.f, atk_rot, AtkHB);
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


	sword = Init_SpriteSheet("Assets/slash.png", 1, 7, ATTACK_DURATION);

}

void Update_Player() 
{
	Movement();
	if (Attacking()) {
		std::cout << "yes";
	}
	Loop_Timer(&sword.timer, sword.duration);
	sword.current_col = static_cast<int>(sword.timer.current * sword.cols);
	sword.current_row = 0;
	Draw_SpriteSheet(sword,player.pos.x,player.pos.y,player.radius*10.f,player.radius*10.f,sword.uv_width*sword.current_col,sword.uv_height*sword.current_row,atk_rot);
}

// Draw Player
void Draw_Player() 
{
	DrawCircle(player.pos.x, player.pos.y, player.radius, player.color);
	
}