#include "enemies.h"
#include "player.h"

Color idle, windup, swing, stunned, parried;

DummyBotMelee MParryBot;

//timer for each state: idle, windup, swing, parry stun duration
const float PARRY_STUN[4] = { 
	1.f,	//idle
	1.f,	//wind up for swing
	0.5f,	//SWING
	3.f };	//if parried, this the stun duration

const float JQMonke[7] = {
	1.f,	//idle
	1.f,	//wind up for smash
	0.5f,	//SMASH
	1.f,	//rocket propeler wind up
	1.f,	//SWEEP
	1.f,	//recovery before going back to idle
	3.f		//if parried during sweep
};

void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp) {
	MParryBot.pos = pos;
	MParryBot.rad = rad;						//ik it says rad but hitbox will be a square so we just leave it as it is
	MParryBot.hp = max_hp;
	MParryBot.max_hp = max_hp;					//not sure if we want a destructable bot that will rebuild so im leaving this here
	MParryBot.state = Idle;

	MParryBot.hitbox = Init_AABB(MParryBot.pos.x, MParryBot.pos.y, MParryBot.rad, MParryBot.rad);

	MParryBot.idle_timer = Create_Timer(2); 
	MParryBot.windup_timer = Create_Timer(2);
	MParryBot.attack_timer = Create_Timer(2);
	MParryBot.stun_timer = Create_Timer(2);
}

void Active_DummyBotMelee() {
	switch (MParryBot.state) 
	{
	case Idle:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, idle);
		break;
	case WindUp:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, windup);
		break;
	case Swinging:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, swing);
		//can check for parry collision here w square below reee
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y) - 100.f, 100.f, 100.f, windup);
		break;
	}
}

void Init_Enemy_Indicator()
{
	idle = SetColor(0xFFFF6961);
	windup = SetColor(0xFFFFCF61);
	swing = SetColor(0xFFAFFF61);
	stunned = SetColor(0xFF000000);
	parried = SetColor(0xFFFF00FF);
}

void EnemyAttackPattern(Timer* ParryTimer,Timer* stunnedtimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad)
{
	if (stunnedtimer->current==0.f)
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
			DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 100.f, 100.f, 100.f, windup);
			break;
		}
	}
	else 
	{
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, stunned);
		Loop_Timer(stunnedtimer);
		/*std::cout << stunnedtimer->current << '\n';*/
	}
}
void Test_Funct(Timer* ParryTimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad )
{
	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 75.f };
	switch (*counter)
	{
	case Idle:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
		break;
	case WindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
		if (ParryTimer->current >= PARRY_STUN[WindUp] - 0.5f) {
			//temp parry eye spark/ flash effect. Replace with proper sprite later (and add audio!)
			DrawRect(World_To_Screen_x(EnemyPos.x) - 10.f, World_To_Screen_y(EnemyPos.y), 10.f, 10.f, idle);
		}
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

void JQMonkeAttackDemo(Timer* ParryTimer, int* counter, f32* stunCounter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad)
{
	std::cout << "\n\n\n\n\n JQ COUNTER: " << *counter << "\n\n\n\n\n";
	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 150.f };				//to be replaced by proper enemy's attack hitbox!!!!
	if (*counter == JQParried && ParryTimer->current >= 1.f)
	{
		++*stunCounter;
		if (*stunCounter == static_cast<int>(JQMonke[JQParried])) {
			*counter = 0;
			*stunCounter = 0;
		}
	}
	else if (*counter == JQRecovery && ParryTimer->current >= JQMonke[*counter])
	{
		*counter = 0;
		ParryTimer->current = 0.f;
	}
	else if (ParryTimer->current >= JQMonke[*counter])
	{
		++*counter;
		ParryTimer->current = 0.f;
	}

	switch (*counter)
	{
	case JQIdle:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
		break;
	case JQSmashWindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
		break;
	case JQSmash:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, swing);
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y)-150.f, HitBoxRad*2, parried);
		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
			std::cout << "U KENA JQ SMASHED!!!!!\n";
		}
		break;
	case JQSweepWindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
		break;
	case JQSweep:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, swing);
		DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 150.f, 400.f, 150.f, parried);
		if (AETestCircleToRect(&PlayerPos, PlayerRad, &TempHitbox, 400.f, 150.f)) {
			std::cout << "U KENA JQ SWEEPED!!!!!\n";
			*counter = JQParried;
		}
		break;
	case JQRecovery:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
		break;
	case JQParried:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, parried);
		break;
	}
}