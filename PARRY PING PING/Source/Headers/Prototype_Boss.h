#pragma once
#include "Enemies.h"

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

typedef enum Boss_Attack_State
{

	BOSS_ATTACK_WINDUP,
	BOSS_ATTACK_ACTIVE,
	BOSS_ATTACK_RECOVER,

} Boss_Attack_State;

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

typedef struct Boss
{

	AEVec2 pos;
	AEVec2 target_pos;
	f32 target_dist;
	f32 target_rot;
	AEVec2 target_dir;
	f32 width, height;
	f32 hp, max_hp;
	Boss_State state;
	Boss_Attack_State attackstate;

	aabb hitbox;

	Timer idle_timer;
	Timer attack_timer;
	Timer stun_timer;

	Timer range_warning_timer;
	Timer range_timer;
	bool isShootingWarningActive;
	bool isShooting;

	int attack_type;

	Timer explosion_timer;
	Timer death_explosion_duration;

} Boss;

typedef struct Boss_Melee
{

	AEVec2 pos;
	Timer timer;
	f32 attack_windup;
	f32 attack_active;
	f32 attack_recover;
	bool can_parry;
	bool hitbox_active;
	f32 stun_duration;

} Boss_Melee;

typedef struct Boss_Range
{

	AEVec2 pos;
	AEVec2 velocity;
	AEMtx33 transform;
	Timer timer;
	bool active;

} Boss_Range;

typedef struct Boss_Explosion
{

	AEVec2 pos;
	AEMtx33 transform;
	Timer timer;
	bool active;

} Boss_Explosion;

void Load_Prototype_Boss();

void Init_Prototype_Indicator();

void Init_Prototype_Boss();

void Update_Prototype_Boss();

void Draw_Prototype_Boss();

void Unload_Prototype_Boss();

void Smash_Sweep_Logic();

void Three_Hit_Logic();

void Boss_Dash_Logic();

void Boss_Rocket_Logic();

void Proto_Monke_Attack_Test(bool canparry, f32 damage);

void Proto_Monke_Range_Attack(const AEVec2& start, const AEVec2& direction);

void Draw_Proto_Monke_Death_Explosion(AEVec2& start);