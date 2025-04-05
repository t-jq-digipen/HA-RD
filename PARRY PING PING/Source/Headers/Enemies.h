//---------------------------------------------------------
// File:	Enemies.h
// Project: Heavy Assault : Rust & Dust
// Author:	Muhammad Ashfer Danishkhan bin Afzalkhan
//			-
// 
//			Tan Jun Quan
//			- Parry spark
//
// Brief:	Declaration of General Enemy related functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"
#include "Camera.h"
#include "player.h"

// ----------------------------------------------------------------------------
// For basic enemy attack pattern, mainly used for practice dummies
// ----------------------------------------------------------------------------
typedef enum EnemyAttackPatternState
{
	Idle = 0,						// For each state the enemy can be in
	WindUp = 1,						// General lingo for states: Idle: Doing nothing, WindUp: Going to attack, Swinging: currently attacking, Parried: upon successful player parry, Destroyed: If parry bot health < 0
	Swinging = 2,
	Parried = 3,
	Destroyed = 4

}EnemyAttackPatternState;

// ----------------------------------------------------------------------------
// Very early implementation of first boss attacks, will remove soon
// ----------------------------------------------------------------------------
typedef enum JQMonkeAttackPatternState
{
	JQIdle = 0,				//idle
	JQSmashWindUp = 1,		//wind up for smash
	JQSmash = 2,			//SMASH
	JQSweepWindUp = 3,		//rocket propeler wind up
	JQSweep = 4,			//SWEEP
	JQRecovery = 5,			//recovery before going back to idle
	JQParried = 6			//if parried during sweep
}JQMonkeAttackPatternState;


// ----------------------------------------------------------------------------
// Struct for Melee Practice Range Dummy!
// ----------------------------------------------------------------------------
typedef struct DummyBotMelee
{
	AEVec2 pos, attackpos;							// Positions for dummy and where it will attack
	f32 rad;										// Radius
	f32 hp, max_hp;									// Current health and starting health
	f32 damage;										// Damage it will do & receive
	f32 spark_buffer;								// time before attack starts (indicates to player a parryable attack is coming)
	bool spark_draw;								// Bool to draw spark ^
	f32 AttackHitboxWidth, AttackHitboxHeight;		// Width and Height for rectangular hitbox
	EnemyAttackPatternState state;					// States! Idle, attacking, etc.
	enum FacingDir {								// Which direction the bot is facing - Code is designed for dynamic placement! Just need to specify!
		FacingLeft = 0, FacingRight = 1, FacingUp = 2, FacingDown = 3
	}FacingDir;

	aabb hitbox;									// Hitbox 
	f32 hitboxWidth, hitboxHeight;					// Hitbox width and height

	Timer idle_timer, windup_timer, attack_timer, stun_timer, respawn_timer;		// Timers! - Respectively, how long it will take to transition between states
}DummyBotMelee;


// ----------------------------------------------------------------------------
// Struct for Ramged Projectile Practice Range Dummy!
// ----------------------------------------------------------------------------
typedef struct DummyBotRange
{
	AEVec2 pos, attackpos, projectile_pos;			// Positions for dummy and where it will attack + position of current projectile 
	f32 rad;										// Radius
	f32 hp, max_hp;									// Current health and starting health
	f32 damage;										// Damage it will do & receive
	f32 projectile_speed, projectile_radius, projectile_range;		// Projectile variables! 
	bool projectile_parry, projectile_draw;			// Bool checks for when to draw projectile and if it was parried
	AEMtx33 projectile_transform;					// Transform for projectile!
	f32 AttackHitboxWidth, AttackHitboxHeight;		//  Width and Height for rectangular hitbox
	EnemyAttackPatternState state;					// States! Idle, attacking, etc.
	enum FacingDir {								// Which direction the bot is facing - Code is designed for dynamic placement! Just need to specify!
		FacingLeft = 0, FacingRight = 1, FacingUp = 2, FacingDown = 3
	}FacingDir;

	aabb hitbox;									// Hitbox 
	f32 hitboxWidth, hitboxHeight;					// Hitbox width and height

	Timer idle_timer, windup_timer, attack_timer, stun_timer, respawn_timer;		// Timers! - Respectively, how long it will take to transition between states
}DummyBotRange;


// ----------------------------------------------------------------------------
// Init_DummyBotMelee/Range()		- Init Variables
// Active_DummyBotMelee/Range()		- Update each frame for dummy logic
// Attack_DummyBotMelee/Range()		- Attack detection logic
// Draw_DummyBotMelee/Range()		- Draw respective sprites
// ----------------------------------------------------------------------------
// Melee Dummy!
void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp);
void Active_DummyBotMelee();
void Attack_DummyBotMelee();
void Draw_DummyBotMelee();

// Projectile/ Ranged Dummy!
void Init_DummyBotRange(AEVec2 pos, f32 rad, f32 max_hp);
void Active_DummyBotRange();
void Attack_DummyBotRange();
void Draw_DummyBotRange();

// Load and unload spritesheets
void Load_DummyBot();
void Unload_DummyBot();

// ----------------------------------------------------------------------------
// Inits colours!
// ----------------------------------------------------------------------------
void Init_Enemy_Indicator();

// ----------------------------------------------------------------------------
// For parry spark!
// ----------------------------------------------------------------------------
// Initializes spark
void Init_Parry_Spark();
// Loads spark sprite
void Load_Parry_Spark();
// Unloads spark sprite
void Unload_Parry_Spark();
// Sets spark location
void Set_Spark_Loc(f32 pos_x, f32 pos_y);
// Start spark drawing, makes it active
void Start_Spark(f32 duration);
// End spark, e.g when boss is dead
void End_Spark();
// Draws the spark
void Draw_Spark();