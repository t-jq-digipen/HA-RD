#pragma once
#include "utils.h"
#include "Camera.h"
#include "player.h"

//Early Implementation of enemy attacks ignore the ugly ass name
typedef enum EnemyAttackPatternState
{
	Idle = 0,
	WindUp = 1,
	Swinging = 2,
	Parried = 3,
	Destroyed = 4

}EnemyAttackPatternState;

//JQ Monke Attack Pattern
typedef enum JQMonkeAttackPatternState
{
	JQIdle = 0,			//idle
	JQSmashWindUp = 1,	//wind up for smash
	JQSmash = 2,			//SMASH
	JQSweepWindUp = 3,	//rocket propeler wind up
	JQSweep = 4,		//SWEEP
	JQRecovery = 5,		//recovery before going back to idle
	JQParried = 6		//if parried during sweep
}JQMonkeAttackPatternState;

typedef struct DummyBotMelee
{
	AEVec2 pos, attackpos;
	f32 rad;
	f32 hp, max_hp;					//not sure if we want a destructable bot that will rebuild so im leaving this here
	f32 damage;
	f32 AttackHitboxWidth, AttackHitboxHeight;
	EnemyAttackPatternState state;
	enum FacingDir {
		FacingLeft = 0, FacingRight = 1, FacingUp = 2, FacingDown = 3
	}FacingDir;

	aabb hitbox;
	f32 hitboxWidth, hitboxHeight;

	Timer idle_timer, windup_timer, attack_timer, stun_timer, respawn_timer;
}DummyBotMelee;

typedef struct DummyBotRange
{
	AEVec2 pos, attackpos;
	f32 rad;
	f32 hp, max_hp;					//not sure if we want a destructable bot that will rebuild so im leaving this here
	f32 damage;
	f32 firerate;
	f32 AttackHitboxWidth, AttackHitboxHeight;
	EnemyAttackPatternState state;
	enum FacingDir {
		FacingLeft = 0, FacingRight = 1, FacingUp = 2, FacingDown = 3
	}FacingDir;

	aabb hitbox;
	f32 hitboxWidth, hitboxHeight;

	Timer idle_timer, windup_timer, attack_timer, stun_timer, respawn_timer, projectile_expiry_timer;
}DummyBotRange;


void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp);
void Active_DummyBotMelee();
void Attack_DummyBotMelee();

void Init_DummyBotRange(AEVec2 pos, f32 rad, f32 max_hp, f32 firerate);
void Active_DummyBotRange();
void Attack_DummyBotRange();



//Dummy attack pattern

void Init_Enemy_Indicator();
//void EnemyAttackPattern(Timer* ParryTimer, Timer* Stunned, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, Player* player);

void JQMonkeAttackDemo(Timer* ParryTimer, int* counter, f32* StunCounter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad);