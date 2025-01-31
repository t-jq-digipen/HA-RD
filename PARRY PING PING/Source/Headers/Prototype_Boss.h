#pragma once
#include "Enemies.h"

typedef enum Boss_State
{
	BOSS_IDLE,
	BOSS_ATTACK,
	BOSS_PARRIED,
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
	f32 width, height;
	f32 hp, max_hp;
	Boss_State state;

	Timer idle_timer;
	Timer attack_timer;
	Timer stun_timer;

	int attack_type;
} Boss;

typedef struct Boss_Melee
{
	AEVec2 pos;
	Boss_Attack_State state;
	Timer timer;
	f32 attack_windup;
	f32 attack_active;
	f32 attack_recover;
	bool can_parry;
	bool hitbox_active;
	f32 stun_duration;

	//f32 damage;

} Boss_Melee;



void Init_Prototype_Indicator();

void Init_Prototype_Boss();

void Update_Prototype_Boss();

void Draw_Prototype_Boss();


void Prototype_Boss_Attack();

void AttackTest(bool canparry, f32 damage);