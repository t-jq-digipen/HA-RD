#include "Prototype_Boss.h"
#include "player.h"
#include "UI.h"
#include <vector>

Boss proto_monke;

Color proto_idle, proto_windup, proto_swing, proto_stunned, proto_parried, proto_projectile_indicator, proto_projectile_color, proto_death_explosion_color;

SpriteSheet ssproto_monke;

#define MAX_PROJECTILES 3
#define PROJECTILE_DAMAGE 10

const f32 PROTO_MONKE_SPAWN_X = 0.f;
const f32 PROTO_MONKE_SPAWN_Y = 200.f;

const f32 PROTO_MONKE_WIDTH = 500.f;
const f32 PROTO_MONKE_HEIGHT = 500.f;

const f32 PROTO_MONKE_HITBOX_WIDTH = 400.f;
const f32 PROTO_MONKE_HITBOX_HEIGHT = 300.f;

const f32 PROTO_MONKE_MAXHP = 200.f;

// DASH
const f32 PROTO_MONKE_DASH_SPEED = 1000.f;
const f32 PROTO_MONKE_DASH_DISTANCE = 300.f;
const f32 DASH_WIDTH = 500.f;
const f32 DASH_HEIGHT = 500.f;

// Performs an overhead smash, followed by a wide sweep that can be parried
const s32 ATTACK_TYPES = 5;											//  1			 2
const f32 PROTO_MONKE_WINDUP[ATTACK_TYPES] = { 0.8f, 1.f, 0.8f, 0.8f, 1.f};		//  0.8s		 1s
const f32 PROTO_MONKE_ATTACK[ATTACK_TYPES] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };		// +0.2s		+0.2s
const f32 PROTO_MONKE_RECOVER[ATTACK_TYPES] = { 0.4f, 1.f, 0.4f, 0.4f, 1.f };		// +0.4s		+1s

const f32 PROTO_MONKE_STUN[ATTACK_TYPES] = { 0.f, 3.f, 0.f, 0.f, 0.f };			//  0s			 3s

const f32 PROTO_PROJECTILE_RADIUS = 30.f;
const f32 PROTO_PROJECTILE_DAMAGE = 20.f;
const f32 PROTO_PROJECTILE_SPEED = 500.f;

Boss_Melee proto_melee[ATTACK_TYPES];

Boss_Range proto_proj;
std::vector<Boss_Range> proto_projectiles;

// Boss Explosion Effects and Particles
const f32 PROTO_MONKE_DEATH_EXPLOSION_RADIUS = 50.f;
Boss_Explosion explosion_particle;
std::vector<Boss_Explosion> proto_death_explosion;

void Load_Prototype_Boss()
{
	ssproto_monke = Init_SpriteSheet("Assets/Proto_Monke/ProtoMonke.png",5,8,1.f);
}

void Init_Prototype_Indicator()
{
	proto_idle = SetColor(0xFFFF6961);
	proto_windup = SetColor(0xFFFFCF61);
	proto_swing = SetColor(0xFFAFFF61);
	proto_stunned = SetColor(0xFF000000);
	proto_parried = SetColor(0xFFFF00FF);
	proto_projectile_indicator = SetColor(0xFFFF0000);
	proto_projectile_color = SetColor(0xFFFF13F0);
	proto_death_explosion_color = SetColor(0xFFED6240);
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

	proto_monke.idle_timer = Create_Timer(1.f);
	proto_monke.attack_timer = Create_Timer(1.f);
	proto_monke.stun_timer = Create_Timer(3.f);
	proto_monke.dash_timer = Create_Timer(2.f);

	proto_monke.attack_type = BOSS_SMASH;

	proto_monke.range_warning_timer = Create_Timer(1.f);
	proto_monke.range_timer = Create_Timer(3.f);
	proto_monke.isShootingWarningActive = 0;
	proto_monke.isShooting = 0;

	proto_monke.death_explosion_duration = Create_Timer(4.f);

	for (s32 i = 0; i < ATTACK_TYPES; i++)
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

s32 last_hit = 0;
s32 new_hit = 0;
f32 damage = 0.f;
 
void Update_Prototype_Boss()
{
	if (proto_monke.state != BOSS_DEAD)
	{
		switch (proto_monke.state)
		{

		case BOSS_IDLE:
			if (!Update_Timer(&proto_monke.idle_timer))
			{
				// Calculate distance between player to check if you wanna do ranged attack
				f32 distance_to_player = AEVec2Distance(&proto_monke.pos, &player.pos);

				if (distance_to_player > PROTO_MONKE_DASH_DISTANCE) // Player is out of melee range
				{

					// Calculate direction to player and normalize it
					proto_monke.target_dir = { player.pos.x - proto_monke.pos.x, player.pos.y - proto_monke.pos.y };
					AEVec2Normalize(&proto_monke.target_dir, &proto_monke.target_dir);

					// Set target position for the ranged attack to end at
					proto_monke.target_pos = { player.pos.x , player.pos.y };

					f32 randomizer_ranged = AERandFloat();
					if (randomizer_ranged > 0.5f)
					{
						// Switch to ranged attack state
						proto_monke.state = BOSS_RANGED_CHARGE;
					}
					else
					{
						proto_monke.state = BOSS_RANGED_ROCKETFIST;
					}

				}
				else // Player is in melee range
				{
					f32 randomizer_melee = AERandFloat();
					if (randomizer_melee > 0.5f)
					{
						// Do smash sweep combo
						proto_monke.attack_timer.current = 0.f;
						proto_monke.attack_type = BOSS_SMASH;
						proto_monke.state = BOSS_SMASH_SWEEP;
					}
					else
					{
						// Do three hit combo
						proto_monke.attack_timer.current = 0.f;
						proto_monke.attack_type = BOSS_LEFT_HIT;
						proto_monke.state = BOSS_THREE_HIT;
					}
				}

			}
			break;
		case BOSS_SMASH_SWEEP:
			Smash_Sweep_Logic();
			break;

		case BOSS_THREE_HIT:
			Three_Hit_Logic();
			break;

		case BOSS_RANGED_CHARGE:
			Boss_Dash_Logic();
			break;
	
		case BOSS_RANGED_ROCKETFIST:
			Boss_Rocket_Logic();
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

		// Check for player melee attack collision**
		if (Attacked_Melee_rect(proto_monke.pos, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT, &new_hit, &damage))
		{
			if (last_hit != new_hit)
			{
				if (player.ammo < 3)
				{
					player.ammo++;
				}
				proto_monke.hp -= damage;
				last_hit = new_hit;
			}
		}

		// Check for player projectile collisions**
		if (Attacking_Ranged_Impact(proto_monke.pos, PROTO_MONKE_HITBOX_WIDTH, PROTO_MONKE_HITBOX_HEIGHT, &damage))
		{
			proto_monke.hp -= damage;
			std::cout << "Boss hit! HP: " << proto_monke.hp << '\n';
		}

		// Update boss projectiles' behavior


		for (auto& proj : proto_projectiles)
		{
			if (proj.active)
			{
				// Update boss projectiles' movement
				proj.pos.x += proj.velocity.x * dt;
				proj.pos.y += proj.velocity.y * dt;
				proj.timer.current += 1.f * dt;

				// Update boss projectiles' transformation
				AEMtx33 trans, scale;
				AEMtx33Trans(&trans, proj.pos.x, proj.pos.y);
				AEMtx33Scale(&scale, PROTO_PROJECTILE_RADIUS, PROTO_PROJECTILE_RADIUS);
				AEMtx33Concat(&proj.transform, &trans, &scale);

				// Deactivate projectiles after they hit player or after 4 seconds (aka range)
				if (AETestCircleToCircle(&proj.pos, PROTO_PROJECTILE_RADIUS, &player.pos, player.radius) || proj.timer.current >= proj.timer.duration)
				{
					proj.active = false;
					if (AETestCircleToCircle(&proj.pos, PROTO_PROJECTILE_RADIUS, &player.pos, player.radius))
					{
						switch (player.state)
						{
						case PARRY:

							Parried_Hit();
							break;
						case BLOCK:
							Blocked_Hit(PROTO_PROJECTILE_DAMAGE);
							break;
						case PARRIED:
						case BLOCKED:
						case DODGE:
						case HIT:
							break;
						default:
							Eat_Hit(PROTO_PROJECTILE_DAMAGE);
						}
					}
				}
			}
		}

		// Remove inactive boss projectiles
		proto_projectiles.erase(std::remove_if(proto_projectiles.begin(), proto_projectiles.end(), [](const Boss_Range& p) { return !p.active; }), proto_projectiles.end());



		// Clamp HP to prevent over-reduction
		proto_monke.hp = AEClamp(proto_monke.hp, 0.f, PROTO_MONKE_MAXHP);
		if (proto_monke.hp > 0.f && proto_monke.hp <= 0.6 * PROTO_MONKE_MAXHP && proto_monke.state != BOSS_PARRIED)
		{
			std::cout << proto_monke.range_timer.current << '\n';
			if (!Update_Timer(&proto_monke.range_timer))
			{
				proto_monke.isShooting = 1;
				proto_monke.range_timer.current = proto_monke.range_warning_timer.current = 0.f;
				proto_monke.isShootingWarningActive = 1;
			}

			if (proto_monke.isShooting == 1) 
			{
				proto_monke.isShootingWarningActive = 1;
				std::cout << "Boss is shooting!!!" << '\n';
				if (!Update_Timer(&proto_monke.range_warning_timer))
				{
					proto_monke.isShootingWarningActive = 0;
					AEVec2 direction = { player.pos.x - proto_monke.pos.x, player.pos.y - proto_monke.pos.y };
					AEVec2Normalize(&direction, &direction);

					Proto_Monke_Range_Attack(proto_monke.pos, direction);
					proto_monke.isShooting = 0;
				}
			}
		}
		if (proto_monke.hp <= 0.f)
		{
			proto_monke.state = BOSS_DEAD;
			proto_monke.death_explosion_duration.current = proto_monke.explosion_timer.current = 0.f;
		}

		// Update Boss UI
		Update_Boss_UI(proto_monke.hp);
	}

	else if (proto_monke.state == BOSS_DEAD)
	{
		levelState = CLEARED;
		for (auto& proj : proto_projectiles)
		{
			proj.active = false;
		}
		if (Update_Timer(&proto_monke.death_explosion_duration))
		{
			std::cout << proto_monke.death_explosion_duration.current << '\n';
			if (!Update_Timer(&proto_monke.explosion_timer))
			{
				Draw_Proto_Monke_Death_Explosion(proto_monke.pos);
			}
		}
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
	case BOSS_SMASH_SWEEP:
		color = &proto_swing;
		break;
	case BOSS_THREE_HIT:
		//TBC
		break;
	case BOSS_PARRIED:
		color = &proto_parried;
		break;
	case BOSS_RANGED_CHARGE:
		//TBC
		break;

	case BOSS_RANGED_ROCKETFIST:
		//TBC
		break;

	case BOSS_IDLE:
	default:
		color = &proto_idle;
		break;
	}

	switch (proto_monke.state)
	{
	case BOSS_SMASH_SWEEP:
	{
		u32 row = 0;
		u32 start_col = 0;
		u32 end_col = 0;
		f32 duration = 0.f;
		switch (proto_monke.attack_type)
		{
		case BOSS_SMASH:
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
		case BOSS_SWEEP:
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
	case BOSS_THREE_HIT:
	{
		//Up to you my brother JQ
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

	// Ranged attack warning indicator
	if (proto_monke.isShootingWarningActive && proto_monke.state != BOSS_DEAD) 
	{
		DrawCircle(World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), PROTO_PROJECTILE_RADIUS, proto_projectile_indicator);
		DrawCircle(World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), (proto_monke.range_warning_timer.current / proto_monke.range_warning_timer.duration) * PROTO_PROJECTILE_RADIUS, proto_projectile_color);
	}

	// Ranged attack projectiles
	for (const auto& proj : proto_projectiles)
	{
		if (proj.active)
		{
			DrawCircle(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 25.f, proto_projectile_color); // Example projectile size and color
		}
	}

	// Death explosion
	for (auto& exp : proto_death_explosion)
	{
		if (exp.active)
		{
			f32 spawn_x = World_To_Screen_x(exp.pos.x) + (AERandFloat() > 0.5f ? (AERandFloat() * PROTO_MONKE_HITBOX_WIDTH * 0.8f) : -(AERandFloat() * PROTO_MONKE_HITBOX_WIDTH * 0.8f));
			f32 spawn_y = World_To_Screen_y(exp.pos.y) + (AERandFloat() > 0.5f ? (AERandFloat() * PROTO_MONKE_HITBOX_HEIGHT * 0.8f) : -(AERandFloat() * PROTO_MONKE_HITBOX_HEIGHT * 0.8f));
			DrawCircle(spawn_x, spawn_y, (AERandFloat() + 0.5f) * PROTO_MONKE_DEATH_EXPLOSION_RADIUS, proto_death_explosion_color);
			if (exp.timer.current >= exp.timer.duration)
			{
				exp.active = false;
			}
		}
		// Remove inactive explosion particles
		proto_death_explosion.erase(std::remove_if(proto_death_explosion.begin(), proto_death_explosion.end(), [](const Boss_Explosion& p) { return !p.active; }), proto_death_explosion.end());
	}

	Draw_SpriteSheet_test(ssproto_monke, World_To_Screen_x(proto_monke.pos.x), World_To_Screen_y(proto_monke.pos.y), proto_monke.width, proto_monke.height, 0.f);
}

void Unload_Prototype_Boss()
{
	Unload_SpriteSheet(&ssproto_monke);
}



const f32 SMASH_RADIUS = 200.f;
const f32 SMASH_DAMAGE = 30.f;
const f32 BIG_SMASH_RADIUS = 500.f;

const f32 SWEEP_WIDTH = 500.f;
const f32 SWEEP_HEIGHT = 500.f;

f32 x_offset = 0.f;

bool will_hit = true;

void Smash_Sweep_Logic()
{
	if (Update_Timer(&proto_monke.attack_timer))
	{

		if (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration < proto_melee[proto_monke.attack_type].attack_windup)			// Attack is winding up (to update anim)
		{
			will_hit = true;
			proto_monke.attackstate = BOSS_ATTACK_WINDUP;
			switch (proto_monke.attack_type)
			{
			case BOSS_SMASH:
				x_offset = -200.f;
				proto_melee[BOSS_SMASH].pos = { proto_monke.pos.x + x_offset, proto_monke.pos.y - 150.f };
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_SMASH].pos.x), World_To_Screen_y(proto_melee[BOSS_SMASH].pos.y), SMASH_RADIUS, proto_windup);
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_SMASH].pos.x), World_To_Screen_y(proto_melee[BOSS_SMASH].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SMASH_RADIUS, proto_swing);
				break;

			case BOSS_SWEEP:
				proto_melee[BOSS_SWEEP].pos = { proto_monke.pos.x, proto_monke.pos.y - 150.f };
				DrawRect(World_To_Screen_x(proto_melee[BOSS_SWEEP].pos.x), World_To_Screen_y(proto_melee[BOSS_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, proto_windup);
				DrawRect(World_To_Screen_x(proto_melee[BOSS_SWEEP].pos.x) + ((proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SWEEP_WIDTH/2.f - SWEEP_WIDTH/2.f) , World_To_Screen_y(proto_melee[BOSS_SWEEP].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SWEEP_WIDTH, SWEEP_HEIGHT, proto_swing);

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
			case BOSS_SMASH:
				if (AETestCircleToCircle(&proto_melee[BOSS_SMASH].pos, SMASH_RADIUS, &player.pos, player.radius))
				{
					// player dmg calc
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[BOSS_SMASH].can_parry, 30.f);
					}
				}
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_SMASH].pos.x), World_To_Screen_y(proto_melee[BOSS_SMASH].pos.y), SMASH_RADIUS, proto_idle);
				break;
			case BOSS_SWEEP:
				if (AETestCircleToRect(&player.pos, player.radius, &proto_melee[BOSS_SWEEP].pos, SWEEP_WIDTH, SWEEP_HEIGHT))
				{
					// parry checks
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[BOSS_SWEEP].can_parry, 30.f);
					}
				}
				DrawRect(World_To_Screen_x(proto_melee[BOSS_SWEEP].pos.x), World_To_Screen_y(proto_melee[BOSS_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, proto_idle);
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
		if (proto_monke.attack_type == BOSS_SMASH)
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

void Three_Hit_Logic()
{
	if (Update_Timer(&proto_monke.attack_timer))
	{

		if (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration < proto_melee[proto_monke.attack_type].attack_windup)			// Attack is winding up (to update anim)
		{
			will_hit = true;
			proto_monke.attackstate = BOSS_ATTACK_WINDUP;
			switch (proto_monke.attack_type)
			{
			case BOSS_LEFT_HIT:
				x_offset = -200.f;
				proto_melee[BOSS_LEFT_HIT].pos = { proto_monke.pos.x + x_offset, proto_monke.pos.y - 150.f };
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_LEFT_HIT].pos.y), SMASH_RADIUS, proto_windup);
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_LEFT_HIT].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SMASH_RADIUS, proto_swing);
				break;

			case BOSS_RIGHT_HIT:
				x_offset = 200.f;
				proto_melee[BOSS_RIGHT_HIT].pos = { proto_monke.pos.x + x_offset, proto_monke.pos.y - 150.f };
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_RIGHT_HIT].pos.y), SMASH_RADIUS, proto_windup);
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_RIGHT_HIT].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * SMASH_RADIUS, proto_swing);
				break;

			case BOSS_CIRCLE_HIT:
				proto_melee[BOSS_CIRCLE_HIT].pos = { proto_monke.pos.x, proto_monke.pos.y - 150.f };
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_CIRCLE_HIT].pos.y), BIG_SMASH_RADIUS, proto_windup);
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_CIRCLE_HIT].pos.y), (proto_monke.attack_timer.current * proto_melee[proto_monke.attack_type].timer.duration / proto_melee[proto_monke.attack_type].attack_windup) * BIG_SMASH_RADIUS, proto_swing);
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
			case BOSS_LEFT_HIT:
				if (AETestCircleToCircle(&proto_melee[BOSS_LEFT_HIT].pos, SMASH_RADIUS, &player.pos, player.radius))
				{
					// player dmg calc
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[BOSS_LEFT_HIT].can_parry, 30.f);
					}
				}
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_LEFT_HIT].pos.y), SMASH_RADIUS, proto_idle);
				break;
			case BOSS_RIGHT_HIT:
				if (AETestCircleToCircle(&proto_melee[BOSS_RIGHT_HIT].pos, SMASH_RADIUS, &player.pos, player.radius))
				{
					// player dmg calc
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[BOSS_RIGHT_HIT].can_parry, 30.f);
					}
				}
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_RIGHT_HIT].pos.y), SMASH_RADIUS, proto_idle);
				break;

			case BOSS_CIRCLE_HIT:
				if (AETestCircleToCircle(&proto_melee[BOSS_CIRCLE_HIT].pos, SMASH_RADIUS, &player.pos, player.radius))
				{
					// player dmg calc
					if (will_hit)
					{
						Proto_Monke_Attack_Test(proto_melee[BOSS_CIRCLE_HIT].can_parry, 30.f);
					}
				}
				DrawCircle(World_To_Screen_x(proto_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(proto_melee[BOSS_CIRCLE_HIT].pos.y), BIG_SMASH_RADIUS, proto_idle);
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
		if (proto_monke.attack_type == BOSS_LEFT_HIT || proto_monke.attack_type == BOSS_RIGHT_HIT)
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

void Boss_Dash_Logic()
{
	// Move boss towards the target position
	proto_monke.pos.x += proto_monke.target_dir.x * PROTO_MONKE_DASH_SPEED * dt;
	proto_monke.pos.y += proto_monke.target_dir.y * PROTO_MONKE_DASH_SPEED * dt;

	// Calculate distance between boss and target position
	f32 distance_to_target = AEVec2Distance(&proto_monke.pos, &proto_monke.target_pos);

	// Check if boss has reached with a slight buffer
	if (distance_to_target <= 10.f)
	{
		proto_monke.pos = proto_monke.target_pos;  // Ensure the boss stops exactly at the target position
		//proto_monke.state = BOSS_IDLE; // Switch to attack state after reaching the target
		proto_monke.attack_timer.current = 0.f;
		proto_monke.attack_type = BOSS_SMASH;
		proto_monke.state = BOSS_SMASH_SWEEP;
	}
}

void Boss_Rocket_Logic()
{
	// Move boss towards the target position
	proto_monke.pos.x += proto_monke.target_dir.x * PROTO_MONKE_DASH_SPEED * dt;
	proto_monke.pos.y += proto_monke.target_dir.y * PROTO_MONKE_DASH_SPEED * dt;

	// Calculate distance between boss and target position
	f32 distance_to_target = AEVec2Distance(&proto_monke.pos, &proto_monke.target_pos);

	// Check if boss has reached with a slight buffer
	if (distance_to_target <= 10.f)
	{
		proto_monke.pos = proto_monke.target_pos;  // Ensure the boss stops exactly at the target position
		proto_monke.state = BOSS_IDLE; // Switch to attack state after reaching the target
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
		}
		else
		{
		}
		Parried_Hit();
		break;
	case BLOCK:
		Blocked_Hit(boss_damage);
		break;
	case PARRIED:
	case BLOCKED:
	case HIT:
	case DODGE:
		// do nothing, immune
		break;
	default:
		Eat_Hit(boss_damage);
	}
}

void Proto_Monke_Range_Attack(const AEVec2& start, const AEVec2& direction)
{
	proto_proj.pos = start;
	proto_proj.velocity = { direction.x * PROTO_PROJECTILE_SPEED, direction.y * PROTO_PROJECTILE_SPEED };
	proto_proj.active = true;
	proto_proj.timer = Create_Timer(4.f);
	AEMtx33Identity(&proto_proj.transform);
	proto_projectiles.push_back(proto_proj);
}

void Draw_Proto_Monke_Death_Explosion(AEVec2& start)
{
	explosion_particle.pos = start;
	explosion_particle.active = true;
	explosion_particle.timer = Create_Timer(0.5f);
	AEMtx33Identity(&explosion_particle.transform);
	proto_death_explosion.push_back(explosion_particle);
}