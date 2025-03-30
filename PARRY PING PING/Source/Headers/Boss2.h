//---------------------------------------------------------
// file:	SpriteSheet.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Spritesheet class declaration
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "utils.h"
#include "player.h"
#include "UI.h"
#include <vector>

// ----------------------------------------------------------------------------
// Enumeration for Boss phases
// ----------------------------------------------------------------------------
typedef enum Boss2_Phase
{
    PHASE1,
    PHASE2,
    PHASE3,
} Boss2_Phase;

// ----------------------------------------------------------------------------
// Enumeration for Boss states
// ----------------------------------------------------------------------------
typedef enum Boss2_State
{
    BOSS2_IDLE,
    BOSS2_PARRIED,
    BOSS2_PHASEBREAK,
    BOSS2_DEAD,


    BOSS2_MISSILE_BARRAGE,
    BOSS2_DASH_SLASH,
    BOSS2_LIGHTNING_WAVE,

} Boss2_State;

// ----------------------------------------------------------------------------
// Enumeration for Boss attack states
// ----------------------------------------------------------------------------
typedef enum Boss2_Attack_State
{
    BOSS2_ATTACK_WINDUP,
    BOSS2_ATTACK_ACTIVE,
    BOSS2_ATTACK_RECOVER,
} Boss2_Attack_State;

// ----------------------------------------------------------------------------
// Enumeration for Boss attack types
// ----------------------------------------------------------------------------
typedef enum Boss2_AttackType
{
    MISSILE_BARRAGE,

    DASH,
    DASH_SLASH1,
    DASH_SLASH2,

    LIGHTNING_WAVE,
} Boss2_AttackType;

// ----------------------------------------------------------------------------
// Structure representing the Boss
// ----------------------------------------------------------------------------
typedef struct Boss2
{
    AEVec2 pos;                 // Position of the Boss
    AEVec2 target_pos;          // Target position for attacks
    f32 target_dist;            // Distance to the target
    f32 target_rot;             // Rotation angle towards the target
    AEVec2 target_dir;          // Direction vector towards the target
    AEVec2 vel;                 // Velocity
    f32 width, height;          // Dimensions of the Boss
    f32 hp, max_hp;             // Current and maximum health points
    Boss2_Phase phase;          // Current phase the boss is in
    Boss2_State state;           // Current state of the Boss
    Boss2_Attack_State attackstate; // Current attack state
    aabb hitbox;                // Hitbox for collision detection

    Timer idle_timer;           // Timer for idle state
    Timer attack_timer;         // Timer for attack states
    Timer stun_timer;           // Timer for stun states
    Timer missile_prep;         // Timer to prep missiles
    Timer phasebreak_timer;     // Timer for phase breaks

    Boss2_AttackType attack_type;            // Current attack type

} Boss2;

extern Boss2 boss_2;

// ----------------------------------------------------------------------------
// Structure representing a Boss attack
// ----------------------------------------------------------------------------
typedef struct Boss2_Attack
{
    AEVec2 pos;                 // Position of the attack
    Timer timer;                // Timer for the attack
    f32 attack_windup;          // Windup time for the attack
    f32 attack_active;          // Active time for the attack
    f32 attack_recover;         // Recovery time after the attack
    bool can_parry;             // Whether the attack can be parried
    bool hitbox_active;         // Whether the hitbox is active
    f32 stun_duration;          // Duration of stun if parried
} Boss2_Attack;

// ----------------------------------------------------------------------------
// Structure representing missiles
// ----------------------------------------------------------------------------
typedef struct Missile {
    AEVec2 pos;                 // Position of the missile
    AEVec2 targetpos;           // Targets for the missiles
    AEVec2 velocity;            // Velocity of the missile
    f32 radius;                 // Radius of the missile
    bool active;                // Whether the missile is active

} Missile;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
extern void Load_Boss2();               // Load Boss resources
extern void Init_Boss2_Indicator();     // Initialize Boss indicators
extern void Init_Boss2();               // Initialize Boss state
extern void Update_Boss2();             // Update Boss logic
extern void Draw_Boss2_Hitboxes();      // Draw boss attack hitboxes
extern void Draw_Boss2();               // Draw Boss and its effects
extern void Unload_Boss2();             // Unload Boss resources
void Boss2_Attack_Test(bool canparry, f32 boss_damage); // Test for Boss attack collision
AEVec2 GetRandomTargetPosAroundPlayer();
void Dash_Slash_Logic();
void Missile_Barrage_Logic();
void Lightning_Wave_Logic();