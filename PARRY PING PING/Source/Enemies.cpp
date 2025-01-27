#include "enemies.h"

Color idle, windup, swing;

void Init_Enemy_Indicator()
{
	idle = SetColor(0xFFFF6961);
	windup = SetColor(0xFFFFCF61);
	swing = SetColor(0xFFAFFF61);
}

void EnemyAttackPattern(Timer* ParryTimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad) 
{
	if (ParryTimer->current >= 1.f)
	{
		if (*counter == Swinging && ParryTimer->current >= 1.f)
		{
			*counter = 0;
		}
		else
		{
			++*counter;
		}
	}

	switch (*counter)
	{
	case Idle:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
		break;
	case WindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
		break;
	case Swinging:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, swing);
		//can check for parry collision here w square below reee
		//DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 100.f, 100.f, 100.f, windup);
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 75.f, HitBoxRad*2, SetColor(0xFFFF00FF));
		if (AETestCircleToCircle(&EnemyPos, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
			std::cout << "!!!!!PARRYABLE!!!!!\n";
		}
		break;
	}
}