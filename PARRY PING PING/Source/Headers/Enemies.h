#pragma once
#include "utils.h"
#include "Camera.h"
#include "player.h"

//Early Implementation of enemy attacks ignore the ugly ass name
typedef enum EnemyAttackPatternState
{
	Idle,
	WindUp,
	Swinging

}EnemyAttackPatternState;

//Dummy attack pattern

void Init_Enemy_Indicator();
void EnemyAttackPattern(Timer* ParryTimer, Timer* Stunned, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, Player* player);