#include "enemies.h"
#include "player.h"

Color color_idle, color_windup, color_swing, color_stunned, color_parried;

DummyBotMelee MParryBot;		//Melee Practice Range Parry Bot
DummyBotRange RParryBot;		//Ranged Attack

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

//MELEE PARRY BOT  -  Init, Active (Update), Attack 
void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp) {
	MParryBot.pos = pos;
	MParryBot.FacingDir = MParryBot.FacingDown;								//Init here which dir bot should face!!!
	switch (MParryBot.FacingDir) {
		case MParryBot.FacingLeft:
			MParryBot.attackpos = AEVec2{ pos.x - 100, pos.y };
			break;
		case MParryBot.FacingRight:
			MParryBot.attackpos = AEVec2{ pos.x + 100, pos.y };
			break;
		case MParryBot.FacingUp:
			MParryBot.attackpos = AEVec2{ pos.x + 0, pos.y + 100.f };
			break;
		case MParryBot.FacingDown:
			MParryBot.attackpos = AEVec2{ pos.x + 0, pos.y - 100.f };
			break;
		default:
			MParryBot.attackpos = AEVec2{pos.x + 0, pos.y - 100.f};
			break;
	}
	MParryBot.rad = rad;						//ik it says rad but hitbox will be a square so we just leave it as it is
	MParryBot.hp = max_hp;
	MParryBot.max_hp = max_hp;					//not sure if we want a destructable bot that will rebuild so im leaving this here
	MParryBot.damage = 10.f;
	MParryBot.AttackHitboxWidth = 150.f;
	MParryBot.AttackHitboxHeight = 75.f;
	MParryBot.state = Idle;

	MParryBot.hitbox = Init_AABB(MParryBot.pos.x, MParryBot.pos.y, MParryBot.rad, MParryBot.rad);
	MParryBot.hitboxWidth = MParryBot.hitbox.max.x - MParryBot.hitbox.min.x;
	MParryBot.hitboxHeight = MParryBot.hitbox.max.y - MParryBot.hitbox.min.y;

	MParryBot.idle_timer = Create_Timer(1); 
	MParryBot.windup_timer = Create_Timer(1);
	MParryBot.attack_timer = Create_Timer(1);
	MParryBot.stun_timer = Create_Timer(3);
	MParryBot.respawn_timer = Create_Timer(3);
}

int MParryBot_last_hit = 0;
int MParryBot_new_hit = 0;

void Active_DummyBotMelee() {
	//std::cout << "state " << MParryBot.state << '\n';
	if (MParryBot.state != Destroyed) {
		std::cout << MParryBot.hp << '\n';

		// Reset last hit if no new melee hit detected
		if (!MParryBot_new_hit)
		{
			MParryBot_last_hit = 0;
		}

		if (Attacked_Melee_rect(MParryBot.pos, MParryBot.hitboxWidth, MParryBot.hitboxHeight, &MParryBot_new_hit, &MParryBot.damage))
		{
			if (MParryBot_last_hit != MParryBot_new_hit)
			{
				if (player.ammo < 3)
				{
					player.ammo++;
				}
				MParryBot.hp -= MParryBot.damage;
				MParryBot_last_hit = MParryBot_new_hit;
			}
		}

		if (Attacking_Ranged_Impact(MParryBot.pos, MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight, &MParryBot.damage))
		{
			MParryBot.hp -= MParryBot.damage;
		}


		if (MParryBot.hp <= 0) {
			MParryBot.respawn_timer.current = 0.f;
			MParryBot.state = Destroyed;
		}

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
			Attack_DummyBotMelee();
			if (Update_Timer(&MParryBot.attack_timer)) {
				//timer has not ended
			}
			else {
				MParryBot.idle_timer.current = 0.f;
				MParryBot.state = Idle;
			}
			break;
		case Parried:
			DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_parried);
			if (Update_Timer(&MParryBot.stun_timer)) {
				//timer has not ended
			}
			else {
				MParryBot.idle_timer.current = 0.f;
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
			MParryBot.idle_timer.current = 0.f;
			MParryBot.hp = MParryBot.max_hp;
			MParryBot.state = Idle;
		}
	}
}

void Attack_DummyBotMelee() {
	DrawRect(World_To_Screen_x(MParryBot.attackpos.x), World_To_Screen_y(MParryBot.attackpos.y), MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight, color_windup);			//draw 'hitbox'
	if (AETestCircleToRect(&player.pos, player.radius, &MParryBot.attackpos, MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight)) {
		switch (player.state) {
		case PARRY:
			//stun logic
			MParryBot.stun_timer.current = 0.f;
			MParryBot.state = Parried;
			break;
		case BLOCK:
			Blocked_Hit(MParryBot.damage);
			break;
		case PARRIED:
		case BLOCKED:
		case HIT:
		case DODGE:
			// do nothing, immune
			break;
		default:
			Eat_Hit(MParryBot.damage);
		}
	}
}


//RANGED PARRY BOT  -  Init, Active (Update), Attack
void Init_DummyBotRange(AEVec2 pos, f32 rad, f32 max_hp) {
	RParryBot.pos = pos;
	RParryBot.FacingDir = RParryBot.FacingDown;								//Init here which dir bot should face!!!
	switch (RParryBot.FacingDir) {
	case RParryBot.FacingLeft:
		RParryBot.attackpos = AEVec2{ pos.x - 50, pos.y };
		break;
	case RParryBot.FacingRight:
		RParryBot.attackpos = AEVec2{ pos.x + 50, pos.y };
		break;
	case RParryBot.FacingUp:
		RParryBot.attackpos = AEVec2{ pos.x + 0, pos.y + 50.f };
		break;
	case RParryBot.FacingDown:
		RParryBot.attackpos = AEVec2{ pos.x + 0, pos.y - 50.f };
		break;
	default:
		RParryBot.attackpos = AEVec2{ pos.x + 0, pos.y - 50.f };
		break;
	}

	RParryBot.rad = rad;						//ik it says rad but hitbox will be a square so we just leave it as it is
	RParryBot.hp = max_hp;
	RParryBot.max_hp = max_hp;					//not sure if we want a destructable bot that will rebuild so im leaving this here
	RParryBot.damage = 10.f;
	RParryBot.projectile_speed = 350.f;
	RParryBot.projectile_radius = 20.f;
	RParryBot.projectile_range = 500.f;
	RParryBot.projectile_parry = false;
	RParryBot.AttackHitboxWidth = 150.f;
	RParryBot.AttackHitboxHeight = 75.f;
	RParryBot.state = Idle;

	RParryBot.hitbox = Init_AABB(RParryBot.pos.x, RParryBot.pos.y, RParryBot.rad, RParryBot.rad);
	RParryBot.hitboxWidth = RParryBot.hitbox.max.x - RParryBot.hitbox.min.x;
	RParryBot.hitboxHeight = RParryBot.hitbox.max.y - RParryBot.hitbox.min.y;
	RParryBot.projectile_pos = RParryBot.attackpos;

	RParryBot.idle_timer = Create_Timer(1);
	RParryBot.windup_timer = Create_Timer(1);
	RParryBot.attack_timer = Create_Timer(3);
	RParryBot.stun_timer = Create_Timer(3);
	RParryBot.respawn_timer = Create_Timer(3);
}


int RParryBot_last_hit = 0;
int RParryBot_new_hit = 0;

void Active_DummyBotRange() {
	std::cout << "range bot state " << RParryBot.state << '\n';
	if (RParryBot.state != Destroyed) {
		std::cout << RParryBot.hp << '\n';
		std::cout << AEVec2Distance(&RParryBot.pos, &RParryBot.projectile_pos) << '\n';

		// Reset last hit if no new melee hit detected
		if (!RParryBot_new_hit)
		{
			RParryBot_last_hit = 0;
		}

		if (Attacked_Melee_rect(RParryBot.pos, RParryBot.hitboxWidth, RParryBot.hitboxHeight, &RParryBot_new_hit, &RParryBot.damage))
		{
			if (RParryBot_last_hit != RParryBot_new_hit)
			{
				if (player.ammo < 3)
				{
					player.ammo++;
				}
				RParryBot.hp -= RParryBot.damage;
				RParryBot_last_hit = RParryBot_new_hit;
			}
		}

		if (Attacking_Ranged_Impact(RParryBot.pos, RParryBot.AttackHitboxWidth, RParryBot.AttackHitboxHeight, &RParryBot.damage))
		{
			RParryBot.hp -= RParryBot.damage;
		}


		if (RParryBot.hp <= 0) {
			RParryBot.respawn_timer.current = 0.f;
			RParryBot.state = Destroyed;
		}

		switch (RParryBot.state)
		{
		case Idle:
			DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_idle);
			if (Update_Timer(&RParryBot.idle_timer)) {
				//timer has not ended
			}
			else {
				RParryBot.windup_timer.current = 0.f;
				RParryBot.state = WindUp;
			}
			break;
		case WindUp:
			DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_windup);
			if (Update_Timer(&RParryBot.windup_timer)) {
				//timer has not ended
			}
			else {
				RParryBot.attack_timer.current = 0.f;
				RParryBot.state = Swinging;
			}
			break;
		case Swinging:
			DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_swing);
			Attack_DummyBotRange();
			if (Update_Timer(&RParryBot.attack_timer)) {			//idk why when i or this if statement it dont work so pls pardon this ugly ass code
				if (AEVec2Distance(&RParryBot.pos, &RParryBot.projectile_pos) > RParryBot.projectile_range) {
					RParryBot.idle_timer.current = 0.f;
					RParryBot.projectile_pos = RParryBot.attackpos;
					RParryBot.state = Idle;
				}
			}
			else {
				RParryBot.idle_timer.current = 0.f;
				RParryBot.projectile_pos = RParryBot.attackpos;
				RParryBot.state = Idle;
			}
			break;
		case Parried:
			DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_parried);
			if (Update_Timer(&RParryBot.stun_timer)) {
				//timer has not ended
			}
			else {
				RParryBot.idle_timer.current = 0.f;
				RParryBot.state = Idle;
			}
		}

	}
	else {
		//wait for timer to reset, play animation of respawn
		if (Update_Timer(&RParryBot.respawn_timer)) {
			//timer has not ended
		}
		else {
			RParryBot.idle_timer.current = 0.f;
			RParryBot.hp = RParryBot.max_hp;
			RParryBot.state = Idle;
		}
	}
}

void Attack_DummyBotRange() {
	//TO DO: ON PARRY - DEFLECT PROJECTILE BACK AT ENEMY
	
	switch (RParryBot.FacingDir)
	{
	case RParryBot.FacingLeft:
		RParryBot.projectile_pos.x -= RParryBot.projectile_speed * dt;
		break;
	case RParryBot.FacingRight:
		RParryBot.projectile_pos.x += RParryBot.projectile_speed * dt;
		break;
	case RParryBot.FacingUp:
		RParryBot.projectile_pos.y += RParryBot.projectile_speed * dt;
		break;
	case RParryBot.FacingDown:
		RParryBot.projectile_pos.y -= RParryBot.projectile_speed * dt;
		break;
	default:
		RParryBot.projectile_pos.y -= RParryBot.projectile_speed * dt;
		break;
	}
	//std::cout << "x: " << RParryBot.projectile_pos.x << '\n';
	//std::cout << "y: " << RParryBot.projectile_pos.y << '\n';

	DrawCircle(World_To_Screen_x(player.pos.x), World_To_Screen_y(player.pos.y), player.radius, color_idle);
	DrawCircle(World_To_Screen_x(RParryBot.projectile_pos.x), World_To_Screen_y(RParryBot.projectile_pos.y), RParryBot.projectile_radius, color_stunned);

	if (AETestCircleToCircle(&player.pos, player.radius, &RParryBot.projectile_pos, RParryBot.projectile_radius)) {
		std::cout << "FVSDIKBVBVBVHSVBSDHJKVBSSVB\n\n\n";
		switch (player.state) {
		case PARRY:
			RParryBot.stun_timer.current = 0.f;
			RParryBot.state = Parried;				//change to just deflect proj
			RParryBot.projectile_parry = true;
			break;
		case BLOCK:
			Blocked_Hit(RParryBot.damage);
			break;
		case PARRIED:
		case BLOCKED:
		case HIT:
		case DODGE:
			// do nothing, immune
			break;
		default:
			Eat_Hit(RParryBot.damage);
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

////outdated code, can probs remove!
//void Test_Funct(Timer* ParryTimer, int* counter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad )
//{
//	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 75.f };
//
//	switch (*counter)
//	{
//	case Idle:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_idle);
//		break;
//	case WindUp:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_windup);
//		if (ParryTimer->current >= PARRY_STUN[WindUp] - 0.5f) {
//			//temp parry eye spark/ flash effect. Replace with proper sprite later (and add audio!)
//			DrawRect(World_To_Screen_x(EnemyPos.x) - 10.f, World_To_Screen_y(EnemyPos.y), 10.f, 10.f, color_idle);
//		}
//		break;
//	case Swinging:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_swing);
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 75.f, HitBoxRad*2, color_parried);
//		//To be replaced with player's attack
//		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
//			std::cout << "!!!!!PARRYABLE!!!!!\n";
//			*counter = Parried;
//		}
//		break;
//	case Parried:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, color_parried);
//		std::cout << "ahhhhhh cb i kena parried\n";
//		break;
//	}
//}

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