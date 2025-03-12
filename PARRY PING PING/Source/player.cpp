//---------------------------------------------------------
// file:	player.cpp
// author:	TAN JUN QUAN, MUHAMMAD FAUZAN BIN MOHAMED FAUZY
// email:	t.junquan@digipen.edu, muhammadfauzan.b@digipen.edu
//
// brief:	Player functions  |	Line number
//			- Movement			24
//			- Atack				72
//			- Attacking_Ranged
//			- Attacking_Ranged_Impact
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "player.h"
#include "Enemies.h"
#include <vector>
#include <algorithm>
#include <cmath>



// Define map boundary limits
const float MAP_LEFT = -500.f;
const float MAP_RIGHT = 500.f;
const float MAP_TOP = 300.f;
const float MAP_BOTTOM = -300.f;

typedef struct Attack
{
	Timer timer;
	int combo;
} Attack;

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


const float PLAYER_WIDTH = 250.f;
const float PLAYER_HEIGHT = 250.f;

const float PLAYER_HITBOX_WIDTH = 80.f;
const float PLAYER_HITBOX_HEIGHT = 120.f;

Player player;
Attack melee_attack;
SpriteSheet_Old sword;

SpriteSheet_Old ssidle_move;
SpriteSheet_Old ssmelee_atk;
SpriteSheet_Old sshitdodgeparry;

// ATTACK LOGIC
f32 atk_pos_x, atk_pos_y, atk_rot;
AEVec2 atk_dir;

static float rightClickTimer = 0.f;
const float rightClickHoldDuration = 0.2f; // 1 second hold to trigger heavy attack

// MELEE ATTACK
// =======================================================================
#pragma region MELEE
// Melee Attack hitbox and durations in seconds

AEGfxTexture* AtkHB;

const float ATTACK_COOLDOWN[4] = { 0.f, 0.5f, 0.7f, 0.7f };
const float ATTACK_DURATION = 1.f;
const float ATTACK_START[4] = { 0.f, 0.2f, 0.2f, 0.3f };
const float ATTACK_END[4] = { 0.f, 0.4f, 0.5f, 0.7f };
const float ATTACK_TOTAL[4] = { 1.f,1.f,1.f,1.f };

const float ATTACK_COMBO_WINDOW = 1.f;

Timer combo_window = { 1.f, 1.f, ATTACK_COMBO_WINDOW };

const float ATTACK_ANGLE[2] = { PI / 4.f, PI / 3.f };
const float ATTACK_RADIUS[4] = { 150.f,150.f,150.f,150.f };

const float ATTACK_DAMAGE[4] = { 0.f, 5.f, 5.f, 15.f };

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
	if (!Update_Timer(&melee_attack.timer) && melee_attack.combo == 3 || !Update_Timer(&combo_window))
	{
		// reset the combo to idle
		melee_attack.combo = ATTACK_IDLE;

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

		combo_window.current = 0.f;
		combo_window.duration = ATTACK_END[melee_attack.combo] + ATTACK_COMBO_WINDOW;
		
	}

	// update sword swing timer
	Update_Timer(&sword.timer);

	hitbox_active = false;
	// logic for if hitbox should be active, to be replaced with a bool return probably @raziq
	if (melee_attack.timer.current >= ATTACK_START[melee_attack.combo] && melee_attack.timer.current <= ATTACK_END[melee_attack.combo] && player.state == MELEE)
	{
		std::cout << "ATTACKRADIUS: " << ATTACK_RADIUS[melee_attack.combo] << '\n';
		DrawCircle(World_To_Screen_x(atk_pos_x), World_To_Screen_y(atk_pos_y), ATTACK_RADIUS[melee_attack.combo], player.color);
		DrawImage(World_To_Screen_x(atk_pos_x), World_To_Screen_y(atk_pos_y), player.radius * 10.f, player.radius * 10.f, atk_rot, AtkHB);
		hitbox_active = true;
	}
	else if (melee_attack.timer.current >= ATTACK_END[melee_attack.combo] && player.state == MELEE)
	{
		player.state = IDLE;
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

bool Attacked_Melee_rect(AEVec2 target, f32 width, f32 height, int* check_hit, float* damage)
{
	// Check if:
	// - Hitbox is active
	// - Player is attacking
	// - Target is within range of current attack
	if (hitbox_active && melee_attack.combo && (AETestCircleToRect(&player.pos, ATTACK_RADIUS[melee_attack.combo], &target, width, height)))
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
				*check_hit = melee_attack.combo;
				*damage = ATTACK_DAMAGE[melee_attack.combo];
				return true;
			}
			break;
		}
		case 3:
		{
			// No need to check direction for third swing as it is 360 circle
			//std::cout << "HIT ATTACK " << melee_attack.combo << '\n';
			*check_hit = melee_attack.combo;
			*damage = ATTACK_DAMAGE[melee_attack.combo];
			return true;
			break;
		}
		default:
		{
			break;
		}
		}
	}
	else if (!melee_attack.combo)
	{
		*check_hit = 0;
	}
	return false;
}
#pragma endregion
// =======================================================================

// RANGED ATTACK
// =======================================================================
#pragma region RANGED

AEGfxTexture* projectile_texture;

AEGfxTexture* heavy_texture;

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

			// Debugging: Print projectile and target info
			std::cout << "Projectile Position: (" << proj.pos.x << ", " << proj.pos.y << ")\n";
			std::cout << "Heavy Attack Position: (" << heavyAttackPos.x << ", " << heavyAttackPos.y << ")\n";
			std::cout << "Target Position (Boss): (" << target.x << ", " << target.y << ")\n";

			// Direction of the heavy attack based on the angle (scaled by the attack's size)
			float offsetX2 = cosf(proj.angle) * height;  // Use height to define the attack range (or use width)
			float offsetY2 = sinf(proj.angle) * height;  // Same for Y

			// Apply the offset to the projectile's position to get the endpoint of the laser
			AEVec2 heavyAttackPosEnd = { heavyAttackPos.x + offsetX2, heavyAttackPos.y + offsetY2 };

			// Create a line segment for the laser (start from heavyAttackPos, end at calculated end point)
			AELineSegment2 lineSegment;
			lineSegment.mP0 = heavyAttackPos;      // Start of the line segment (laser starts at heavyAttackPos)
			lineSegment.mP1 = heavyAttackPosEnd;   // End of the line segment (laser moves based on attack size and direction)

			// Check if the laser intersects with the boss's circle (represented by the target and radius)
			if (LineSegmentCircleCollision(heavyAttackPos, heavyAttackPosEnd, target, width / 2)) {
				*damage = 10.f * dt;  // Heavy attack damage
				hit = true;
				std::cout << "Heavy Attack Hit!\n";
			}
			else {
				std::cout << "NO HIT\n";
			}
		}
		// **Light projectile collision check** (No change here, you can keep this as is)
		if (!proj.isHeavy && AETestCircleToRect(&proj.pos, proj.radius, &target, width, height)) {
			*damage = 5.f;  // Light attack damage
			hit = true;
			proj.isActive = false; // Deactivate light projectile after hit
			std::cout << "Light Attack Hit!\n";
		}
	}

	return hit; // Return true if at least one projectile hits
}















void Attacking_Ranged()
{
	if (AEInputCheckTriggered(AEVK_R) && player.ammo < 3)  //Reloading of Ammo and ensures does not exceed more than 3.
	{
		player.ammo += 1;
	}


	if (player.ammo > 0)
	{


		// In your update function, handle the right-click hold event
		if (player.ammo == 3 && AEInputCheckCurr(AEVK_RBUTTON)) // AEInputCheckCurr for right-click being held
		{
			// Increase the right-click timer as long as the right button is held
			rightClickTimer += dt; 

			// Fire the heavy attack when the right-click is held for the set duration
			if (rightClickTimer >= rightClickHoldDuration)
			{
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

				// Reset the right-click timer
				rightClickTimer = 0.f;

				//player.state = HEAVY;
			}


		}
		// If right mouse button is released, reset the timer
		else if (AEInputCheckReleased(AEVK_RBUTTON))
		{
			rightClickTimer = 0.f;
		}

		// Light Attack Handling with Cooldown
		if (player.ammo != 0 && AEInputCheckReleased(AEVK_RBUTTON) && player.lightAttackCooldown <= 0.f)
		{
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

			//player.state = RANGE;

			// Set cooldown
			player.lightAttackCooldown = 1.0f;
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

	// Debugging print to see if the timer is updating
	//std::cout << "Timer: " << heavy_atk.lifetimeTimer.current << " seconds left.\n";


	if (!lifetimeExpiredHeavy) 
	{
		//std::cout << "Projectile lifetime expired, deactivating.\n";
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





const float DASH_COOLDOWN = 1.5f;
const float DASH_DURATION = 0.3f;

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
		else // melee
		{
			float atk_vel_scale = -(melee_attack.timer.current * melee_attack.timer.current) + ATTACK_END[melee_attack.combo] * melee_attack.timer.current;

			AEVec2Scale(&player.vel, &atk_dir, player.speed * atk_vel_scale * dt);
		}
	}

	bool dodging = Update_Timer(&player.dodgetimer);
	if (player.vel.x || player.vel.y)
	{
		if (player.state == MOVING)
		{
			AEVec2Normalize(&player.vel, &player.vel);
		}
		AEVec2Scale(&player.vel, &player.vel, player.speed * dt);

		if (!dodging)
		{
			if (AEInputCheckTriggered(AEVK_SPACE))
			{
				AEVec2Set(&player.dodge_vel, player.vel.x, player.vel.y);
				AEVec2Normalize(&player.dodge_vel, &player.dodge_vel);
				player.state = DODGE;
				player.dodgetimer.current = 0.f;
			}
		}
	}


	if (player.dodgetimer.current * DASH_COOLDOWN <= DASH_DURATION)
	{
		player.direction_h = (player.dodge_vel.x >= 0.f) ? RIGHT : LEFT;
		player.state = DODGE;
		boost = (player.dodgetimer.current * DASH_COOLDOWN <= DASH_DURATION / 2.f) ? 1.5f * player.speed : 0.f;
		AEVec2Scale(&player.vel, &player.dodge_vel, (player.speed + boost) * dt);
	}

	// position update
	AEVec2Add(&player.pos, &player.vel, &player.pos);
	Update_AABB(&player.hitbox, player.pos.x, player.pos.y, PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);
}

const float PARRY_DURATION = 0.3f; // PARRY DURATION		0.2s parry
const float BLOCK_DURATION = 1.0f; // PARRY+BLOCK DURATION	0.8s block + 0.2s parry
const float BLOCK_COOLDOWN = 3.0f; // TOTAL BLOCK DURATION	0.2s parry + 0.8s block + 2s cooldown

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
			std::cout << "parrying";
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
			//std::cout << "block cd starts\n";
		}
		if (player.state == BLOCK || player.state == BLOCKED || player.state == PARRY || player.state == PARRIED)
		{
			player.state = IDLE;
		}
	}
	else if (AEInputCheckTriggered(AEVK_E))
	{
		player.blocktimer.current = 0.f;
		//std::cout << "block starts\n";
	}
	//std::string plystate = (player.state == PARRY) ? "Parry" : player.state == BLOCK ? "Block" : "IDK";
	//std::cout << "Block timer: " << player.blocktimer.current * player.blocktimer.duration << " seconds\n";
	//std::cout << "Player State: " << plystate << '\n';
}

void Parried_Hit()
{
	player.state = PARRIED;
	player.blocked.current = 0.f;

	// reload player ammo
	player.ammo = 3;

	// reset block cooldown
	player.blocktimer.current = 1.f;
}

const f32 BLOCK_REDUCTION = 0.5f;

void Blocked_Hit(f32 damage)
{
	player.state = BLOCKED;
	player.blocked.current = 0.f;
	player.hp -= damage * BLOCK_REDUCTION;
}

void Eat_Hit(f32 damage)
{
	// take dmg
	player.state = HIT;
	player.hittimer.current = 0.f;
	player.hp -= damage;
}


void Load_Player()
{
	AtkHB = AEGfxTextureLoad("Assets/hitboxtest.png");

	sword = Init_SpriteSheet("Assets/slash.png", 4, 10, ATTACK_DURATION);

	ssidle_move = Init_SpriteSheet("Assets/Player/IdleMove.png", 2, 8, 1.f);

	ssmelee_atk = Init_SpriteSheet("Assets/Player/Melee_Attack.png", 2, 20, ATTACK_DURATION);

	sshitdodgeparry = Init_SpriteSheet("Assets/Player/HitDodgeParry.png", 4, 4, 1.f);

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
	player.pos = { 0.f,0.f };
	player.vel = { 0.f,0.f };
	player.speed = 500.f;
	player.hp = 100.f;
	player.radius = 25.f;

	player.dodge_vel = { 0.f,0.f };

	player.width = PLAYER_WIDTH;
	player.height = PLAYER_HEIGHT;

	player.hitbox = Init_AABB(player.pos.x, player.pos.y, PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);

	player.hittimer = Create_Timer(0.5f);
	player.dodgetimer = Create_Timer(DASH_COOLDOWN);
	player.color = SetColor(0xFF00008B);
	player.state = IDLE;
	player.direction_v = DOWN;
	player.direction_h = RIGHT;

	player.spritesheet = &ssidle_move;

	melee_attack.timer = Create_Timer(1.f);
	melee_attack.combo = 0;

	player.blocked = Create_Timer(0.5f);

	AEVec2Zero(&atk_dir);

	player.ammo = 3;

	heavy_atk;
	heavy_atk.pos = {};
	heavy_atk.vel = { 0.f, 0.f };
	heavy_atk.speed = 0.f; // Adjust speed as needed
	heavy_atk.angle = { 0.f };
	heavy_atk.isActive = true;
	heavy_atk.isHeavy = true;
	heavy_atk.lifetimeTimer = Create_Timer(1.5f);

	new_proj.lifetimeTimer = Create_Timer(5.0f);

	player.blocktimer = Create_Timer(BLOCK_COOLDOWN);

	Init_Player_UI(player.ammo, 3, player.hp, 100.f);
}

void Update_Player()
{
	if (player.state != DEAD)
	{
		if (player.knockbackTimer > 0.0f)
		{
				// Calculate the knockback direction based on the direction of the attack
				f32 cursor_x = Screen_To_World_x(Mouse.x); // or other method to get attack target
				f32 cursor_y = Screen_To_World_y(Mouse.y); // Get the attack position (e.g., mouse cursor or impact point)

				AEVec2 attackDirection = { cursor_x - player.pos.x, cursor_y - player.pos.y };  // Direction from player to target
				AEVec2Normalize(&attackDirection, &attackDirection); // Normalize to get a unit vector

				// Apply knockback in the opposite direction of the attack
				AEVec2 oppositeDirection = { -attackDirection.x, -attackDirection.y };

				// Apply knockback boost if necessary
				float knockbackBoost = (player.knockbackTimer > 0.15f) ? 1.5f * player.speed : 0.f;

				// Apply knockback velocity (scaled by the speed and knockback boost)
				AEVec2Scale(&player.vel, &oppositeDirection, (player.speed + knockbackBoost) * dt);

				// Move the player using the knockback velocity
				AEVec2Add(&player.pos, &player.vel, &player.pos);

				// Apply friction to gradually slow down the knockback
				float knockbackFriction = 0.92f; // Adjust this value to control the smoothness of the knockback stop
				player.vel.x *= knockbackFriction;
				player.vel.y *= knockbackFriction;

				// Decrease knockback timer
				player.knockbackTimer -= dt;

				// Stop movement when the knockback velocity is very small
				if (fabs(player.vel.x) < 1.0f) player.vel.x = 0.f;
				if (fabs(player.vel.y) < 1.0f) player.vel.y = 0.f;
			//if (player.state == RANGE)
			//{
			//	// Calculate the knockback direction based on the direction of the attack
			//	f32 cursor_x = Screen_To_World_x(Mouse.x); // or other method to get attack target
			//	f32 cursor_y = Screen_To_World_y(Mouse.y); // Get the attack position (e.g., mouse cursor or impact point)

			//	AEVec2 attackDirection = { cursor_x - player.pos.x, cursor_y - player.pos.y };  // Direction from player to target
			//	AEVec2Normalize(&attackDirection, &attackDirection); // Normalize to get a unit vector

			//	// Apply knockback in the opposite direction of the attack
			//	AEVec2 oppositeDirection = { -attackDirection.x, -attackDirection.y };

			//	// Apply knockback boost if necessary
			//	float knockbackBoost = (player.knockbackTimer > 0.15f) ? 1.1f * player.speed : 0.f;

			//	// Apply knockback velocity (scaled by the speed and knockback boost)
			//	AEVec2Scale(&player.vel, &oppositeDirection, (player.speed + knockbackBoost) * dt);

			//	// Move the player using the knockback velocity
			//	AEVec2Add(&player.pos, &player.vel, &player.pos);

			//	// Apply friction to gradually slow down the knockback
			//	float knockbackFriction = 0.92f; // Adjust this value to control the smoothness of the knockback stop
			//	player.vel.x *= knockbackFriction;
			//	player.vel.y *= knockbackFriction;

			//	// Decrease knockback timer
			//	player.knockbackTimer -= dt;

			//	// Stop movement when the knockback velocity is very small
			//	if (fabs(player.vel.x) < 1.0f) player.vel.x = 0.f;
			//	if (fabs(player.vel.y) < 1.0f) player.vel.y = 0.f;
			//}
		}

		if (Update_Timer(&player.hittimer))
		{
			player.state = HIT;
		}
		else if (Update_Timer(&player.blocked))
		{
			//?
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

				Attacking();

				Attacking_Ranged();
			}
		}
		player.hp = AEClamp(player.hp, 0.f, 100.f);
		if (!player.hp)
		{
			player.state = DEAD;
		}



		f32 map_width = 2900.0f, map_height = 2300.0f;
		f32 left_bound = -map_width / 2;
		f32 right_bound = map_width / 2;
		f32 top_bound = -map_height / 2;
		f32 bottom_bound = 1100.0f;



		// Restrict movement within the map boundaries
		if (player.pos.x < left_bound) player.pos.x = left_bound;
		if (player.pos.x > right_bound) player.pos.x = right_bound;
		if (player.pos.y < top_bound) player.pos.y = top_bound;
		if (player.pos.y > bottom_bound) player.pos.y = bottom_bound;


		
	}
	else
	{
		levelState = GAMEOVER;
		std::cout << "bro is dead\n";
	}
	Update_Player_UI(player.ammo, player.hp, player.blocktimer.current);
}

// Draw Player
void Draw_Player()
{
	//DrawCircle(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius, player.color);

	sword.current_col = static_cast<int>(sword.timer.current * sword.cols);
	sword.current_row = melee_attack.combo;
	//Draw_SpriteSheet(sword, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius * 10.f, player.radius * 10.f, sword.uv_width * sword.current_col, sword.uv_height * sword.current_row, atk_rot);
	
	// DRAW HITBOX
	//DrawRect(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.hitbox.max.x - player.hitbox.min.x, player.hitbox.max.y - player.hitbox.min.y, player.color);

	//std::cout << "States:\n0 - IDLE\n1 - MOVING\n2 - MELEE\n3 - RANGE\n4 - HEAVY\n5 - DODGE\n6 - HIT\n7 - DEAD\n8 - BLOCK\n9 - BLOCKED\n10 - PARRY\n11 - PARRIED\n12 - MISSED";
	//std::cout << "Playerstate = " << player.state << "\n\n";
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
			Set_SpriteSheet_current(&ssmelee_atk, 0, 0, 10, melee_attack.timer, ATTACK_END[1]);
			break;
		case 2:
			Set_SpriteSheet_current(&ssmelee_atk, 0, 10, 20, melee_attack.timer, ATTACK_END[2]);
			break;
		case 3:
			Set_SpriteSheet_current(&ssmelee_atk, 1, 0, 20, melee_attack.timer, ATTACK_END[3]);
			break;
		default:
			break;
		}
		player.spritesheet = &ssmelee_atk;
		break;
	case MOVING:
		Loop_Timer(&ssidle_move.timer);
		Set_SpriteSheet_current(&ssidle_move, 1, 0, 8, ssidle_move.timer, ssidle_move.timer.duration);
		player.spritesheet = &ssidle_move;
		break;
	case HIT:
		Set_SpriteSheet_current(&sshitdodgeparry, 0, 0, 4, player.hittimer, player.hittimer.duration);
		player.spritesheet = &sshitdodgeparry;
		break;
	case DODGE:
		Set_SpriteSheet_current(&sshitdodgeparry, 1, 0, 4, player.dodgetimer, DASH_DURATION);
		player.spritesheet = &sshitdodgeparry;
		break;
	case PARRY:
		Set_SpriteSheet_current(&sshitdodgeparry, 2, 0, 1, player.blocktimer, player.blocktimer.duration);
		player.spritesheet = &sshitdodgeparry;
		break;
	case PARRIED:
		Set_SpriteSheet_current(&sshitdodgeparry, 2, 1, 4, player.blocked, player.blocked.duration);
		player.spritesheet = &sshitdodgeparry;
		break;
	case BLOCK:
		Set_SpriteSheet_current(&sshitdodgeparry, 3, 0, 1, player.blocktimer, player.blocktimer.duration);
		player.spritesheet = &sshitdodgeparry;
		break;
	case BLOCKED:
		Set_SpriteSheet_current(&sshitdodgeparry, 3, 1, 4, player.blocked, player.blocked.duration);
		player.spritesheet = &sshitdodgeparry;
		break;
	case IDLE:
	default:
		Loop_Timer(&ssidle_move.timer);
		Set_SpriteSheet_current(&ssidle_move, 0, 0, 8, ssidle_move.timer, ssidle_move.timer.duration);
		player.spritesheet = &ssidle_move;
		break;
	}

	switch (player.direction_h)
	{
	case LEFT:
		Draw_SpriteSheet_test(*player.spritesheet, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius * 10.f, player.radius * 10.f, 0.f);
		break;
	case RIGHT:
		Draw_SpriteSheet_test(*player.spritesheet, World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), -player.radius * 10.f, player.radius * 10.f, 0.f);
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
				if (heavy_atk.isActive)
				{

					float offsetDistance = 500.0f;
					float offsetX = cosf(proj.angle) * offsetDistance;
					float offsetY = sinf(proj.angle) * offsetDistance;

					// Apply the offset to the projectile's position
					float screenX = World_To_Screen_x(proj.pos.x + offsetX);
					float screenY = World_To_Screen_y(proj.pos.y + offsetY);

					// Draw the heavy projectile slightly in front of the player based on angle
					DrawImage(screenX, screenY, 100.f, 1000.f, proj.angle, heavy_texture);
				}
				else {
					projectiles.pop_back();
				}
			}
			else
			{
				if (new_proj.isActive)
				{
					DrawImage(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 100.f, 100.f, proj.angle, projectile_texture); // Light projectile
				}
				else {
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

					float offsetDistance = 500.0f;
					float offsetX = cosf(it->angle) * offsetDistance;
					float offsetY = sinf(it->angle) * offsetDistance;

					// Apply the offset to the projectile's position
					float screenX = World_To_Screen_x(it->pos.x + offsetX);
					float screenY = World_To_Screen_y(it->pos.y + offsetY);

					// Draw the heavy projectile slightly in front of the player based on angle
					DrawImage(screenX, screenY, 1000.f, 1000.f, it->angle, heavy_texture);
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
	AEGfxTextureUnload(AtkHB);

	Unload_SpriteSheet(&sword);

	AEGfxTextureUnload(projectile_texture);
	AEGfxTextureUnload(heavy_texture);

	Unload_SpriteSheet(&sshitdodgeparry);
	Unload_SpriteSheet(&ssidle_move);
	Unload_SpriteSheet(&ssmelee_atk);
	//AEGfxTextureUnload(player.spritesheet->texture);
}