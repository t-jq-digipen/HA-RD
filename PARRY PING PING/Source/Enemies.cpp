#include "enemies.h"
#include "player.h"

Color color_idle, color_windup, color_swing, color_stunned, color_parried;

DummyBotMelee MParryBot;		//Melee Practice Range Parry Bot
DummyBotRange RParryBot;		//Ranged Attack

//MELEE PARRY BOT FUNCTIONS:
// Init            - Inits values such as timers etc
// Active (Update) - Call in Update(), handles checking of states
// Attack          - Call in Attacking state, handles logic for parrying, damage detection etc
//Draw             - Render sprites
void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp) {
	MParryBot.pos = pos;
	MParryBot.FacingDir = MParryBot.FacingDown;								//Init here which dir bot should face!!!
	switch (MParryBot.FacingDir) 
	{
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
	MParryBot.max_hp = max_hp;
	MParryBot.damage = 10.f;
	MParryBot.AttackHitboxWidth = 150.f;
	MParryBot.AttackHitboxHeight = 75.f;
	MParryBot.state = Idle;

	MParryBot.spark_buffer = 0.5;
	MParryBot.spark_draw = false;

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
	if (MParryBot.state != Destroyed) {

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


		if (MParryBot.hp <= 0) 
		{
			MParryBot.respawn_timer.current = 0.f;
			MParryBot.state = Destroyed;
		}

		switch (MParryBot.state)
		{
		case Idle:
			if (Update_Timer(&MParryBot.idle_timer)) {
				//timer has not ended
			}
			else 
			{
				MParryBot.windup_timer.current = 0.f;
				MParryBot.state = WindUp;
			}
			break;
		case WindUp:
			if (Update_Timer(&MParryBot.windup_timer)) {
				//timer has not ended
				if (MParryBot.windup_timer.current >= MParryBot.windup_timer.end - MParryBot.spark_buffer) {
					MParryBot.spark_draw = true;
				}
			}
			else 
			{
				MParryBot.spark_draw = false;
				MParryBot.attack_timer.current = 0.f;
				MParryBot.state = Swinging;
			}
			break;
		case Swinging:
			Attack_DummyBotMelee();
			if (Update_Timer(&MParryBot.attack_timer)) {
				//timer has not ended
			}
			else 
			{
				MParryBot.idle_timer.current = 0.f;
				MParryBot.state = Idle;
			}
			break;
		case Parried:
			if (Update_Timer(&MParryBot.stun_timer)) {
				//timer has not ended
			}
			else 
			{
				MParryBot.idle_timer.current = 0.f;
				MParryBot.state = Idle;
			}
		}

	}
	else {
		//wait for timer to reset, play animation of respawn
		MParryBot.spark_draw = false;
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
	if (AETestCircleToRect(&player.pos, player.radius, &MParryBot.attackpos, MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight)) 
	{
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

void Draw_DummyBotMelee() {
	//jq the almighty animation lord please do ur magic these drawrects look ASS
	switch (MParryBot.state)
	{
	case Idle:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_idle);
		break;
	case WindUp:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_windup);
		break;
	case Swinging:
		DrawRect(World_To_Screen_x(MParryBot.attackpos.x), World_To_Screen_y(MParryBot.attackpos.y), MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight, color_windup);			//draw 'hitbox'
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_swing);
		break;
	case Parried:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_parried);
		break;
	}
	if (MParryBot.spark_draw) {
		std::cout << "draw cool spark like shiiiing sheeeeoonng not the supermarket but like the sound ykwim shiiiing and also play the sound like the one i just made shiiiing ok ty\n";
		//DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad * 10, MParryBot.rad, color_parried);
		//add spark anim here :), its pos is alr synced with enem anim so just need to draw here :)
	}
}


//RANGED PARRY BOT FUNCTIONS:
// Init            - Inits values such as timers etc
// Active (Update) - Call in Update(), handles checking of states
// Attack          - Call in Attacking state, handles logic for parrying, damage detection etc, i know the attack state is 'swinging' but yeah
//Draw             - Render sprites
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
	RParryBot.projectile_draw = false;
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
	if (RParryBot.state != Destroyed) {

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


		if (RParryBot.hp <= 0) 
		{
			RParryBot.respawn_timer.current = 0.f;
			RParryBot.state = Destroyed;
		}

		switch (RParryBot.state)
		{
		case Idle:
			if (Update_Timer(&RParryBot.idle_timer)) {
				//timer has not ended
			}
			else 
			{
				RParryBot.windup_timer.current = 0.f;
				RParryBot.state = WindUp;
			}
			break;
		case WindUp:
			if (Update_Timer(&RParryBot.windup_timer)) {
				//timer has not ended
			}
			else 
			{
				RParryBot.attack_timer.current = 0.f;
				RParryBot.state = Swinging;
			}
			break;
		case Swinging:
			RParryBot.projectile_draw = true;
			Attack_DummyBotRange();
			if (Update_Timer(&RParryBot.attack_timer)) {			//idk why when i or this if statement it dont work so pls pardon this ugly ass code
				if (AEVec2Distance(&RParryBot.pos, &RParryBot.projectile_pos) > RParryBot.projectile_range) 
				{
					RParryBot.projectile_draw = false;
					RParryBot.idle_timer.current = 0.f;
					RParryBot.projectile_pos = RParryBot.attackpos;
					RParryBot.state = Idle;
				}
			}
			else 
			{
				RParryBot.projectile_draw = false;
				RParryBot.idle_timer.current = 0.f;
				RParryBot.projectile_pos = RParryBot.attackpos;
				RParryBot.state = Idle;
			}
			break;
		case Parried:
			//REPLACE THIS BECAUSE DONT MAKE SENSE FOR PARRIED PROJECTILE TO STUN ENEMY
			if (Update_Timer(&RParryBot.stun_timer)) {
				//timer has not ended
			}
			else 
			{
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
		else 
		{
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

	if (AETestCircleToCircle(&player.pos, player.radius, &RParryBot.projectile_pos, RParryBot.projectile_radius)) 
	{
		switch (player.state) {
		case PARRY:
			RParryBot.stun_timer.current = 0.f;
			RParryBot.state = Parried;				//change to just deflect proj
			RParryBot.projectile_parry = true;
			RParryBot.projectile_draw = false;		//set to true once deflect logic implemented
			break;
		case BLOCK:
			Blocked_Hit(RParryBot.damage);
			RParryBot.projectile_draw = false;
			break;
		case PARRIED:
		case BLOCKED:
		case HIT:
		case DODGE:
			// do nothing, immune
			RParryBot.projectile_draw = false;
			break;
		default:
			Eat_Hit(RParryBot.damage);
			RParryBot.projectile_draw = false;
		}
	}
}

void Draw_DummyBotRange() {
	//jq the almighty animation lord please do ur magic these drawrects look ASS
	switch (RParryBot.state)
	{
	case Idle:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_idle);
		break;
	case WindUp:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_windup);
		break;
	case Swinging:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_swing);
		break;
	case Parried:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_parried);
	}
	if (RParryBot.projectile_draw)
	{
		//Temp Projectile Drawing
		DrawCircle(World_To_Screen_x(RParryBot.projectile_pos.x), World_To_Screen_y(RParryBot.projectile_pos.y), RParryBot.projectile_radius, color_stunned);
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