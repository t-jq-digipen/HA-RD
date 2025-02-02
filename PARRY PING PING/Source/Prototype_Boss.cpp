#include "Prototype_Boss.h"
#include "player.h"
#include "UI.h"

Boss proto_monke;

Color proto_idle, proto_windup, proto_swing, proto_stunned, proto_parried;

SpriteSheet ssproto_monke;

#define MAX_PROJECTILES 3
#define PROJECTILE_DAMAGE 10

const float PROTO_MONKE_SPAWN_X = 0.f;
const float PROTO_MONKE_SPAWN_Y = 200.f;

const float PROTO_MONKE_WIDTH = 500.f;
const float PROTO_MONKE_HEIGHT = 500.f;

const float PROTO_MONKE_HITBOX_WIDTH = 400.f;
const float PROTO_MONKE_HITBOX_HEIGHT = 300.f;

const float PROTO_MONKE_MAXHP = 100.f;


// Performs an overhead smash, followed by a wide sweep that can be parried
const int ATTACK_TYPES = 2;											//  1			 2
const float PROTO_MONKE_WINDUP[ATTACK_TYPES] = { 0.8f, 1.f };		//  0.8s		 1s
const float PROTO_MONKE_ATTACK[ATTACK_TYPES] = { 0.2f, 0.2f };		// +0.2s		+0.2s
const float PROTO_MONKE_RECOVER[ATTACK_TYPES] = { 0.4f, 1.f };		// +0.4s		+1s

const float PROTO_MONKE_STUN[ATTACK_TYPES] = { 0.f, 3.f };			//  0s			 3s

Boss_Melee proto_melee[ATTACK_TYPES];


void Init_Prototype_Indicator()
{
	proto_idle = SetColor(0xFFFF6961);
	proto_windup = SetColor(0xFFFFCF61);
	proto_swing = SetColor(0xFFAFFF61);
	proto_stunned = SetColor(0xFF000000);
	proto_parried = SetColor(0xFFFF00FF);
}

void Load_Prototype_Boss()
{
	ssproto_monke = Init_SpriteSheet("Assets/Proto_Monke/ProtoMonke.png",5,8,1.f);
}

void Init_Prototype_Boss()
{
	proto_monke.pos = { PROTO_MONKE_SPAWN_X, PROTO_MONKE_SPAWN_Y };

	proto_monke.width = PROTO_MONKE_WIDTH;
	proto_monke.height = PROTO_MONKE_HEIGHT;

	proto_monke.hitbox = Init_AABB(proto_monke.pos.x, proto_monke.pos.y, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT);

	proto_monke.hp = proto_monke.max_hp = PROTO_MONKE_MAXHP;

	proto_monke.state = BOSS_IDLE;
	proto_monke.attackstate = BOSS_ATTACK_WINDUP;

	proto_monke.idle_timer = Create_Timer(2.f);
	proto_monke.attack_timer = Create_Timer(1.f);
	proto_monke.stun_timer = Create_Timer(3.f);

	proto_monke.attack_type = MONKE_SMASH;

	for (int i = 0; i < ATTACK_TYPES; i++)
	{
		proto_melee[i].pos = { 0.f, 0.f };
		proto_melee[i].attack_windup = PROTO_MONKE_WINDUP[i];
		proto_melee[i].attack_active = proto_melee[i].attack_windup + PROTO_MONKE_ATTACK[i];
		proto_melee[i].attack_recover = proto_melee[i].attack_active + PROTO_MONKE_RECOVER[i];

		proto_melee[i].timer = Create_Timer(proto_melee[i].attack_recover);

		proto_melee[i].stun_duration = PROTO_MONKE_STUN[i];
		if (proto_melee[i].stun_duration)
		{
			proto_melee[i].can_parry = true;
		}
		else
		{
			proto_melee[i].can_parry = false;
		}
	}

	Init_Boss_UI(proto_monke.hp);
}

int last_hit = 0;
int new_hit = 0;
float damage = 0.f;

void Update_Prototype_Boss()
{
	if (proto_monke.state != BOSS_DEAD)
	{
		switch (proto_monke.state)
		{
		case BOSS_IDLE:
			if (Update_Timer(&proto_monke.idle_timer))
			{
				// Implement walk to above player
			}
			else
			{
				proto_monke.attack_timer.current = 0.f;
				proto_monke.state = BOSS_ATTACK;
			}
			break;
		case BOSS_ATTACK:
			Prototype_Boss_Attack();
			break;
		case BOSS_PARRIED:
			if (!Update_Timer(&proto_monke.stun_timer))
			{
				proto_monke.state = BOSS_IDLE;
			}
			break;
		default:
			proto_monke.state = BOSS_IDLE;
			break;
		}

		// Reset last hit if no new melee hit detected
		if (!new_hit)
		{
			last_hit = 0;
		}

		// **Check for melee attack collision**
		if (Attacked_Melee_rect(proto_monke.pos, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT, &new_hit, &damage))
		{
			if (last_hit != new_hit)
			{
				player.ammo++;
				proto_monke.hp -= damage;
				last_hit = new_hit;
			}
		}

		// **Check for projectile collisions**
		if (Attacking_Ranged_Impact(proto_monke.pos, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT, &damage))
		{
			proto_monke.hp -= damage;
			std::cout << "Boss hit! HP: " << proto_monke.hp << '\n';
		}



		// Clamp HP to prevent over-reduction
		proto_monke.hp = AEClamp(proto_monke.hp, 0.f, PROTO_MONKE_MAXHP);

		// **Check if boss is dead**
		if (proto_monke.hp <= 0.f)
		{
			proto_monke.state = BOSS_DEAD;
		}

		// Update Boss UI
		Update_Boss_UI(proto_monke.hp);
	}

	// **Update Boss Hitbox**
	Update_AABB(&proto_monke.hitbox, proto_monke.pos.x, proto_monke.pos.y, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT);

	std::cout << "BOSS STATES: \n0 - IDLE\n1 - ATTACK\n2 - PARRIED\nCURRENT STATE: " << proto_monke.state << '\n';
}


void Draw_Prototype_Boss()
{
	Color* color = {};
	switch (proto_monke.state)
	{
	case BOSS_ATTACK:
		color = &proto_swing;
		break;
	case BOSS_PARRIED:
		color = &proto_parried;
		break;
	case BOSS_IDLE:
	default:
		color = &proto_idle;
		break;
	}

	switch (proto_monke.state)
	{
	case BOSS_ATTACK:
	{
		u32 row = 0;
		u32 start_col = 0;
		u32 end_col = 0;
		f32 duration = 0.f;
		switch (proto_monke.attack_type)
		{
		case MONKE_SMASH:
			switch (proto_monke.attackstate)
			{
			case BOSS_ATTACK_WINDUP:
				row = 1;
				start_col = 0;
				end_col = 4;
				duration = proto_melee[proto_monke.attack_type].attack_windup;
				break;
			case BOSS_ATTACK_ACTIVE:
				row = 1;
				start_col = 4;
				end_col = 5;
				duration = proto_melee[proto_monke.attack_type].attack_active;
				break;
			case BOSS_ATTACK_RECOVER:
				row = 1;
				start_col = 5;
				end_col = 8;
				duration = proto_melee[proto_monke.attack_type].attack_recover;
				break;
			}
			break;
		case MONKE_SWEEP:
			switch (proto_monke.attackstate)
			{
			case BOSS_ATTACK_WINDUP:
				row = 2;
				start_col = 0;
				end_col = 8;
				duration = proto_melee[proto_monke.attack_type].attack_windup;
				break;
			case BOSS_ATTACK_ACTIVE:
				row = 3;
				start_col = 0;
				end_col = 3;
				duration = proto_melee[proto_monke.attack_type].attack_active;
				break;
			case BOSS_ATTACK_RECOVER:
				row = 3;
				start_col = 3;
				end_col = 8;
				duration = proto_melee[proto_monke.attack_type].attack_recover;
				break;
			}
			break;
		}
		Set_SpriteSheet_current(&ssproto_monke, row, start_col, end_col, proto_monke.attack_timer, duration);
		break;
	}
	case BOSS_PARRIED:
		Loop_Timer(&ssproto_monke.timer);
		Set_SpriteSheet_current(&ssproto_monke, 4, 0, 8, ssproto_monke.timer, ssproto_monke.timer.duration);
		break;
	case BOSS_IDLE:
	default:
		Loop_Timer(&ssproto_monke.timer);
		Set_SpriteSheet_current(&ssproto_monke, 0, 0, 8, ssproto_monke.timer, ssproto_monke.timer.duration);
		break;
	}

	Draw_AABB(World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), proto_monke.hitbox, SetColor(0x30FFFFFF));
	//DrawRect(World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), proto_monke.width, proto_monke.height, SetColor(0x30FFFFFF));
	Draw_SpriteSheet_test(ssproto_monke, World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), proto_monke.width, proto_monke.height, 0.f);
}

void Unload_Prototype_Boss()
{
	Unload_SpriteSheet(&ssproto_monke);
}



const float SMASH_RADIUS = 80.f;
const float SMASH_DAMAGE = 30.f;

const float SWEEP_WIDTH = 400.f;
const float SWEEP_HEIGHT = 150.f;

f32 x_offset = 0.f;

bool will_hit = true;

void Prototype_Boss_Attack()
{
	if (Update_Timer(&proto_monke.attack_timer))
	{
		std::cout << "timer.current : " << proto_monke.attack_timer.current << '\n';
		std::cout << "attack_type " << proto_monke.attack_type << " | windup = " << proto_melee[proto_monke.attack_type].attack_windup << ", active = " << proto_melee[proto_monke.attack_type].attack_active << ", recover = " << proto_melee[proto_monke.attack_type].attack_recover << '\n';
		if (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration < proto_melee[proto_monke.attack_type].attack_windup)			// Attack is winding up (to update anim)
		{
			will_hit = true;
			proto_monke.attackstate = BOSS_ATTACK_WINDUP;
			switch (proto_monke.attack_type)
			{
			case MONKE_SMASH:
				//x_offset = (player.pos.x > proto_monke.pos.x) ? 200.f : -200.f;
				x_offset = -200.f;
				proto_melee[MONKE_SMASH].pos = { proto_monke.pos.x + x_offset, proto_monke.pos.y - 150.f };
				DrawCircle(World_To_Screen_x(proto_melee[MONKE_SMASH].pos.x), World_To_Screen_y(proto_melee[MONKE_SMASH].pos.y), SMASH_RADIUS, proto_windup);
				DrawCircle(World_To_Screen_x(proto_melee[MONKE_SMASH].pos.x), World_To_Screen_y(proto_melee[MONKE_SMASH].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SMASH_RADIUS, proto_swing);
				//std::cout << "proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].attack_windup: " << proto_monke.attack_timer.current / proto_melee[proto_monke.attack_type].attack_windup << '\n';
				break;
			case MONKE_SWEEP:
				proto_melee[MONKE_SWEEP].pos = { proto_monke.pos.x, proto_monke.pos.y - 150.f };
				DrawRect(World_To_Screen_x(proto_melee[MONKE_SWEEP].pos.x), World_To_Screen_y(proto_melee[MONKE_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, proto_windup);
				DrawRect(World_To_Screen_x(proto_melee[MONKE_SWEEP].pos.x) + ((proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SWEEP_WIDTH/2.f - SWEEP_WIDTH/2.f) , World_To_Screen_y(proto_melee[MONKE_SWEEP].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SWEEP_WIDTH, SWEEP_HEIGHT, proto_swing);

				break;
			default:
				break;
			}
		}
		else if (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration < proto_melee[proto_monke.attack_type].attack_active)	// Attack is active (check collision)
		{
			proto_monke.attackstate = BOSS_ATTACK_ACTIVE;
			switch (proto_monke.attack_type)
			{
			case MONKE_SMASH:
				if (AETestCircleToCircle(&proto_melee[MONKE_SMASH].pos, SMASH_RADIUS, &player.pos, player.radius))
				{
					//std::cout << "bonked\n";
					// player dmg calc
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[MONKE_SMASH].can_parry, 30.f);
					}
				}
				DrawCircle(World_To_Screen_x(proto_melee[MONKE_SMASH].pos.x), World_To_Screen_y(proto_melee[MONKE_SMASH].pos.y), SMASH_RADIUS, proto_idle);
				break;
			case MONKE_SWEEP:
				if (AETestCircleToRect(&player.pos, player.radius, &proto_melee[MONKE_SWEEP].pos, SWEEP_WIDTH, SWEEP_HEIGHT))
				{
					//std::cout << "slapped\n";
					// parry checks
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[MONKE_SWEEP].can_parry, 30.f);
					}
				}
				DrawRect(World_To_Screen_x(proto_melee[MONKE_SWEEP].pos.x), World_To_Screen_y(proto_melee[MONKE_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, proto_idle);
				break;
			default:
				proto_monke.attackstate = BOSS_ATTACK_RECOVER;
				break;
			}
		}
		else if (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration < proto_melee[proto_monke.attack_type].attack_recover)	// Attack is recovering (update anim)
		{
			proto_monke.attackstate = BOSS_ATTACK_RECOVER;
		}
	}
	else
	{
		if (proto_monke.attack_type < ATTACK_TYPES-1)
		{
			proto_monke.attack_type++;
			proto_melee[proto_monke.attack_type].timer.current = 0.f;
			proto_monke.attack_timer = proto_melee[proto_monke.attack_type].timer;
			proto_monke.attackstate = BOSS_ATTACK_WINDUP;
		}
		else
		{
			std::cout << "YES";
			proto_monke.state = BOSS_IDLE;
			proto_monke.attack_type = 0;
			proto_monke.idle_timer.current = 0.f;
		}
	}
}


void Proto_Monke_Attack_Test(bool canparry, f32 boss_damage)
{
	will_hit = false;
	switch (player.state)
	{
	case PARRY:
		//stun logic
		if (canparry)
		{
			proto_monke.stun_timer.current = 0.f;
			proto_monke.stun_timer.duration = proto_melee[proto_monke.attack_type].stun_duration;
			proto_monke.state = BOSS_PARRIED;
			proto_monke.attack_type = 0;
			bullet_time.current = 0.f;
			std::cout << "genna parried\n";
			player.state = PARRIED;
			player.blocked.current = 0.f;
		}
		else
		{
			// make enemy attack inactive?
		}
		// reload player ammo
		player.ammo = 3;
		// reset block cooldown
		player.blocktimer.current = 1.f;
		break;
	case BLOCK:
		//reduced dmg
		player.state = BLOCKED;
		player.blocked.current = 0.f;
		player.hp -= 0.5f * boss_damage;
		break;
	case PARRIED:
	case BLOCKED:
	case HIT:
	case DODGE:
		// do nothing, immune
		break;
	default:
		// take dmg
		player.state = HIT;
		player.hittimer.current = 0.f;
		player.hp -= boss_damage;
	}
}

//void JQMonkeAttackDemo(Timer* ParryTimer, int* counter, f32* stunCounter, AEVec2 EnemyPos, f32 HitBoxRad, AEVec2 PlayerPos, f32 PlayerRad)
//{
//	std::cout << "\n\n\n\n\n JQ COUNTER: " << *counter << "\n\n\n\n\n";
//	AEVec2 TempHitbox{ EnemyPos.x, EnemyPos.y - 150.f };				//to be replaced by proper enemy's attack hitbox!!!!
//	if (*counter == JQParried && ParryTimer->current >= 1.f)
//	{
//		++*stunCounter;
//		if (*stunCounter == static_cast<int>(JQMonke[JQParried])) {
//			*counter = 0;
//			*stunCounter = 0;
//		}
//	}
//	else if (*counter == JQRecovery && ParryTimer->current >= JQMonke[*counter])
//	{
//		*counter = 0;
//		ParryTimer->current = 0.f;
//	}
//	else if (ParryTimer->current >= JQMonke[*counter])
//	{
//		++*counter;
//		ParryTimer->current = 0.f;
//	}
//
//	switch (*counter)
//	{
//	case JQIdle:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
//		break;
//	case JQSmashWindUp:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
//		break;
//	case JQSmash:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, swing);
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 150.f, HitBoxRad * 2, parried);
//		if (AETestCircleToCircle(&TempHitbox, HitBoxRad * 2, &PlayerPos, PlayerRad)) {
//			std::cout << "U KENA JQ SMASHED!!!!!\n";
//		}
//		break;
//	case JQSweepWindUp:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, windup);
//		break;
//	case JQSweep:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, swing);
//		DrawRect(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y) - 150.f, 400.f, 150.f, parried);
//		if (AETestCircleToRect(&PlayerPos, PlayerRad, &TempHitbox, 400.f, 150.f)) {
//			std::cout << "U KENA JQ SWEEPED!!!!!\n";
//			*counter = JQParried;
//		}
//		break;
//	case JQRecovery:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, idle);
//		break;
//	case JQParried:
//		DrawCircle(World_To_Screen_x(EnemyPos.x), World_To_Screen_y(EnemyPos.y), HitBoxRad, parried);
//		break;
//	}
//}