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
    PHASE1 = 0,
    PHASE2 = 1,
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

    LIGHTNING_STAB,
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
    Timer death_timer;          // Timer for death or phasebreak

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
struct Missile {
    AEVec2 pos = { 0,0 };
    AEVec2 velocity = { 0,0 };
    AEVec2 targetpos = { 0,0 };
    f32 width = 50.f;  // Default size
    f32 height = 50.f; // Default size
    f32 targetradius = 50.f;
    Timer timer;
    bool active = false;
    bool locked = false;

    // Constructor
    Missile() = default;
};

// ----------------------------------------------------------------------------
// Structure representing lightning wave
// ----------------------------------------------------------------------------
typedef struct LightningWave {
    AEVec2 pos;                 // Position of the wave
    f32 radius;
    f32 velocity;
    Timer timer;
}LightningWave;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void Load_Boss2();               // Load Boss resources
void Init_Boss2_Indicator();     // Initialize Boss indicators
void Init_Boss2();               // Initialize Boss state
void Update_Boss2();             // Update Boss logic
void Draw_Boss2_Hitboxes();      // Draw boss attack hitboxes
void Draw_Boss2();               // Draw Boss and its effects
void Unload_Boss2();             // Unload Boss resources
AEVec2 GetRandomTargetPosAroundPlayer();
void Dash_Slash_Logic();
void Missile_Barrage_Logic();
void Update_Missiles();
void Lightning_Wave_Logic(); 
void Update_Lightning_Wave();