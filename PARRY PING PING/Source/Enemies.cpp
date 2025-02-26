#include "enemies.h"
#include "player.h"

Color color_idle, color_windup, color_swing, color_stunned, color_parried;

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

	MParryBot.idle_timer = Create_Timer(1); 
	MParryBot.windup_timer = Create_Timer(1);
	MParryBot.attack_timer = Create_Timer(1);
	MParryBot.stun_timer = Create_Timer(3);
	MParryBot.respawn_timer = Create_Timer(3);
}

void Active_DummyBotMelee() {
	std::cout << "state " << MParryBot.state << '\n';
	if (MParryBot.state != Destroyed) {
		//MParryBot.state = EnemyAttackPatternState(MParryBot.count);
		switch (MParryBot.state)
		{
		case Idle:
			DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_idle);
			if (Update_Timer(&MParryBot.idle_timer)) {
				//timer has not ended
			}
			else {
				MParryBot.windup_timer.current = 0.f;
				MParryBot.state = WindUp;
			}
			break;
		case WindUp:
			DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_windup);
			if (Update_Timer(&MParryBot.windup_timer)) {
				//timer has not ended
			}
			else {
				MParryBot.attack_timer.current = 0.f;
				MParryBot.state = Swinging;
			}
			break;
		case Swinging:
			DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_swing);
			if (Update_Timer(&MParryBot.attack_timer)) {
				//timer has not ended
			}
			else {
				MParryBot.idle_timer.current = 0.f;
				MParryBot.state = Idle;
			}
			//CHECK FOR PARRY HERE!!!
			DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y) - 100.f, 100.f, 100.f, color_windup);
			break;
		case Parried:
			if (!Update_Timer(&MParryBot.stun_timer)) {
				MParryBot.state = Idle;
			}
		}

	}
	else {
		//wait for timer to reset, play animation of respawn
		if (Update_Timer(&MParryBot.respawn_timer)) {
			//timer has not ended
		}
		else {
			MParryBot.respawn_timer.current = 0.f;
			MParryBot.state = Idle;
		}
	}
}

void Init_Enemy_Indicator()
{
	color_idle = SetColor(0xFFFF6961);
	color_windup = SetColor(0xFFFFCF61);
	color_swing = SetColor(0xFFAFFF61);
	color_stunned = SetColor(0xFF000000);
	color_parried = SetColor(0xFFFF00FF);
}

//!!!!!
//OUTDATED CODE! WILL REMOVE ONCE PARRY BOT FINALISED!
//!!!!!
//void EnemyAttackPattern(Timer* ParryTimer,Timer* stunnedtimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad)
//{
//	if (stunnedtimer->current==0.f)
//	{
//		if (ParryTimer->current >= 1.f)
//		{
//			if (*counter == Swinging && ParryTimer->current >= 1.f)
//			{
//				*counter = 0;
//			}
//			else
//			{
//				++*counter;
//			}
//		}
//
//		switch (*counter)
//		{
//		case Idle:
//			DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_idle);
//			break;
//		case WindUp:
//			DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_windup);
//			break;
//		case Swinging:
//			DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_swing);
//			//can check for parry collision here w square below reee
//			DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 100.f, 100.f, 100.f, color_windup);
//			break;
//		}
//	}
//	else 
//	{
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_stunned);
//		Loop_Timer(stunnedtimer);
//		/*std::cout << stunnedtimer->current << '\n';*/
//	}
//}
void Test_Funct(Timer* ParryTimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad )
{
	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 75.f };
	switch (*counter)
	{
	case Idle:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_idle);
		break;
	case WindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_windup);
		if (ParryTimer->current >= PARRY_STUN[WindUp] - 0.5f) {
			//temp parry eye spark/ flash effect. Replace with proper sprite later (and add audio!)
			DrawRect(World_To_Screen_x(EnemyPos.x) - 10.f, World_To_Screen_y(EnemyPos.y), 10.f, 10.f, color_idle);
		}
		break;
	case Swinging:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_swing);
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 75.f, HitBoxRad*2, color_parried);
		//To be replaced with player's attack
		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
			std::cout << "!!!!!PARRYABLE!!!!!\n";
			*counter = Parried;
		}
		break;
	case Parried:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_parried);
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
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_idle);
		break;
	case JQSmashWindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_windup);
		break;
	case JQSmash:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_swing);
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y)-150.f, HitBoxRad*2, color_parried);
		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
			std::cout << "U KENA JQ SMASHED!!!!!\n";
		}
		break;
	case JQSweepWindUp:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_windup);
		break;
	case JQSweep:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_swing);
		DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 150.f, 400.f, 150.f, color_parried);
		if (AETestCircleToRect(&PlayerPos, PlayerRad, &TempHitbox, 400.f, 150.f)) {
			std::cout << "U KENA JQ SWEEPED!!!!!\n";
			*counter = JQParried;
		}
		break;
	case JQRecovery:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_idle);
		break;
	case JQParried:
		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_parried);
		break;
	}
}