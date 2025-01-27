#include "enemies.h"
#include "utils.h"

Color idle, windup, swing, parried;

//timer for each state: idle, windup, swing, parry stun duration
const float PARRY_STUN[4] = { 1.f, 1.f, 0.5f, 3.f };

void Init_Enemy_Indicator()
{
	idle = SetColor(0xFFFF6961);
	windup = SetColor(0xFFFFCF61);
	swing = SetColor(0xFFAFFF61);
	parried = SetColor(0xFFFF00FF);
}

void EnemyAttackPattern(Timer* ParryTimer, int* counter, f32* stunCounter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad)
{
	f32 StunTimer{ 3.f };
	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 75.f };				//to be replaced by proper enemy's attack hitbox!!!!
	if (ParryTimer->current >= 1.f)
	{
		if (*counter == Parried && ParryTimer->current >= 1.f)
		{
			++*stunCounter;
			if (*stunCounter == StunTimer) {
				*counter = 0;
				*stunCounter = 0;
			}
		}
		else if (*counter == Swinging && ParryTimer->current >= 1.f)
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
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 75.f, HitBoxRad*2, parried);
		//To be replaced with player's attack
		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
			std::cout << "!!!!!PARRYABLE!!!!!\n";
			*counter = Parried;
		}
		break;
	case Parried:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, parried);
		std::cout << "ahhhhhh cb i kena parried\n";
		break;
	}
}