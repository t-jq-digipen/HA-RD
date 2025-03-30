/*!
 * \file    player.h
 * \author  TAN JUN QUAN, MUHAMMAD FAUZAN BIN MOHAMED FAUZY
 * \par     t.junquan@digipen.edu, muhammadfauzan.b@digipen.edu
 * \brief   Declaration of player struct and movement functions.
 *
 * This file contains declarations for the `Player` struct, various player states,
 * and functions related to player movement, attacks, and other mechanics.
 *
 * Copyright © 2020 DigiPen, All rights reserved.
 */

#pragma once
#include "utils.h"
#include "Camera.h"
#include "UI.h"
#include "Prototype_Boss.h"

 // ----------------------------------------------------------------------------
 // Enum for player states
 // ----------------------------------------------------------------------------
 /*!
  * \brief Enumeration representing different player states.
  */
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
/*!
 * \brief struct for player melee attack.
 */
typedef struct Attack
{
    Timer timer;
    Timer active_time;
    int combo;
} Attack;

extern Attack melee_attack;

// ----------------------------------------------------------------------------
// Player struct
// ----------------------------------------------------------------------------
/*!
 * \brief Struct representing the player.
 *
 * This struct holds data related to the player's position, movement, health,
 * state, and various player timers. It also includes pointers to related
 * sprites and other game objects.
 */
typedef struct Player
{
    bool canAttack;        ///< Flag to disable attacks
    AEVec2 pos;            ///< Position of the player
    AEVec2 vel;            ///< Velocity of the player
    float speed;           ///< Speed of the player
    AEVec2 dodge_vel;      ///< Velocity during a dodge
    float hp;              ///< Health of the player
    int Mod_DmgTakenIndex; ///< Damage taken multiplier
    float radius;          ///< Radius for collision checks
    float width, height;   ///< Dimensions of the player
    aabb hitbox;           ///< Axis-aligned bounding box for collision detection
    int ammo;              ///< Ammo count for ranged attacks
    float knockbackTimer;  ///< Timer for knockback effect
    float lightAttackCooldown;
    Timer dodgetimer;      ///< Timer for dodge action
    Timer hittimer;        ///< Timer for hit action
    Timer blocktimer;      ///< Timer for block action
    Timer blocked;         ///< Timer for when the player is blocked
    PlayerState state;     ///< Current state of the player
    int ragemeter;         ///< Counter for successful parries
    int ragemeter_max;     ////< max counter for rage meter
    int direction_v;       ///< Vertical direction (up/down)
    int direction_h;       ///< Horizontal direction (left/right)
    Color color;           ///< Color of the player (for rendering)
    SpriteSheet_Old* spritesheet; ///< Pointer to the sprite sheet for animation
} Player;

// ----------------------------------------------------------------------------
// Player timers struct
// ----------------------------------------------------------------------------
/*!
 * \brief Struct holding various timers for player actions.
 *
 * This struct includes timers for actions such as dodging, hitting, and blocking.
 */
typedef struct PlayerTimers
{
    Timer dodge;           ///< Timer for dodge action
    Timer hit;             ///< Timer for hit action
    Timer blockparry;      ///< Timer for block or parry action
    Timer blocked;         ///< Timer for when the player is blocked
} PlayerTimers;

// ----------------------------------------------------------------------------
// Direction enums
// ----------------------------------------------------------------------------
/*!
 * \brief Enumeration for vertical direction (up/down).
 */
enum Direction_v
{
    DOWN,  ///< Down direction
    UP     ///< Up direction
};

/*!
 * \brief Enumeration for horizontal direction (left/right).
 */
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
/*!
 * \brief Handles player movement input and updates position.
 */
void Movement();

/*!
 * \brief Loads player resources (textures, sprites).
 */
void Load_Player();

/*!
 * \brief Initializes player properties and variables.
 */
void Init_Player();

/*!
 * \brief Updates player states and actions.
 */
void Update_Player();

/*!
 * \brief Draws the player to the screen.
 */
void Draw_Player();

/*!
 * \brief Frees any dynamically allocated memory for the player.
 */
void Free_Player();

/*!
 * \brief Unloads player resources (textures, sprites).
 */
void Unload_Player();

/*!
 * \brief Handles attacking actions for the player.
 */
void Attacking();

/*!
 * \brief Checks if a melee attack hit a target.
 * \param target The position of the target to check for collision.
 * \return 1 if the attack hits, 0 otherwise.
 */
int Attacked_Melee(AEVec2 target);

/*!
 * \brief Handles the behavior when the player is parried.
 */
void Parried_Hit();

/*!
 * \brief Handles the behavior when the player is blocked.
 * \param damage The amount of damage blocked.
 */
void Blocked_Hit(f32 damage);

/*!
 * \brief Applies damage to the player when hit.
 * \param damage The amount of damage to apply.
 */
void Eat_Hit(f32 damage);

/*!
 * \brief Handles ranged attacking actions for the player.
 */
void Attacking_Ranged();

/*!
 * \brief Checks if a ranged attack hits a target.
 * \param target The position of the target.
 * \param width The width of the hitbox.
 * \param height The height of the hitbox.
 * \param damage Pointer to the damage value of the attack.
 * \return True if the attack hits the target, false otherwise.
 */
bool Attacking_Ranged_Impact(AEVec2 target, f32 width, f32 height, float* damage);

/*!
 * \brief Checks if a melee attack hits a target's hitbox.
 * \param target The position of the target.
 * \param width The width of the target's hitbox.
 * \param height The height of the target's hitbox.
 * \param check_hit Pointer to a flag to indicate if the hit is valid.
 * \param damage Pointer to the damage value of the attack.
 * \return True if the attack hits the target, false otherwise.
 */
bool Attacked_Melee_rect(AEVec2 target, f32 width, f32 height, int* check_hit, float* damage);
