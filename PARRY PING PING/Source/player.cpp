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
#include <vector>
#include <iostream>

typedef struct Attack
{
	Timer timer;
	int combo;
} Attack;

typedef struct Projectile 
{
	AEVec2 pos;           // Position of the projectile
	AEVec2 vel;           // Velocity (direction and speed)
	//AEGfxTexture* texture; // Texture for the projectile
	f32 speed;            // Speed of the projectile
	f32 angle;
	Timer lifetimeTimer;
	bool isActive;
	bool isHeavy;
} Projectile;

std::vector<Projectile> projectiles; // List to hold active projectiles

Player player;
Attack melee_attack;
SpriteSheet sword;
SpriteSheet2 sword_test;

// ATTACK LOGIC
f32 atk_pos_x, atk_pos_y, atk_rot;
AEVec2 atk_dir;

// MELEE ATTACK
// =======================================================================
#pragma region MELEE
// Melee Attack hitbox and durations in seconds

AEGfxTexture* AtkHB;

const float ATTACK_COOLDOWN[4] = { 0.f, 0.5f, 0.7f, 0.7f };
const float ATTACK_DURATION = 1.f;
const float ATTACK_START[4] = { 0.f, 0.2f, 0.2f, 0.3f };
const float ATTACK_END[4] = { 0.f, 0.4f, 0.4f, 0.7f };
const float ATTACK_TOTAL[4] = { 1.f,1.f,1.f,1.f };

const float ATTACK_ANGLE[2] = { PI / 4.f, PI / 3.f };
const float ATTACK_RADIUS[4] = { 250.f,250.f,250.f,250.f };

bool hitbox_active = false;

enum ATTACK_PATTERN
{
	ATTACK_IDLE,	// IDLE :D
	ATTACK_1,		// ATTACK 1 (right swing)
	ATTACK_2,		// ATTACK 2 (left swing)
	ATTACK_3,		// ATTACK 3 (360)
};


bool Attacking_Melee()
{
	// if timer is fully done updating and not idling
	if (!Update_Timer(&melee_attack.timer) && melee_attack.combo)
	{
		// reset the combo to idle
		melee_attack.combo = ATTACK_IDLE;

		sword_test.current_row = 0;
	}

	// check left click input
	// check if melee_attack cooldown over + combo not finished
	if (AEInputCheckTriggered(AEVK_LBUTTON) && (melee_attack.timer.current > ATTACK_COOLDOWN[melee_attack.combo]) && (melee_attack.combo < 3))
	{
		AEVec2Set(&atk_dir, Screen_To_World_x(Mouse.x) - atk_pos_x, Screen_To_World_y(Mouse.y) - atk_pos_y);
		AEVec2Normalize(&atk_dir, &atk_dir);
		player.state = MELEE;
		melee_attack.timer.current = 0.f;
		melee_attack.combo++;
		//std::cout << "melee_attack " << melee_attack.combo << '\n';
		sword.timer.current = 0.f;

		u32 col_start = 0;
		u32 row_len = 0;
		f32 duration = ATTACK_TOTAL[melee_attack.combo];
		switch (melee_attack.combo)
		{
		case 0:
			sword_test.current_row = melee_attack.combo;
			col_start = 0;
			row_len = 10;
			break;
		case 1:
			sword_test.current_row = melee_attack.combo;
			col_start = 0;
			row_len = 10;
			break;
		case 2:
			sword_test.current_row = melee_attack.combo;
			col_start = 0;
			row_len = 10;
			break;
		case 3:
			sword_test.current_row = melee_attack.combo;
			col_start = 0;
			row_len = 10;
			break;
		default:
			break;
		}
		Set_SpriteSheet(&sword_test, col_start, row_len, duration);
	}

	// update sword swing timer
	Update_Timer(&sword.timer);

	hitbox_active = false;
	// logic for if hitbox should be active, to be replaced with a bool return probably @raziq
	if (melee_attack.timer.current >= ATTACK_START[melee_attack.combo] && melee_attack.timer.current <= ATTACK_END[melee_attack.combo] && player.state == MELEE)
	{
		DrawImage(World_To_Screen_x(atk_pos_x), World_To_Screen_y(atk_pos_y), player.radius * 10.f, player.radius * 10.f, atk_rot, AtkHB);
		hitbox_active = true;
	}
	else if (melee_attack.timer.current >= ATTACK_START[melee_attack.combo] && player.state == MELEE)
	{
		player.state = IDLE;
	}
	/*std::cout << "States:\n0 - IDLE\n1 - MOVING\n2 - MELEE\n3 - RANGE\n4 - HEAVY\n5 - DODGE\n6 - HIT\n7 - DEAD\n";
	std::cout << "Playerstate = " << player.state << "\n\n";
	std::cout << "Atk_Dir x: " << atk_dir.x << " | Atk_Dir y: " << atk_dir.y << "\n\n";*/

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
				//std::cout << "HIT ATTACK " << melee_attack.combo << '\n';
				return melee_attack.combo;
			}
			break;
		}
		case 3:
		{
			// No need to check direction for third swing as it is 360 circle
			//std::cout << "HIT ATTACK " << melee_attack.combo << '\n';
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
#pragma endregion
// =======================================================================

// RANGED ATTACK
// =======================================================================
#pragma region RANGED

AEGfxTexture* projectile_texture;

AEGfxTexture* heavy_texture;

Projectile heavy_atk;

int ammo = 3;

void Attacking_Ranged()
{
	if (AEInputCheckTriggered(AEVK_R) && ammo < 3)  //Reloading of Ammo and ensures does not exceed more than 3.
	{
		ammo += 1;
	}

	if (ammo > 0) 
	{

		if (ammo == 3 && AEInputCheckTriggered(AEVK_E)) 
		{
			f32 cursor_x = Screen_To_World_x(Mouse.x);
			f32 cursor_y = Screen_To_World_y(Mouse.y);

			AEVec2 dir = { cursor_x - player.pos.x, cursor_y - player.pos.y };
			AEVec2Normalize(&dir, &dir);


			heavy_atk.pos = player.pos;
			heavy_atk.vel = dir;
			heavy_atk.speed = 0.f; // Adjust speed as needed
			heavy_atk.angle = atan2f(cursor_y - player.pos.y, cursor_x - player.pos.x);
			heavy_atk.isActive = true;
			heavy_atk.isHeavy = true;
			heavy_atk.lifetimeTimer.current = 0.f;

			projectiles.push_back(heavy_atk);
			ammo = 0; // Deplete ammo after firing
		}

		if (ammo != 0 && AEInputCheckTriggered(AEVK_RBUTTON)) 
		{
			// Calculate direction and angle to the cursor
			f32 cursor_x = Screen_To_World_x(Mouse.x);
			f32 cursor_y = Screen_To_World_y(Mouse.y);

			AEVec2 dir = { cursor_x - player.pos.x, cursor_y - player.pos.y };
			AEVec2Normalize(&dir, &dir);

			// Create a single projectile
			Projectile new_proj;
			new_proj.pos = player.pos; // Start at player's position
			new_proj.vel = dir;        // Set direction
			new_proj.speed = 250.f;    // Speed of the projectile
			new_proj.isActive = true;
			new_proj.isHeavy = false;
			new_proj.lifetimeTimer = Create_Timer(10.0f);
			new_proj.angle = atan2f(cursor_y - player.pos.y, cursor_x - player.pos.x);

			// Add the single projectile to the vector
			projectiles.push_back(new_proj);
			ammo -= 1;
		}

	}

	for (auto& proj : projectiles) 
	{
		// Update position
		AEVec2 scaledVel;
		AEVec2Scale(&scaledVel, &proj.vel, proj.speed * dt);
		AEVec2Add(&proj.pos, &proj.pos, &scaledVel);

		// Draw the projectile
		DrawImage(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y),
			20.f, 20.f, proj.angle, projectile_texture);
	}

	// Update lifetime timer
	bool lifetimeExpired = Update_Timer(&heavy_atk.lifetimeTimer);

	// Debugging print to see if the timer is updating
	//std::cout << "Timer: " << heavy_atk.lifetimeTimer.current << " seconds left.\n";

	if (!lifetimeExpired) 
	{
		//std::cout << "Projectile lifetime expired, deactivating.\n";
		heavy_atk.isActive = false;  // Mark the projectile as inactive
	}

	if (!heavy_atk.isActive) 
	{
		//std::cout << "Removing inactive projectile.\n";
		//heavy_atk = projectiles.erase(heavy_atk);  // Remove inactive projectile from the list
	}
}
#pragma endregion
// =======================================================================

// Attack function
void Attacking()
{
	// maybe not needed? placeholder i guess
	atk_pos_x = player.pos.x;
	atk_pos_y = player.pos.y;

	// update for player to face mouse
	{
		f32 x = Screen_To_World_x(Mouse.x) - atk_pos_x;
		f32 y = Screen_To_World_y(Mouse.y) - atk_pos_y;
		atk_rot = atan2f(y, x) - PI / 2.f;
	}

	Attacking_Melee();

	Attacking_Ranged();
}





const float DASH_COOLDOWN = 1.5f;
const float DASH_DURATION = 0.2f;

// WASD movement
void Movement() {

	// player movement default 0
	player.vel = { 0,0 };
	f32 boost = 0.f;
	bool moving = false;

	// change when key pressed
	if (player.state != HEAVY)
	{
		if (player.state != MELEE)
		{
			if (AEInputCheckCurr(AEVK_W))
			{
				moving = true;
				player.vel.y += 1.f;
			}
			if (AEInputCheckCurr(AEVK_A))
			{
				moving = true;
				player.vel.x -= 1.f;
			}
			if (AEInputCheckCurr(AEVK_S))
			{
				moving = true;
				player.vel.y -= 1.f;
			}
			if (AEInputCheckCurr(AEVK_D))
			{
				moving = true;
				player.vel.x += 1.f;
			}
			if (moving)
			{
				player.state = MOVING;
			}
			else
			{
				player.state = IDLE;
			}
		}
		else
		{
			float atk_vel_scale = -(melee_attack.timer.current * melee_attack.timer.current) + ATTACK_END[melee_attack.combo] * melee_attack.timer.current;

			AEVec2Scale(&player.vel, &atk_dir, player.speed * atk_vel_scale * dt);
		}
	}

	if (!Update_Timer(&player.dodgetimer))
	{
		if (AEInputCheckTriggered(AEVK_SPACE))
		{
			player.state = DODGE;
			player.dodgetimer.current = 0.f;
		}
	}
	if (player.dodgetimer.current * DASH_COOLDOWN <= DASH_DURATION)
	{
		boost = 1.2f * player.speed;
	}

	if (player.vel.x || player.vel.y)
	{
		if (player.state == MOVING)
		{
			AEVec2Normalize(&player.vel, &player.vel);
		}
		AEVec2Scale(&player.vel, &player.vel, (player.speed + boost) * dt);
	}

	// position update
	AEVec2Add(&player.pos, &player.vel, &player.pos);
}






void Load_Player()
{
	AtkHB = AEGfxTextureLoad("Assets/hitboxtest.png");

	sword = Init_SpriteSheet("Assets/slash.png", 4, 10, ATTACK_DURATION);

	sword_test = Init_SpriteSheet2("Assets/slash.png", 4, 10);

	projectile_texture = AEGfxTextureLoad("Assets/LightRangeAttack.png");
	if (!projectile_texture) {
		std::cerr << "Failed to load projectile texture!" << std::endl;
	}

	heavy_texture = AEGfxTextureLoad("Assets/HeavyRangeAttack.png");
	if (!heavy_texture) {
		std::cerr << "Failed to load projectile texture!" << std::endl;
	}
}

void Init_Player()
{
	player.pos = { 0,0 };
	player.vel = { 0,0 };
	player.speed = 500.f;
	player.hp = 100.f;
	player.radius = 25.f;
	player.dodgetimer = Create_Timer(DASH_COOLDOWN);
	player.color = SetColor(0xFF00008B);
	player.state = IDLE;
	player.direction_v = DOWN;
	player.direction_h = RIGHT;

	melee_attack.timer = Create_Timer(1.f);
	melee_attack.combo = 0;

	AEVec2Zero(&atk_dir);

	heavy_atk;
	heavy_atk.pos = player.pos;
	heavy_atk.vel = { 0.f, 0.f };
	heavy_atk.speed = 150.f; // Adjust speed as needed
	heavy_atk.angle = { 0.f };
	heavy_atk.isActive = true;
	heavy_atk.isHeavy = true;
	heavy_atk.lifetimeTimer = Create_Timer(2.0f);

	Init_UI(ammo, 3, player.hp, 100.f);
}

void Update_Player()
{
	if (player.state != DEAD)
	{
		Movement();

		Attacking();
		
	}
	Update_UI(ammo, player.hp);
}

// Draw Player
void Draw_Player()
{
	DrawCircle(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius, player.color);

	sword.current_col = static_cast<int>(sword.timer.current * sword.cols);
	sword.current_row = melee_attack.combo;
	//Draw_SpriteSheet(sword, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius * 10.f, player.radius * 10.f, sword.uv_width * sword.current_col, sword.uv_height * sword.current_row, atk_rot);

	Draw_SpriteSheet2(&sword_test, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius * 10.f, player.radius * 10.f, atk_rot);


// Draw projectiles
	for (const auto& proj : projectiles)
	{
		if (projectile_texture != nullptr)
		{ // Ensure texture is loaded
			// Use different size depending on whether the projectile is heavy or not
			if (proj.isHeavy) 
			{
				if (heavy_atk.isActive)
				{
					DrawImage(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 1000.f, 1000.f, proj.angle, heavy_texture); // Heavy projectile
				}
				else {
					projectiles.pop_back();
				}
			}
			else {
				DrawImage(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 100.f, 100.f, proj.angle, projectile_texture); // Light projectile
			}
		}
	}
}

void Free_Player()
{

}

void Unload_Player()
{
	AEGfxTextureUnload(AtkHB);
	AEGfxTextureUnload(sword.texture);
	AEGfxMeshFree(sword.mesh);
	AEGfxTextureUnload(projectile_texture);
	AEGfxTextureUnload(heavy_texture);
	AEGfxTextureUnload(sword_test.texture);
}