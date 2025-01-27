#pragma once
#include "utils.h"
#include "Camera.h"

//Early Implementation of enemy attacks ignore the ugly ass name
typedef enum EnemyAttackPatternState
{
	Idle,
	WindUp,
	Swinging

}EnemyAttackPatternState;

//Dummy attack pattern

void Init_Enemy_Indicator();
void EnemyAttackPattern(Timer* ParryTimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad);