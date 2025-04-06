//---------------------------------------------------------
// File:	player.cpp
// Project: Heavy Assault : Rust & Dust
// Authors:	
//          Tan Jun Quan ------------------------------ 70%
//          - Player struct
//			- States
//          - Movement
//          - Melee
//          - Dodging  
//          - Parrying
// 
//			Muhammad Fauzan bin Mohamed Fauzy --------- 25%
//			- Ranged attacks
// 
//			Muhammad Ashfer Danishkhan bin Afzalkhan -- 5%
//			- Damage based on difficulty modifier
//			- Rage meter buff
//
// Brief:	Declaration of player functions and state enums
//          governing the player behaviour and systems.
// 
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#pragma once
#include "utils.h"
#include "Camera.h"
#include "UI.h"
#include "Prototype_Boss.h"

 // ----------------------------------------------------------------------------
 // Enum for player states
 // ----------------------------------------------------------------------------
typedef enum PlayerState
{
    IDLE,   ///< The player is idle and not moving or attacking
    MOVING, ///< The player is moving
    MELEE,  ///< The player is performing a melee attack
    RANGE,  ///< The player is performing a ranged attack
    HEAVY,  ///< The player is performing a heavy attack
    DODGE,  ///< The player is dodging
    HIT,    ///< The player has been hit
    DEAD,   ///< The player is dead
    BLOCK,  ///< The player is blocking
    BLOCKED,///< The player has been blocked
    PARRY,  ///< The player is performing a parry
    PARRIED ///< The player has been parried
} PlayerState;

// ----------------------------------------------------------------------------
// Enum for player states
// ----------------------------------------------------------------------------
typedef struct Attack
{
    Timer timer;
    Timer active_time;
    int combo;
} Attack;

extern Attack melee_attack;

// ----------------------------------------------------------------------------
// Player struct
// Holds data for player position, movement, health, state, and various timers
// ----------------------------------------------------------------------------
typedef struct Player
{
    bool canAttack;        ///< Flag to disable attacks
    AEVec2 pos;            ///< Position of the player
    AEVec2 vel;            ///< Velocity of the player
    AEVec2 dodge_vel;      ///< Velocity during a dodge
    float hp;              ///< Health of the player
    int Mod_DmgTakenIndex; ///< Damage taken multiplier
    float radius;          ///< Radius for collision checks
    aabb hitbox;           ///< Axis-aligned bounding box for collision detection
    int ammo, prevAmmo;    ///< Ammo count for ranged attacks
    float knockbackTimer;  ///< Timer for knockback effect
    float lightAttackCooldown;
    Timer dodgetimer;      ///< Timer for dodge action
    Timer hittimer;        ///< Timer for hit action
    Timer blocktimer;      ///< Timer for block action
    Timer blocked;         ///< Timer for when the player is blocked
    Timer deathtimer;
    PlayerState state;     ///< Current state of the player
    int ragemeter;         ///< Counter for successful parries
    int direction_v;       ///< Vertical direction (up/down)
    int direction_h;       ///< Horizontal direction (left/right)
} Player;


// ----------------------------------------------------------------------------
// Direction enums
// ----------------------------------------------------------------------------
enum Direction_v
{
    DOWN,  ///< Down direction
    UP     ///< Up direction
};

enum Direction_h
{
    RIGHT, ///< Right direction
    LEFT   ///< Left direction
};

// ----------------------------------------------------------------------------
// External player object
// ----------------------------------------------------------------------------
extern Player player;

// ----------------------------------------------------------------------------
// Player movement and action functions
// ----------------------------------------------------------------------------
void Movement();

// ----------------------------------------------------------------------------
// Load player sprites and values
// ----------------------------------------------------------------------------
void Load_Player();

// ----------------------------------------------------------------------------
// Initialize player values
// ----------------------------------------------------------------------------
void Init_Player();

// ----------------------------------------------------------------------------
// Update player
// ----------------------------------------------------------------------------
void Update_Player();

// ----------------------------------------------------------------------------
// Draws the player
// ----------------------------------------------------------------------------
void Draw_Player();

// ----------------------------------------------------------------------------
// Frees dynamic memory
// ----------------------------------------------------------------------------
void Free_Player();

// ----------------------------------------------------------------------------
// Unloads player textures and sprites
// ----------------------------------------------------------------------------
void Unload_Player();

// ----------------------------------------------------------------------------
// Manages player melee attack input
// ----------------------------------------------------------------------------
void Attacking();

// ----------------------------------------------------------------------------
// Checks if melee attack hits the target, return 1 if attack hits
// ----------------------------------------------------------------------------
int Attacked_Melee(AEVec2 target);

// ----------------------------------------------------------------------------
// Manages player behaviour after successful parry
// ----------------------------------------------------------------------------
void Parried_Hit();

// ----------------------------------------------------------------------------
// Manages player behaviour after blocking a hit
// Takes in parameter of how much damage
// ----------------------------------------------------------------------------
void Blocked_Hit(f32 damage);

// ----------------------------------------------------------------------------
// Manages player behaviour after getting hit
// Takes in parameter of how much damage
// ----------------------------------------------------------------------------
void Eat_Hit(f32 damage);

// ----------------------------------------------------------------------------
// Manages player ranged attack behaviour, input, projectiles
// ----------------------------------------------------------------------------
void Attacking_Ranged();

// ----------------------------------------------------------------------------
// Checks if ranged attacks hit the target, return true if attack hits
// Takes the following parameters:
// - Target position
// - Target width
// - Target height
// - Pointer to damage value of attack
// ----------------------------------------------------------------------------
bool Attacking_Ranged_Impact(AEVec2 target, f32 width, f32 height, float* damage);

// ----------------------------------------------------------------------------
// Checks if melee attacks hit a rectangular target, return true if attack hits
// Takes the following parameters:
// - Target position
// - Target width
// - Target height
// - Pointer to flag to check if hit valid
// - Pointer to damage value of attack
// ----------------------------------------------------------------------------
bool Attacked_Melee_rect(AEVec2 target, f32 width, f32 height, int* check_hit, float* damage);

// ----------------------------------------------------------------------------
// Helper function to play audio for melee attack
// ----------------------------------------------------------------------------
void PlayAttackSound(int combo);
