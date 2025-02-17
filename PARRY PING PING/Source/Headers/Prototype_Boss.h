#pragma once
#include "Enemies.h"

typedef enum Boss_State
{
	BOSS_IDLE,
	BOSS_ATTACK,
	BOSS_PARRIED,
	BOSS_DASH,
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
	MONKE_SMASH,
	MONKE_SWEEP,
} Boss_AttackType;

typedef struct Boss
{
	AEVec2 pos;
	AEVec2 target_pos;
	f32 width, height;
	f32 hp, max_hp;
	Boss_State state;
	Boss_Attack_State attackstate;

	aabb hitbox;

	Timer idle_timer;
	Timer attack_timer;
	Timer stun_timer;

	Timer dash_timer;
	Timer dash_cooldown_timer;

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

	//f32 damage;

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


void Prototype_Boss_Attack();

void Proto_Monke_Attack_Test(bool canparry, f32 damage);

void Proto_Monke_Range_Attack(const AEVec2& start, const AEVec2& direction);

void Draw_Proto_Monke_Death_Explosion(AEVec2& start);