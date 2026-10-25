//---------------------------------------------------------
// File:	player.cpp
// Project: Heavy Assault : Rust & Dust
// Author:	Tan Jun Quan ------------------------------ 55%
//			- Movement
//			- Melee attacks
//			- Dashing
//			- Parrying
//			- Sprites
//			- File IO polish
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 30%
//			- Ranged attacks (light and heavy)
//			- Audio
// 
//			Manfred Sim ------------------------------- 10%
//			- File IO
//			- Score system
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 5%
//			- Damage based on difficulty modifier
//			- Rage meter buff
//
// Brief:	Implementation of the player.
//			Contains the logic for behaviour and systems
//			involving the player, including:
//			- Movement + Dodging
//			- Attacks
//			- Blocking + Parrying
//			- Rage buff from successful parries
//			- Score system
// 
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "player.h"
#include "Enemies.h"
#include "SpriteSheet.h"
#include "GameStateManager.h"
#include <vector>
#include <algorithm>
#include <cmath>

// Rage and modifiers
const float ATTACK_RAGEMULTIPLER = 0.75f;
const float MODIFIER_DAMAGEMULTIPLIER_DECREASED = 0.5f;
const float MODIFIER_DAMAGEMULTIPLIER_INCREASED = 2.f;

// Define level score update values
int SCORE_LIGHT_PROJECTILE_HIT;
int SCORE_HEAVY_LASER_HIT;
int SCORE_PLAYER_EAT_HIT;
int SCORE_PLAYER_LOSE;
int SCORE_LEVEL2_MULTIPLIER;

// PLAYER VALUES
float
PLAYER_SPAWN_X, PLAYER_SPAWN_Y,
PLAYER_SPEED,
PLAYER_MAXHP,
PLAYER_RADIUS,
PLAYER_WIDTH, PLAYER_HEIGHT,
PLAYER_DRAW_WIDTH, PLAYER_DRAW_HEIGHT,
PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT,
PLAYER_DASH_COOLDOWN, PLAYER_DASH_DURATION,
PLAYER_HIT_DURATION,
PLAYER_DEATH_DURATION;

int
PLAYER_MAX_RAGE,
PLAYER_MAX_AMMO;

typedef struct Projectile
{
	AEVec2 pos;           // Position of the projectile
	AEVec2 vel;           // Velocity (direction and speed)
	f32 speed;            // Speed of the projectile
	f32 angle;            // Angle of the projectile's trajectory
	Timer lifetimeTimer;  // Timer to track the lifetime of the projectile
	bool isActive;        // Is the projectile currently active
	bool isHeavy;         // Whether the projectile is a heavy attack (rectangular hitbox)
	f32 radius;           // Radius of the circular hitbox (for light projectiles)
	f32 width;            // Width of the hitbox (for heavy projectiles)
	f32 height;           // Height of the hitbox (for heavy projectiles)
	f32 range;            // Maximum range for the laser
} Projectile;


std::vector<Projectile> projectiles; // List to hold active projectiles

// File IO
std::vector<float> Player_IO;
std::vector<float> PlayerAttack_IO;
std::vector<float> PlayerScore_IO;

Player player;
Attack melee_attack;
Timer ranged_attack;
Timer heavy_attack;

SpriteSheet player_sprite;
SpriteSheet laser_sprite;

// ATTACK LOGIC
f32 atk_pos_x, atk_pos_y, atk_rot;
AEVec2 atk_dir;

static float rightClickTimer = 0.f;
const float rightClickHoldDuration = 0.25f; // 1 second hold to trigger heavy attack

// MELEE ATTACK
// =======================================================================
#pragma region MELEE
// Melee Attack hitbox and durations in seconds

std::vector<float> ATTACK_COOLDOWN;
float ATTACK_DURATION;
std::vector<float> ATTACK_START;
std::vector<float> ATTACK_END;
std::vector<float> ATTACK_START_ACTIVE;
std::vector<float> ATTACK_END_ACTIVE;
std::vector<float> ATTACK_TOTAL;
float ATTACK_COMBO_WINDOW;
std::vector<float> ATTACK_OFFSET;
std::vector<float> ATTACK_WIDTH;
std::vector<float> ATTACK_HEIGHT;
Timer combo_window;
std::vector<float> ATTACK_ANGLE;
std::vector<float> ATTACK_RADIUS;
std::vector<float> ATTACK_DAMAGE;

aabb melee_hitbox;

bool hitbox_active = false;

// Heavy laser tick timer (for combo calcs, +1 combo ever tick)
Timer laserTickTimer;

enum ATTACK_PATTERN
{
	ATTACK_IDLE,	// IDLE :D
	ATTACK_1,		// ATTACK 1 (right swing)
	ATTACK_2,		// ATTACK 2 (left swing)
	ATTACK_3,		// ATTACK 3 (360)
};


// Flag to track if the sound for each combo has already been played
bool sound_played[3] = { false, false, false }; // Three phases: 1, 2, 3

void ResetSound()
{
	// Reset all sound flags after a combo
	for (int i = 0; i < 3; i++)
	{
		sound_played[i] = false; // Reset all phases to not played
	}
}

bool Attacking_Melee()
{
	// if timer is fully done updating and not idling
	if (!Update_Timer(&melee_attack.timer) && melee_attack.combo == 3 || !Update_Timer(&combo_window))
	{
		// reset the combo to idle
		melee_attack.combo = ATTACK_IDLE;
		ResetSound(); // Reset sounds after combo ends
	}

	Update_Timer(&melee_attack.active_time);
	// check left click input
	// check if melee_attack cooldown over + combo not finished
	if (AEInputCheckTriggered(AEVK_LBUTTON) && (melee_attack.timer.current > ATTACK_COOLDOWN[melee_attack.combo]) && (melee_attack.combo < 3) && player.canAttack)
	{
		AEVec2Set(&atk_dir, Screen_To_World_x(Mouse.x) - atk_pos_x, Screen_To_World_y(Mouse.y) - atk_pos_y);
		AEVec2Normalize(&atk_dir, &atk_dir);
		player.state = MELEE;
		melee_attack.timer.current = 0.f;
		melee_attack.combo++;

		melee_attack.active_time.duration = ATTACK_TOTAL[melee_attack.combo];
		melee_attack.active_time.current = 0.f;

		combo_window.current = 0.f;
		combo_window.duration = ATTACK_END[melee_attack.combo] + ATTACK_COMBO_WINDOW;

		ranged_attack.current = 1.f;
		heavy_attack.current = 1.f;
		player.lightAttackCooldown = 0.f;
	}

	hitbox_active = false;
	// Check if the combo is active and within the active time
	if (Update_Timer(&melee_attack.active_time) && melee_attack.active_time.current * ATTACK_END_ACTIVE[melee_attack.combo] >= ATTACK_START_ACTIVE[melee_attack.combo])
	{
		hitbox_active = true;
		melee_hitbox = Init_AABB(World_To_Screen_x(player.pos.x - 50.f), World_To_Screen_y(player.pos.y), ATTACK_WIDTH[melee_attack.combo], ATTACK_HEIGHT[melee_attack.combo]);
	}
	else if (melee_attack.timer.current >= ATTACK_END[melee_attack.combo] && player.state == MELEE)
	{
		player.state = IDLE;
		ResetSound(); // Reset sounds if the attack phase ends
	}

	// not idling
	if (melee_attack.timer.current < ATTACK_DURATION)
	{
		return true;
	}

	return false;
}

void PlayAttackSound(int combo)
{
	// Play sound only if it hasn't been played already for this combo
	if (!sound_played[combo - 1]) // Combo is 1, 2, or 3
	{
		switch (combo)
		{
		case 1:
			AEAudioPlay(sword1, gameSFX, 1.0f, 1.0f, 0); // First slash
			break;
		case 2:
			AEAudioPlay(sword2, gameSFX, 1.0f, 1.0f, 0); // Second slash
			break;
		case 3:
			AEAudioPlay(sword3, gameSFX, 1.0f, 1.0f, 0); // Third slash
			break;
		default:
			break;
		}

		sound_played[combo - 1] = true; // Mark sound as played for this combo
	}
}

int Attacked_Melee(AEVec2 target)
{
	if (hitbox_active && melee_attack.combo)
	{
		// Play sound only when a valid hit occurs and the combo phase hasn't been reset
		if (!sound_played[melee_attack.combo - 1])
		{
			PlayAttackSound(melee_attack.combo); // Trigger sound only once per combo phase
			sound_played[melee_attack.combo - 1] = true; // Mark this combo phase as played
		}

		if (AETestCircleToRect(&target, 250.f, &player.pos, ATTACK_WIDTH[melee_attack.combo], ATTACK_HEIGHT[melee_attack.combo]))
		{
			return melee_attack.combo;
		}
	}
	return 0;
}

bool Attacked_Melee_rect(AEVec2 target, f32 width, f32 height, int* check_hit, float* damage)
{
	if (hitbox_active && melee_attack.combo && (AETestCircleToRect(&player.pos, ATTACK_RADIUS[melee_attack.combo], &target, width, height)))
	{

		if (AETestRectToRect(&player.pos, ATTACK_WIDTH[melee_attack.combo], ATTACK_HEIGHT[melee_attack.combo], &target, width, height))
		{
			*check_hit = melee_attack.combo;

			if (melee_attack.combo == 3) 
			{
				*damage = ATTACK_DAMAGE[melee_attack.combo] + (ATTACK_DAMAGE[melee_attack.combo] * (float(player.ragemeter) / 5.f) * ATTACK_RAGEMULTIPLER * 0.5f);		// 3rd slash was too strong
			}
			else
			{
				*damage = ATTACK_DAMAGE[melee_attack.combo] + (ATTACK_DAMAGE[melee_attack.combo] * (float(player.ragemeter) / 5.f) * ATTACK_RAGEMULTIPLER);
			}

			return true;
		}
		else if (!melee_attack.combo)
		{
			*check_hit = 0;
		}
		return false;
	}
	return false;
}



#pragma endregion
// =======================================================================


// RANGED ATTACK
// =======================================================================
#pragma region RANGED

AEGfxTexture* projectile_texture;

Projectile heavy_atk;
Projectile new_proj;

// AABB hitbox for the heavy attack (laser)
aabb heavyAtkHitbox;

// Calculate the dot product of two vectors
float DotProduct(const AEVec2& v1, const AEVec2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

// Subtract two vectors
AEVec2 Subtract(const AEVec2& v1, const AEVec2& v2) {
	return { v1.x - v2.x, v1.y - v2.y };
}

// Get the length of a vector
float Length(const AEVec2& v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

// Get the closest point on a line segment from a point (the center of the circle)
AEVec2 ClosestPointOnSegment(const AEVec2& A, const AEVec2& B, const AEVec2& C) {
	AEVec2 AB = Subtract(B, A);
	AEVec2 AC = Subtract(C, A);

	float AB_AB = DotProduct(AB, AB); // Length squared of AB
	float AB_AC = DotProduct(AB, AC); // Dot product of AB and AC

	// The scalar projection of AC onto AB
	float t = AB_AC / AB_AB;

	// Clamp t to the range [0, 1] to ensure it falls within the line segment
	t = fmaxf(0.0f, fminf(1.0f, t));

	// Calculate the closest point on the segment
	return { A.x + t * AB.x, A.y + t * AB.y };
}

// Function to check if the laser (line segment) collides with the boss (circle)
bool LineSegmentCircleCollision(const AEVec2& A, const AEVec2& B, const AEVec2& circleCenter, float radius) {
	// Find the closest point on the segment to the circle's center
	AEVec2 closestPoint = ClosestPointOnSegment(A, B, circleCenter);

	// Calculate the distance from the closest point to the circle's center
	float distX = closestPoint.x - circleCenter.x;
	float distY = closestPoint.y - circleCenter.y;
	float distance = sqrtf(distX * distX + distY * distY);

	// Check if the distance is less than or equal to the radius
	return distance <= radius;
}

// Function to check if any projectile collides with the boss's hitbox using the Alpha Engine function
bool Attacking_Ranged_Impact(AEVec2 target, f32 width, f32 height, float* damage)
{
	bool hit = false; // Track if at least one projectile hits

	for (auto& proj : projectiles) {
		// Only check active projectiles
		if (!proj.isActive)
			continue;

		// **Update position (only for light projectiles)**
		if (!proj.isHeavy) {
			AEVec2 scaledVel;
			AEVec2Scale(&scaledVel, &proj.vel, proj.speed * dt);
			AEVec2Add(&proj.pos, &proj.pos, &scaledVel);
		}
		else {
			// Heavy attack logic
			float offsetDistance = 100.0f;
			float offsetX = cosf(proj.angle) * offsetDistance;
			float offsetY = sinf(proj.angle) * offsetDistance;

			// Apply the offset to the projectile's position (keep it in world space)
			AEVec2 heavyAttackPos = { proj.pos.x + offsetX, proj.pos.y + offsetY };

			// Direction of the heavy attack based on the angle (scaled by the attack's size)
			float offsetX2 = cosf(proj.angle) * 1000.f;  // Use height to define the attack range (or use width)
			float offsetY2 = sinf(proj.angle) * 1000.f;  // Same for Y

			// Apply the offset to the projectile's position to get the endpoint of the laser
			AEVec2 heavyAttackPosEnd = { heavyAttackPos.x + offsetX2, heavyAttackPos.y + offsetY2 };

			// Create a line segment for the laser (start from heavyAttackPos, end at calculated end point)
			AELineSegment2 lineSegment;
			lineSegment.mP0 = heavyAttackPos;      // Start of the line segment (laser starts at heavyAttackPos)
			lineSegment.mP1 = heavyAttackPosEnd;   // End of the line segment (laser moves based on attack size and direction)

			// Check if the laser intersects with the boss's circle (represented by the target and radius)
			if (LineSegmentCircleCollision(heavyAttackPos, heavyAttackPosEnd, target, height / 2)) {
				*damage = 10.f * dt;  // Heavy attack damage
				hit = true;
				Loop_Timer(&laserTickTimer);
				if (laserTickTimer.current >= laserTickTimer.duration)
				{
					currentCombo += 1;
					Collect_Points(SCORE_HEAVY_LASER_HIT, BIG_HIT_SCORE);
					laserTickTimer.current = 0.f;
				}
			}
		}
		// **Light projectile collision check** (No change here, you can keep this as is)
		if (!proj.isHeavy && AETestCircleToRect(&proj.pos, proj.radius, &target, width, height)) {
			*damage = 5.f;  // Light attack damage
			hit = true;
			proj.isActive = false; // Deactivate light projectile after hit
			currentCombo += 1;
			Collect_Points(SCORE_LIGHT_PROJECTILE_HIT, HIT_SCORE); // Update score when light projectile hits
		}
	}

	return hit; // Return true if at least one projectile hits
}





typedef enum shoottrig {
	RANGED_BLANK,
	RANGED_LIGHT,
	RANGED_HEAVY
}shoottrig;


shoottrig trigger = RANGED_BLANK;	// 0 - No shoot
									// 1 - Light
									// 2 - Heavy

float 
RANGED_ATTACK_DURATION, 
RANGED_ATTACK_DELAY, 
RANGED_ATTACK_LIFETIME,
HEAVY_ATTACK_DURATION, 
HEAVY_ATTACK_DELAY,
HEAVY_ATTACK_LIFETIME,
HEAVY_ATTACK_TICK;


void Attacking_Ranged()
{

	if (player.ammo == 3 && player.prevAmmo != 3)
	{
		// Play a sound effect when ammo reaches 3
		AEAudioPlay(ammoFullSFX, gameSFX, 1.0f, 1.0f, 0); // Replace 'ammoFullSFX' with your actual sound ID
	}

	// Update previous ammo for next frame
	player.prevAmmo = player.ammo;

	if (player.ammo > 0)
	{
		if (AEInputCheckCurr(AEVK_RBUTTON) && player.lightAttackCooldown <= 0.f && player.state != HEAVY && player.canAttack)
		{
			rightClickTimer += dt;
		}

		if (player.ammo == 3 && rightClickTimer >= rightClickHoldDuration && player.canAttack)
		{
			trigger = RANGED_HEAVY;
			player.state = HEAVY;
			heavy_attack.current = 0.f;
			rightClickTimer = 0.f;
			player.lightAttackCooldown = 1.0f;

			melee_attack.active_time.current = 1.f;
			melee_attack.timer.current = 1.f;

			// Initialize laser tick timer for combo
			laserTickTimer.current = 0.f;

			// **Play the light projectile sound effect**
			AEAudioPlay(heavyProj, gameSFX, 1.0f, 1.0f, 0);
		}

		if (AEInputCheckReleased(AEVK_RBUTTON) && rightClickTimer && player.lightAttackCooldown<=0.f && player.canAttack)
		{
			
			trigger = RANGED_LIGHT;
			player.state = RANGE;
			ranged_attack.current = 0.f;
			
			rightClickTimer = 0.f;
			player.lightAttackCooldown = 1.0f;

			melee_attack.active_time.current = 1.f;
			melee_attack.timer.current = 1.f;

			// **Play the light projectile sound effect**
			AEAudioPlay(lightProj, gameSFX, 1.0f, 1.0f, 0);
		}


	}

	if (Update_Timer(&ranged_attack))
	{
		player.state = RANGE;
		if (ranged_attack.current * ranged_attack.duration > RANGED_ATTACK_DELAY && trigger)
		{
			trigger = RANGED_BLANK;
			AEVec2Set(&atk_dir, Screen_To_World_x(Mouse.x) - atk_pos_x, Screen_To_World_y(Mouse.y) - atk_pos_y);
			AEVec2Normalize(&atk_dir, &atk_dir);

			f32 cursor_x = Screen_To_World_x(Mouse.x);
			f32 cursor_y = Screen_To_World_y(Mouse.y);

			AEVec2 dir = { cursor_x - player.pos.x, cursor_y - player.pos.y };
			AEVec2Normalize(&dir, &dir);

			// Create projectile
			new_proj.pos = player.pos;
			new_proj.vel = dir;
			new_proj.speed = 250.f;
			new_proj.isActive = true;
			new_proj.isHeavy = false;
			new_proj.lifetimeTimer.current = 0.f;
			new_proj.angle = atan2f(cursor_y - player.pos.y, cursor_x - player.pos.x);
			new_proj.radius = 20.f;

			projectiles.push_back(new_proj);
			player.ammo -= 1;

			// Apply knockback
			float knockbackStrength = 50.f;
			AEVec2 knockbackDirection = { -dir.x, -dir.y };
			AEVec2Scale(&player.vel, &knockbackDirection, knockbackStrength);
			player.knockbackTimer = 0.1f;
		}
	}
	else if (Update_Timer(&heavy_attack))
	{
		player.state = HEAVY;
		if (heavy_attack.current * heavy_attack.duration > HEAVY_ATTACK_DELAY && trigger)
		{
			trigger = RANGED_BLANK;
			AEVec2Set(&atk_dir, Screen_To_World_x(Mouse.x) - atk_pos_x, Screen_To_World_y(Mouse.y) - atk_pos_y);
			AEVec2Normalize(&atk_dir, &atk_dir);

			// If the right mouse button has been held for 1 second, fire the heavy attack

			f32 cursor_x = Screen_To_World_x(Mouse.x);
			f32 cursor_y = Screen_To_World_y(Mouse.y);

			// Calculate the direction vector from the player to the cursor
			AEVec2 dir = { cursor_x - player.pos.x, cursor_y - player.pos.y };
			AEVec2Normalize(&dir, &dir);  // Normalize the direction vector

			// Apply an offset to position the heavy attack slightly ahead of the player
			AEVec2 offset = { dir.x * 1.5f, dir.y * 1.5f }; // Adjust multiplier as needed to control distance ahead

			// Set the position of the heavy attack slightly ahead of the player based on the offset
			heavy_atk.pos.x = player.pos.x + offset.x;
			heavy_atk.pos.y = player.pos.y + offset.y;

			// Set the dimensions and other properties of the heavy attack (laser)
			heavy_atk.width = 100.f;  // Width for the laser beam
			heavy_atk.height = 500.f; // Height represents the laser length
			heavy_atk.vel = dir;  // Direction of the laser
			heavy_atk.speed = 0.f; // The laser doesn't move, just extends in the direction
			heavy_atk.angle = atan2f(cursor_y - player.pos.y, cursor_x - player.pos.x);

			// Activate the heavy attack projectile
			heavy_atk.isActive = true;
			heavy_atk.isHeavy = true;

			// Reset the lifetime timer
			heavy_atk.lifetimeTimer.current = 0.f;

			projectiles.push_back(heavy_atk);  // Add the heavy attack to the projectiles list
			player.ammo = 0; // Deplete ammo after firing

			// Apply knockback to the player (move in opposite direction of attack)
			float knockbackStrength = 200.f; // Adjust for stronger/weaker knockback
			AEVec2 knockbackDirection = { -dir.x, -dir.y }; // Inverse of attack direction
			AEVec2Scale(&player.vel, &knockbackDirection, knockbackStrength); // Apply knockback velocity

			player.knockbackTimer = 0.3f; // Knockback lasts for 0.3 seconds
		}
	}

	// Reduce cooldown timer each frame
	if (player.lightAttackCooldown > 0.f)
	{
		player.lightAttackCooldown -= dt;
	}

 
	// Update lifetime timer
	bool lifetimeExpiredHeavy = Update_Timer(&heavy_atk.lifetimeTimer);
	bool lifetimeExpiredLight = Update_Timer(&new_proj.lifetimeTimer);


	if (!lifetimeExpiredHeavy) 
	{
		heavy_atk.isActive = false;  // Mark the projectile as inactive
	}

	if (!lifetimeExpiredLight)
	{
		new_proj.isActive = false;
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
}


Timer dodge_time;
Timer dodge_cooldown;

// WASD movement
void Movement() {

	// player movement default 0
	player.vel = { 0,0 };
	f32 boost = 0.f;
	bool moving = false;

	// change when key pressed
	if (heavy_attack.current == 1.f)
	{
		if (player.state != MELEE && ranged_attack.current == 1.f)
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
				player.direction_h = LEFT;
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
				player.direction_h = RIGHT;
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
		else if (player.state == RANGE)
		{
			float atk_vel_scale =  -.1f * (1.f - ranged_attack.current);

			AEVec2Scale(&player.vel, &atk_dir, PLAYER_SPEED * atk_vel_scale * dt);
		}
		else // melee
		{
			float atk_vel_scale = .1f*(1.f-melee_attack.active_time.current);
			AEVec2Scale(&player.vel, &atk_dir, PLAYER_SPEED * atk_vel_scale * dt);
		}
	}
	else if (heavy_attack.current * HEAVY_ATTACK_DURATION >= HEAVY_ATTACK_DELAY)
	{

		float hvy_vel_scale = -1.f * (1.f - heavy_attack.current);

		AEVec2Scale(&player.vel, &atk_dir, PLAYER_SPEED * hvy_vel_scale * dt);
	}

	bool dodge_cd = Update_Timer(&dodge_cooldown);
	if (player.vel.x || player.vel.y)
	{
		if (player.state == MOVING)
		{
			AEVec2Normalize(&player.vel, &player.vel);
		}
		AEVec2Scale(&player.vel, &player.vel, PLAYER_SPEED * dt);

		if (!dodge_cd)
		{
			AEVec2 input = { 0,0 };
			if (AEInputCheckCurr(AEVK_W))
			{
				input.y += 1.f;
			}
			if (AEInputCheckCurr(AEVK_A))
			{
				input.x -= 1.f;
				player.direction_h = LEFT;
			}
			if (AEInputCheckCurr(AEVK_S))
			{
				input.y -= 1.f;
			}
			if (AEInputCheckCurr(AEVK_D))
			{
				input.x += 1.f;
				player.direction_h = RIGHT;
			}
			if (AEInputCheckTriggered(AEVK_SPACE) && (input.x || input.y))
			{
				AEAudioPlay(dashAudio, gameSFX, 1.0f, 1.0f, 0); // First slash
				AEVec2Set(&player.dodge_vel, input.x, input.y);
				AEVec2Normalize(&player.dodge_vel, &player.dodge_vel);
				player.state = DODGE;
				dodge_time.current = 0.f;
				dodge_cooldown.current = 0.f;
				//player.dodgetimer.current = 0.f;

				// reset timers for attacks
				melee_attack.active_time.current = 1.f;
				melee_attack.timer.current = 1.f;
				ranged_attack.current = 1.f;
				heavy_attack.current = 1.f;
			}
		}
	}


	if (Update_Timer(&dodge_time))
	{
		player.direction_h = (player.dodge_vel.x >= 0.f) ? RIGHT : LEFT;
		player.state = DODGE;
		boost = (dodge_time.current <= .5f ) ? 1.5f * PLAYER_SPEED : 0.f;
		AEVec2Scale(&player.vel, &player.dodge_vel, (PLAYER_SPEED + boost) * dt);
	}

	// position update
	AEVec2Add(&player.pos, &player.vel, &player.pos);
	Update_AABB(&player.hitbox, player.pos.x, player.pos.y, PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_WIDTH);
}

float PARRY_DURATION;	// PARRY DURATION		0.2s parry
float BLOCK_DURATION;	// PARRY+BLOCK DURATION	0.8s block + 0.2s parry
float BLOCK_COOLDOWN;	// TOTAL BLOCK DURATION	0.2s parry + 0.8s block + 2s cooldown
float BLOCK_REDUCTION;

// BLOCK/PARRY CHECK
void Parrying()
{
	if (Update_Timer(&player.blocktimer) && AEInputCheckCurr(AEVK_E) && player.state != PARRIED)
	{
		if (Screen_To_World_x(Mouse.x) > player.pos.x)
		{
			player.direction_h = RIGHT;
		}
		else
		{
			player.direction_h = LEFT;
		}

		if (player.blocktimer.current < PARRY_DURATION / BLOCK_COOLDOWN)
		{
			player.state = PARRY;

		}
		else if (player.blocktimer.current < BLOCK_DURATION / BLOCK_COOLDOWN)
		{
			player.state = BLOCK;
		}
		else
		{
			player.state = IDLE;
		}
	}
	else if (AEInputCheckReleased(AEVK_E))
	{
		if (player.blocktimer.current < BLOCK_DURATION / BLOCK_COOLDOWN)
		{
			player.blocktimer.current = BLOCK_DURATION / BLOCK_COOLDOWN;
		}
		if (player.state == BLOCK || player.state == BLOCKED || player.state == PARRY || player.state == PARRIED)
		{
			player.state = IDLE;
		}
	}
	else if (AEInputCheckTriggered(AEVK_E))
	{
		player.blocktimer.current = 0.f;
	}
}

// Refunds player ammo and reset block cooldown :>
void Parried_Hit()
{
	player.state = PARRIED;
	player.blocked.current = 0.f;

	if (player.ragemeter >= PLAYER_MAX_RAGE) { player.ragemeter = PLAYER_MAX_RAGE; }
	else player.ragemeter++;

	// reload player ammo
	player.ammo = PLAYER_MAX_AMMO;

	// reset block cooldown
	player.blocktimer.current = 1.f;

	AEAudioPlay(parryFX, gameSFX, 1.0f, 1.0f, 0);
}

void Blocked_Hit(f32 damage)
{
	if (player.state != DEAD)
	{
		player.ragemeter = 0;
		player.state = BLOCKED;
		player.blocked.current = 0.f;

	switch (player.Mod_DmgTakenIndex)
	{
	case 0:
		//Player takes no damage
		break;
	case 1:
		player.hp -= damage * BLOCK_REDUCTION * MODIFIER_DAMAGEMULTIPLIER_DECREASED;
		break;
	default:
		player.hp -= damage * BLOCK_REDUCTION;
		break;
	case 3:
		player.hp -= damage * BLOCK_REDUCTION * MODIFIER_DAMAGEMULTIPLIER_INCREASED;
		break;
	}

		if (currentCombo > highestCombo)
		{
			highestCombo = currentCombo;
		}
		currentCombo = 0;
		if (!player.hp)
		{
			// -10000 points if player is dead
			Collect_Points(SCORE_PLAYER_LOSE, RIP_SCORE);
		}
		else
		{
			Collect_Points(static_cast<int>(damage * BLOCK_REDUCTION * SCORE_PLAYER_EAT_HIT), BLOCKED_SCORE);
		}
	}
	
}

void Eat_Hit(f32 damage)
{
	if (player.state != DEAD)
	{
		// take dmg
		player.ragemeter = 0;
		player.state = HIT;
		player.hittimer.current = 0.f;

		switch (player.Mod_DmgTakenIndex)
		{
		case 0:
			// Player takes no damage
			break;
		case 1:
			player.hp -= damage * MODIFIER_DAMAGEMULTIPLIER_DECREASED;
			break;
		default:
			player.hp -= damage;
			break;
		case 3:
			player.hp -= damage * MODIFIER_DAMAGEMULTIPLIER_INCREASED;
			break;
		}

		if (currentCombo > highestCombo)
		{
			highestCombo = currentCombo;
		}
		currentCombo = 0;
		if (!player.hp)
		{
			// -10000 points if player is dead
			Collect_Points(SCORE_PLAYER_LOSE, RIP_SCORE);
		}
		else
		{
			Collect_Points(static_cast<int>(damage * SCORE_PLAYER_EAT_HIT), OUCH_SCORE);
		}
	}
	
}


bool Player_FileRead{ false };
bool PlayerAttack_FileRead{ false };


void Load_Player()
{
	// Read File IO

	// PLAYER STATS
	if (!Player_FileRead)
	{
		ReadFromFile("Assets/FileIO/Player.txt", Player_IO);

		PLAYER_SPAWN_X			= Player_IO[0];
		PLAYER_SPAWN_Y			= Player_IO[1];

		PLAYER_SPEED			= Player_IO[2];
		PLAYER_MAXHP			= Player_IO[3];

		PLAYER_RADIUS			= Player_IO[4];

		PLAYER_WIDTH			= Player_IO[5];
		PLAYER_HEIGHT			= Player_IO[6];

		PLAYER_DRAW_WIDTH		= Player_IO[7];
		PLAYER_DRAW_HEIGHT		= Player_IO[8];

		PLAYER_HITBOX_WIDTH		= Player_IO[9];
		PLAYER_HITBOX_HEIGHT	= Player_IO[10];

		PLAYER_MAX_RAGE			= static_cast<int>(Player_IO[11]);
		PLAYER_MAX_AMMO			= static_cast<int>(Player_IO[12]);

		PLAYER_DASH_COOLDOWN	= Player_IO[13];
		PLAYER_DASH_DURATION	= Player_IO[14];

		PLAYER_HIT_DURATION		= Player_IO[15];
		PLAYER_DEATH_DURATION	= Player_IO[16];

		Player_FileRead = true;
		Player_IO.clear();
	}

	// PLAYER ATTACK VARIABLES
	if (!PlayerAttack_FileRead)
	{
		ReadFromFile("Assets/FileIO/PlayerAttack.txt", PlayerAttack_IO);

		ATTACK_COOLDOWN = { PlayerAttack_IO[0], PlayerAttack_IO[1], PlayerAttack_IO[2], PlayerAttack_IO[3] };			// MELEE COOLDOWN		{ 0.f, 0.5f, 0.5f, 0.7f }
		ATTACK_DURATION = PlayerAttack_IO[4];																			// MELEE DURATION		1.s
		ATTACK_START	= { PlayerAttack_IO[5], PlayerAttack_IO[6],  PlayerAttack_IO[7],  PlayerAttack_IO[8] };			// MELEE START			{ 0.f, 0.2f, 0.2f, 0.3f }
		ATTACK_END		= { PlayerAttack_IO[9], PlayerAttack_IO[10], PlayerAttack_IO[11], PlayerAttack_IO[12] };		// MELEE END			{ 0.f, 0.4f, 0.5f, 0.7f }

		ATTACK_START_ACTIVE = { PlayerAttack_IO[5], PlayerAttack_IO[6],  PlayerAttack_IO[7],  PlayerAttack_IO[8] };		// MELEE START ACTIVE	{ 0.f, 0.2f, 0.2f, 0.3f }
		ATTACK_END_ACTIVE	= { PlayerAttack_IO[9], PlayerAttack_IO[10], PlayerAttack_IO[11], PlayerAttack_IO[12] };	// MELEE END ACTIVE		{ 0.f, 0.4f, 0.5f, 0.7f }

		ATTACK_TOTAL	= { PlayerAttack_IO[13], PlayerAttack_IO[14], PlayerAttack_IO[15], PlayerAttack_IO[16] };		// MELEE TOTAL			{ 1.f, 1.f, 1.f, 1.5f }

		ATTACK_COMBO_WINDOW = PlayerAttack_IO[17];																		// MELEE COMBO WINDOW	1.f

		ATTACK_OFFSET	= { PlayerAttack_IO[18], PlayerAttack_IO[19], PlayerAttack_IO[20], PlayerAttack_IO[21] };		// MELEE OFFSET			{ 0.f, 30.f, 40.f, 0.f }

		ATTACK_WIDTH	= { PlayerAttack_IO[22], PlayerAttack_IO[23], PlayerAttack_IO[24], PlayerAttack_IO[25] };		// MELEE WIDTH			{ 0.f, 200.f, 200.f, 280.f }
		ATTACK_HEIGHT	= { PlayerAttack_IO[26], PlayerAttack_IO[27], PlayerAttack_IO[28], PlayerAttack_IO[29] };		// MELEE HEIGHT			{ 0.f, 150.f, 170.f, 180.f }


		ATTACK_ANGLE	= { PI / PlayerAttack_IO[30], PI / PlayerAttack_IO[31] };										// MELEE ANGLE			{ PI / 4.f, PI / 3.f }
		ATTACK_RADIUS	= { PlayerAttack_IO[32], PlayerAttack_IO[32], PlayerAttack_IO[32], PlayerAttack_IO[32] };		// MELEE RADIUS			{ 150.f, 150.f, 150.f, 150.f }

		ATTACK_DAMAGE	= { PlayerAttack_IO[33], PlayerAttack_IO[34], PlayerAttack_IO[35], PlayerAttack_IO[36] };		// MELEE DAMAGE			{ 0.f, 5.f, 5.f, 15.f }

		PARRY_DURATION	= PlayerAttack_IO[37];			// PARRY DURATION			0.3s parry
		BLOCK_DURATION	= PlayerAttack_IO[38];			// PARRY + BLOCK DURATION	1.0s block + 0.2s parry
		BLOCK_COOLDOWN	= PlayerAttack_IO[39];			// TOTAL BLOCK DURATION		0.2s parry + 1.0 block + 3.0s cooldown
		BLOCK_REDUCTION = PlayerAttack_IO[40];			// BLOCK REDUCTION			0.5f

		RANGED_ATTACK_DURATION	= PlayerAttack_IO[41];	// RANGED ATTACK DURATION	0.4s unload ranged attack
		RANGED_ATTACK_DELAY		= PlayerAttack_IO[42];	// RANGED + DELAY DURATION	0.4s unload + 0.1s delay
		HEAVY_ATTACK_DURATION	= PlayerAttack_IO[43];	// HEAVY ATTACK DURATION	1.0s unload heavy attack
		HEAVY_ATTACK_DELAY		= PlayerAttack_IO[44];	// HEAVY + DELAY DURATION	1.0s unload + 0.5s delay

		HEAVY_ATTACK_LIFETIME	= PlayerAttack_IO[45];	// LASER LIFETIME
		RANGED_ATTACK_LIFETIME	= PlayerAttack_IO[46];	// PROJECTILE LIFETIME
		HEAVY_ATTACK_TICK		= PlayerAttack_IO[47];	// TICK INTERVAL FOR LASER

		PlayerAttack_FileRead = true;
		PlayerAttack_IO.clear();
	}

	// SCORE STATS
	ReadFromFile("Assets/FileIO/PlayerScore.txt", PlayerScore_IO);

	SCORE_LIGHT_PROJECTILE_HIT	= static_cast<int>(PlayerScore_IO[0]);				// + 500 points per projectile
	SCORE_HEAVY_LASER_HIT		= static_cast<int>(PlayerScore_IO[1]);				// + 750 points per tick
	SCORE_PLAYER_EAT_HIT		= static_cast<int>(PlayerScore_IO[2]);				// - 100 points (multiplied by damage) per hit
	SCORE_PLAYER_LOSE			= static_cast<int>(PlayerScore_IO[3]);				// - 10000 points for game over
	SCORE_LEVEL2_MULTIPLIER		= static_cast<int>(PlayerScore_IO[11]);				// x2 points in Level 2

	// Apply multiplier here (not in Init) so restarting, which skips Load, doesn't stack it
	if (currentState == GS_LEVEL2)
	{
		SCORE_LIGHT_PROJECTILE_HIT *= SCORE_LEVEL2_MULTIPLIER;
		SCORE_HEAVY_LASER_HIT *= SCORE_LEVEL2_MULTIPLIER;
		SCORE_PLAYER_EAT_HIT *= SCORE_LEVEL2_MULTIPLIER;
		SCORE_PLAYER_LOSE *= SCORE_LEVEL2_MULTIPLIER;
	}

	PlayerScore_IO.clear();

	// SPRITES
	player_sprite = SpriteSheet("Assets/Player/Player.png", 22, 12);

	laser_sprite = SpriteSheet("Assets/Player/Laser.png", 33, 1);


	projectile_texture = AEGfxTextureLoad("Assets/Player/LightRangeAttack.png");
	if (!projectile_texture) {
		std::cerr << "Failed to load projectile texture!" << std::endl;
	}

}


void Init_Player()
{
	// INIT PLAYER STATS
	player.pos			= { PLAYER_SPAWN_X, PLAYER_SPAWN_Y };	// PLAYER POS
	player.vel			= { 0.f, 0.f };							// PLAYER VEL
	player.dodge_vel	= { 0.f, 0.f };							// PLAYER DODGE VEL
	player.hp			= PLAYER_MAXHP;							// PLAYER HEALTH
	player.radius		= PLAYER_RADIUS;						// PLAYER RADIUS

	player.canAttack	= true;
	player.ragemeter	= 0;

	player.hitbox		= Init_AABB(player.pos.x, player.pos.y, PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);

	player.hittimer		= Create_Timer(PLAYER_HIT_DURATION);	// PLAYER HIT TIMER			0.5s
	player.dodgetimer	= Create_Timer(PLAYER_DASH_COOLDOWN);	// PLAYER DODGE TIMER		1.2s

	dodge_cooldown		= Create_Timer(PLAYER_DASH_COOLDOWN);	// PLAYER DODGE COOLDOWN	1.2s
	dodge_time			= Create_Timer(PLAYER_DASH_DURATION);	// PLAYER DODGE TIME		0.3s

	// INIT ATTACKS
	combo_window		= Create_Timer(ATTACK_COMBO_WINDOW);	// MELEE COMBO WINDOW
	
	player.state		= IDLE;
	player.direction_v  = DOWN;
	player.direction_h  = RIGHT;

	player_sprite.Set_Row(0);
	player_sprite.Set_Col(0);

	laser_sprite.Set_Row(0);
	laser_sprite.Set_Col(0);

	melee_attack.timer		 = Create_Timer(ATTACK_DURATION);
	melee_attack.active_time = Create_Timer(ATTACK_END[ATTACK_IDLE]);
	melee_attack.combo		 = 0;

	melee_hitbox		= Init_AABB(player.pos.x, player.pos.y, ATTACK_WIDTH[0], ATTACK_HEIGHT[0]);

	ranged_attack		= Create_Timer(RANGED_ATTACK_DURATION);

	heavy_attack		= Create_Timer(HEAVY_ATTACK_DURATION);

	player.blocked		= Create_Timer(0.5f);

	AEVec2Zero(&atk_dir);

	player.ammo			= PLAYER_MAX_AMMO;			// PLAYER STARTING AMMO = 3
	player.prevAmmo		= -1;

	heavy_atk;
	heavy_atk.pos		= { 0.f, 0.f };
	heavy_atk.vel		= { 0.f,0.f };
	heavy_atk.speed		= 0.f;
	heavy_atk.angle		= 0.f;
	heavy_atk.isActive	= true;
	heavy_atk.isHeavy	= true;

	heavy_atk.lifetimeTimer = Create_Timer(HEAVY_ATTACK_LIFETIME);

	new_proj.lifetimeTimer = Create_Timer(RANGED_ATTACK_LIFETIME);

	player.blocktimer = Create_Timer(BLOCK_COOLDOWN);

	player.deathtimer = Create_Timer(PLAYER_DEATH_DURATION);

	Init_Player_UI(player.ammo, PLAYER_MAX_AMMO, player.hp, PLAYER_MAXHP);

	laserTickTimer = Create_Timer(HEAVY_ATTACK_TICK);
}

void Update_Player()
{
	if (player.state != DEAD)
	{

		if (Update_Timer(&player.hittimer))
		{
			player.state = HIT;
			Update_Timer(&player.blocktimer);
		}
		else if (Update_Timer(&player.blocked))
		{
			// do nothing
		}
		else
		{
			if (player.state == HIT || player.state == BLOCKED || player.state == PARRIED)
			{
				player.state = IDLE;
			}

			Parrying();

			if (player.state != PARRY && player.state != PARRIED && player.state != BLOCK)
			{
				Movement();

				if (player.state != DODGE)
				{
					Attacking();

					Attacking_Ranged();
				}
			}
		}
		player.hp = AEClamp(player.hp, 0.f, 100.f);
		if (!player.hp)
		{
			player.state = DEAD;
			player.deathtimer.current = 0.f;

			
		}



		if (currentState == GS_LEVEL1) // Only apply boundaries in Level 1
		{
			f32 map_width = 2972.0f, map_height = 1136.0f;
			f32 left_bound = -map_width / 2;
			f32 right_bound = map_width / 2 - 80;
			f32 top_bound = -map_height / 2;
			f32 bottom_bound = 600.0f;

			// Restrict movement within the map boundaries
			if (player.pos.x < left_bound) player.pos.x = left_bound;
			if (player.pos.x > right_bound) player.pos.x = right_bound;
			if (player.pos.y < top_bound) player.pos.y = top_bound;
			if (player.pos.y > bottom_bound) player.pos.y = bottom_bound;
		}

		else if (currentState == GS_LEVEL2) // Only apply boundaries in Level 2
		{
			f32 map_width = 2972.0f, map_height = 1136.0f;
			f32 left_bound = -map_width / 2;
			f32 right_bound = map_width / 2 - 80;
			f32 top_bound = -map_height / 2;
			f32 bottom_bound = 500.0f;

			// Restrict movement within the map boundaries
			if (player.pos.x < left_bound) player.pos.x = left_bound;
			if (player.pos.x > right_bound) player.pos.x = right_bound;
			if (player.pos.y < top_bound) player.pos.y = top_bound;
			if (player.pos.y > bottom_bound) player.pos.y = bottom_bound;
		}
		else if (currentState == GS_HUBWORLD) // Apply for hubworld
		{
			f32 map_width = 2972.0f, map_height = 1136.0f;
			f32 left_bound = -map_width / 2;
			f32 right_bound = map_width / 2;
			f32 top_bound = -map_height / 2;
			f32 bottom_bound = 600.0f;

			// Restrict movement within the map boundaries
			if (player.pos.x < left_bound) player.pos.x = left_bound;
			if (player.pos.x > right_bound) player.pos.x = right_bound;
			if (player.pos.y < top_bound) player.pos.y = top_bound;
			if (player.pos.y > bottom_bound) player.pos.y = bottom_bound;
		}
	}
	else	// Player state DEAD
	{
		if (!Update_Timer(&player.deathtimer))
		{
			levelState = GAMEOVER;

		}
	}
	Update_Player_UI(player.ammo, player.hp, player.blocktimer.current, player.ragemeter, dodge_cooldown.current);
}


// Draw Player
void Draw_Player()
{
	// SHADOW DRAWING
	DrawCircleAdvanced(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y - player.radius * 2.f), 35.f, 20.f, SetColor(0x92000000));			// Draw shadow under player
	
	switch (player.state)
	{
	case MELEE:
		if (Screen_To_World_x(Mouse.x) > player.pos.x)
		{
			player.direction_h = RIGHT;
		}
		else
		{
			player.direction_h = LEFT;
		}
		switch (melee_attack.combo)
		{
		case 1:

			//new
			player_sprite.Set_Row(6);
			player_sprite.Set_Col(0, 9, melee_attack.active_time.current);
			break;
		case 2:

			//new
			player_sprite.Set_Row(7);
			player_sprite.Set_Col(0, 9, melee_attack.active_time.current);
			break;
		case 3:

			//new
			player_sprite.Set_Row(8);
			player_sprite.Set_Col(0, 19, melee_attack.active_time.current);
			break;
		default:
			break;
		}
		break;	// end case MELEE
		// ----------------------------------------------------------------------------
	case RANGE:
		if (Screen_To_World_x(Mouse.x) > player.pos.x)
		{
			player.direction_h = RIGHT;
		}
		else
		{
			player.direction_h = LEFT;
		}
		player_sprite.Set_Row(9);
		player_sprite.Set_Col(0, 7, ranged_attack.current);
		break;	// end case RANGE
		// ----------------------------------------------------------------------------
	case HEAVY:
		if (Screen_To_World_x(Mouse.x) > player.pos.x)
		{
			player.direction_h = RIGHT;
		}
		else
		{
			player.direction_h = LEFT;
		}
		player_sprite.Set_Row(10);
		player_sprite.Set_Col(0, 21, heavy_attack.current);
		break;	// end case HEAVY
		// ----------------------------------------------------------------------------

	case MOVING:
		
		player_sprite.Set_Row(1);
		player_sprite.Loop(0, 7, .8f);
		break;
		// ----------------------------------------------------------------------------
	case HIT:

		player_sprite.Set_Row(2);
		player_sprite.Set_Col(0, 3, player.hittimer.current);
		break;	// end case HIT
		// ----------------------------------------------------------------------------
	case DODGE:

		player_sprite.Set_Row(3);
		player_sprite.Set_Col(0, 3, dodge_time.current);
		break;	// end case DODGE
		// ----------------------------------------------------------------------------
	case PARRY:

		player_sprite.Set_Row(4);
		player_sprite.Set_Col(0);
		break;	// end case PARRY
		// ----------------------------------------------------------------------------
	case PARRIED:

		player_sprite.Set_Row(4);
		player_sprite.Set_Col(1, 3, player.blocked.current);
		break;	// end case PARRIED
		// ----------------------------------------------------------------------------
	case BLOCK:

		//new
		player_sprite.Set_Row(5);
		player_sprite.Set_Col(0);
		break;
		// ----------------------------------------------------------------------------
	case BLOCKED:

		player_sprite.Set_Row(5);
		player_sprite.Set_Col(1, 3, player.blocked.current);
		break;	// end case BLOCKED
		// ----------------------------------------------------------------------------
	case DEAD:

		player_sprite.Set_Row(11);
		player_sprite.Set_Col(0, 10, player.deathtimer.current);
		break;	// end case DEAD
		// ----------------------------------------------------------------------------
	case IDLE:
	default:

		//new
		player_sprite.Set_Row(0);
		player_sprite.Loop(0, 7, 1.f);
		break;
	}

	switch (player.direction_h)
	{
	case LEFT:
		player_sprite.Draw(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), PLAYER_DRAW_WIDTH, PLAYER_DRAW_HEIGHT, false);
		break;
	case RIGHT:
		player_sprite.Draw(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), PLAYER_DRAW_WIDTH, PLAYER_DRAW_HEIGHT, true);
		break;
	default:
		break;
	}


// Draw Heavy projectiles (Without this, the heavy attack will be weird)
	for (auto& proj : projectiles)
	{
		// Update position
		AEVec2 scaledVel;
		AEVec2Scale(&scaledVel, &proj.vel, proj.speed * dt);
		AEVec2Add(&proj.pos, &proj.pos, &scaledVel);
		if (projectile_texture != nullptr)
		{ // Ensure texture is loaded
			// Use different size depending on whether the projectile is heavy or not
			if (proj.isHeavy) 
			{
				if (!heavy_atk.isActive)
				{
					projectiles.pop_back();
				}
			}
			else
			{
				if (!new_proj.isActive)
				{
					projectiles.pop_back();
				}
			}
		}
	}


	// Draw light projectiles so that it can delete on impact
	for (auto it = projectiles.begin(); it != projectiles.end(); )
	{
		// Skip inactive projectiles
		if (!it->isActive)
		{
			it = projectiles.erase(it); // Erase and move iterator forward
			continue; // Skip to next iteration
		}

		// Update position
		AEVec2 scaledVel;
		AEVec2Scale(&scaledVel, &it->vel, it->speed * dt);
		AEVec2Add(&it->pos, &it->pos, &scaledVel);

		if (projectile_texture != nullptr)
		{ // Ensure texture is loaded
			if (it->isHeavy)
			{
				if (heavy_atk.isActive)
				{

					float offsetDistance = 400.0f;
					float offsetX = cosf(it->angle) * offsetDistance;
					float offsetY = sinf(it->angle) * offsetDistance;

					// Apply the offset to the projectile's position
					float screenX = World_To_Screen_x(it->pos.x + offsetX);
					float screenY = World_To_Screen_y(it->pos.y + offsetY);

					// Draw the heavy projectile slightly in front of the player based on angle
					laser_sprite.Set_Col(0, 32, heavy_atk.lifetimeTimer.current);
					laser_sprite.Draw(screenX, screenY, 1000.f, 200.f, false, it->angle);
				}
			}
			else
			{
				DrawImage(World_To_Screen_x(it->pos.x), World_To_Screen_y(it->pos.y), 100.f, 100.f, it->angle, projectile_texture);
			}
		}

		++it; // Move iterator forward
	}





}

void Free_Player()
{

}

void Unload_Player()
{

	player_sprite.Unload();
	laser_sprite.Unload();

	AEGfxTextureUnload(projectile_texture);

}