//---------------------------------------------------------
// File:	Boss1.cpp
// Project: Heavy Assault : Rust & Dust
// Authors:	
//          Muhammad Raziq bin Mohd Rafig ------------- 85%
//          - Boss logic
//          - Boss attacks
// 
//          Manfred Sim ------------------------------- 15%
//          - Death Explosion
//
// Brief:	Boss 1 declaration
//          Declares the functions for boss 1, and various
//          states and structs for the boss update logics.
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------
#pragma once
#include "Enemies.h"
#include <vector>
// ----------------------------------------------------------------------------
// Enumeration for Boss states
// ----------------------------------------------------------------------------
typedef enum Boss_State
{
    BOSS_IDLE,
    BOSS_SMASH_SWEEP,
    BOSS_THREE_HIT,
    BOSS_RANGED_CHARGE,
    BOSS_RANGED_ROCKETFIST,
    BOSS_PARRIED,
    BOSS_DEAD,
} Boss_State;

// ----------------------------------------------------------------------------
// Enumeration for Boss attack states
// ----------------------------------------------------------------------------
typedef enum Boss_Attack_State
{
    BOSS_ATTACK_WINDUP,
    BOSS_ATTACK_ACTIVE,
    BOSS_ATTACK_RECOVER,
} Boss_Attack_State;

// ----------------------------------------------------------------------------
// Enumeration for Boss attack types
// ----------------------------------------------------------------------------
typedef enum Boss_AttackType
{
    BOSS_SMASH,
    BOSS_SWEEP,
    BOSS_LEFT_HIT,
    BOSS_RIGHT_HIT,
    BOSS_CIRCLE_HIT,
    BOSS_CHARGE,
    BOSS_ROCKETFIST,
} Boss_AttackType;

// ----------------------------------------------------------------------------
// Structure representing the Boss
// ----------------------------------------------------------------------------
typedef struct Boss
{
    AEVec2 pos;                 // Position of the Boss
    AEVec2 target_pos;          // Target position for attacks
    f32 target_dist;            // Distance to the target
    f32 target_rot;             // Rotation angle towards the target
    AEVec2 target_dir;          // Direction vector towards the target
    f32 width, height;          // Dimensions of the Boss
    f32 hp, max_hp;             // Current and maximum health points
    Boss_State state;           // Current state of the Boss
    Boss_Attack_State attackstate; // Current attack state
    aabb hitbox;                // Hitbox for collision detection

    Timer idle_timer;           // Timer for idle state
    Timer attack_timer;         // Timer for attack states
    Timer stun_timer;           // Timer for stun states

    int attack_type;            // Current attack type

    bool explosion_triggered;    // Whether an explosion effect is active  
    Timer explosion_timer;      // Timer for explosion effects
    Timer death_explosion_duration; // Timer for death explosion duration
} Boss;

extern Boss boss_1;

// ----------------------------------------------------------------------------
// Structure representing a Boss melee attack
// ----------------------------------------------------------------------------
typedef struct Boss_Melee
{
    AEVec2 pos;                 // Position of the attack
    Timer timer;                // Timer for the attack
    f32 attack_windup;          // Windup time for the attack
    f32 attack_active;          // Active time for the attack
    f32 attack_recover;         // Recovery time after the attack
    bool can_parry;             // Whether the attack can be parried
    bool hitbox_active;         // Whether the hitbox is active
    f32 stun_duration;          // Duration of stun if parried
} Boss_Melee;

// ----------------------------------------------------------------------------
// Structure representing a Rocket Fist projectile
// ----------------------------------------------------------------------------
typedef struct RocketFist
{
    AEVec2 pos;                 // Position of the projectile
    AEVec2 velocity;            // Velocity of the projectile
    f32 radius;                 // Radius of the projectile
    bool active;                // Whether the projectile is active
    Timer timer;                // Timer for the projectile
    AEMtx33 transform;          // Transformation matrix for rendering
} RocketFist;

// ----------------------------------------------------------------------------
// Structure representing a Boss explosion effect
// ----------------------------------------------------------------------------
typedef struct Boss_Explosion
{
    AEVec2 pos;                 // Position of the explosion
    AEMtx33 transform;          // Transformation matrix for rendering
    Timer timer;                // Timer for the explosion
    bool active;                // Whether the explosion is active
} Boss_Explosion;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void Load_Boss();               // Load Boss resources
void Init_Boss_Indicator();     // Initialize Boss indicators
void Init_Boss();               // Initialize Boss state
void Update_Boss();             // Update Boss logic
void Draw_Boss_Hitboxes();      // Draw boss attack hitboxes
void Draw_Boss();               // Draw Boss and its effects
void Unload_Boss();             // Unload Boss resources

void Smash_Sweep_Logic();       // Logic for Smash and Sweep attacks
void Three_Hit_Logic();         // Logic for Three Hit combo
void Boss_Dash_Logic();         // Logic for Dash attack
void Boss_Rocket_Logic();       // Logic for Rocket Fist attack
void Boss_Attack_Test(bool canparry, f32 boss_damage); // Test for Boss attack collision
void Draw_Boss_Death_Explosion(AEVec2& start); // Draw Boss death explosion