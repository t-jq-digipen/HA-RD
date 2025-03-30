//---------------------------------------------------------
// file:	Enemies.cpp
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Declaration of General Enemy related functions
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "enemies.h"
#include "player.h"
#include "SpriteSheet.h"
#include "utils.h"

Color color_idle, color_windup, color_swing, color_stunned, color_parried;

DummyBotMelee MParryBot;		//Melee Practice Range Parry Bot
DummyBotRange RParryBot;		//Ranged Attack

SpriteSheet meleebot_sprite;


typedef struct ParrySpark {
	SpriteSheet sprite;
	AEVec2 pos;
	Timer timer;
	bool active;
} ParrySpark;

ParrySpark spark;

void Init_Parry_Spark()
{
	spark.pos = { 0.f,0.f };
	spark.timer = Create_Timer(.5f);
	spark.active = false;
}

void Load_Parry_Spark()
{
	spark.sprite = SpriteSheet("Assets/UI/Spark.png", 9, 1);
}

void Unload_Parry_Spark()
{
	spark.sprite.Unload();
}

void Set_Spark_Loc(f32 pos_x, f32 pos_y) 
{
	spark.pos = { pos_x, pos_y };
}

void Start_Spark(f32 duration)
{
	if (!spark.active)
	{
		spark.timer.current = 0.f;
		spark.timer.duration = duration;
		spark.active = true;
	}
}

void Draw_Spark()
{
	if (Update_Timer(&spark.timer))
	{
		std::cout << "SHOULDBEDRAWINGSPARKNOW\n";
		spark.sprite.Set_Row(0);
		spark.sprite.Set_Col(0, 8, spark.timer.current);
		spark.sprite.Draw(World_To_Screen_x(spark.pos.x), World_To_Screen_y(spark.pos.y), 1500.f, 1000.f);
	}
	else
	{
		spark.active = false;
	}
}


//MELEE PARRY BOT FUNCTIONS:
// Init            - Inits values such as timers etc
// Active (Update) - Call in Update(), handles checking of states
// Attack          - Call in Attacking state, handles logic for parrying, damage detection etc
//Draw             - Render sprites
void Init_DummyBotMelee(AEVec2 pos, f32 rad, f32 max_hp) 
{
	MParryBot.pos = pos;
	MParryBot.FacingDir = MParryBot.FacingDown;								//Init here which dir bot should face!!!
	switch (MParryBot.FacingDir) 
	{
		case MParryBot.FacingLeft:
			MParryBot.attackpos = AEVec2{ pos.x - 100, pos.y };				// Assigns attack pos based off direction its facing
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
	MParryBot.rad = rad;						// I know it says rad but hitbox will be a square so we just leave it as it is
	MParryBot.hp = max_hp;						// Dummy starts with max health
	MParryBot.max_hp = max_hp;
	MParryBot.damage = 10.f;					// Adjust damage here!
	MParryBot.AttackHitboxWidth = 150.f;		// Adjust Hitbox width and height here!
	MParryBot.AttackHitboxHeight = 75.f;
	MParryBot.state = Idle;						// Starting state = idle

	MParryBot.spark_buffer = 0.5;				// Timing for when spark is drawn on player before attack starts (in seconds)
	MParryBot.spark_draw = false;

	MParryBot.hitbox = Init_AABB(MParryBot.pos.x, MParryBot.pos.y, MParryBot.rad, MParryBot.rad);		// Init hitbox
	MParryBot.hitboxWidth = MParryBot.hitbox.max.x - MParryBot.hitbox.min.x;
	MParryBot.hitboxHeight = MParryBot.hitbox.max.y - MParryBot.hitbox.min.y;

	MParryBot.idle_timer = Create_Timer(1);			// Adjust timer lengths (in seconds) here! More details found in Enemies.h
	MParryBot.windup_timer = Create_Timer(1);
	MParryBot.attack_timer = Create_Timer(1);
	MParryBot.stun_timer = Create_Timer(3);
	MParryBot.respawn_timer = Create_Timer(3);
}

void Load_DummyBot()
{
	meleebot_sprite = SpriteSheet("Assets/Enemies/Dummy1.png", 7, 5);
}

void Unload_DummyBot()
{
	meleebot_sprite.Unload();
}

int MParryBot_last_hit = 0;							// Checks for single hit detections
int MParryBot_new_hit = 0;

// Runs each frame - for dummy logic
void Active_DummyBotMelee() 
{
	if (MParryBot.state != Destroyed) 
	{				// If dummy is still alive

		// Reset last hit if no new melee hit detected
		if (!MParryBot_new_hit)
		{
			MParryBot_last_hit = 0;
		}

		if (Attacked_Melee_rect(MParryBot.pos, MParryBot.hitboxWidth, MParryBot.hitboxHeight, &MParryBot_new_hit, &MParryBot.damage))		// If dummy was hit
		{
			if (MParryBot_last_hit != MParryBot_new_hit)			// Single hit detection
			{
				if (player.ammo < 3)								// Give player a ranged projectile on hit (capped at 3)
				{
					player.ammo++;
				}
				MParryBot.hp -= MParryBot.damage;					// Bot takes damage
				MParryBot_last_hit = MParryBot_new_hit;				// So bot only gets hit once
			}
		}

		if (Attacking_Ranged_Impact(MParryBot.pos, MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight, &MParryBot.damage))			// Same as above but for ranged attack
		{
			MParryBot.hp -= MParryBot.damage;
		}


		if (MParryBot.hp <= 0)										// If dummy is dead :(
		{
			MParryBot.respawn_timer.current = 0.f;					// Start respawn timer AND BRING HIM BACK TO LIFE
			MParryBot.state = Destroyed;							// Set state to destroyed - doesnt repeat its usual cycle
		}

		switch (MParryBot.state)									// Depending on current state
		{
		case Idle:
			if (Update_Timer(&MParryBot.idle_timer)) {
				//timer has not ended
			}
			else 
			{
				MParryBot.windup_timer.current = 0.f;				// Idle timer has ended, change states and start timer for next state
				MParryBot.state = WindUp;
			}
			break;
		case WindUp:
			if (Update_Timer(&MParryBot.windup_timer)) {
				//timer has not ended
				if (MParryBot.windup_timer.current >= MParryBot.windup_timer.end - MParryBot.spark_buffer) {		// Depending on buffer length, set flag to draw spark
					MParryBot.spark_draw = true;
				}
			}
			else 
			{
				MParryBot.spark_draw = false;						// Clear flag
				MParryBot.attack_timer.current = 0.f;				// Windup timer has ended, change states and start timer for next state
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
				MParryBot.idle_timer.current = 0.f;					// Attacking timer has ended, change states and start timer for next state
				MParryBot.state = Idle;
			}
			break;
		case Parried:
			if (Update_Timer(&MParryBot.stun_timer)) {
				//timer has not ended
			}
			else 
			{
				MParryBot.idle_timer.current = 0.f;					// Parried timer has ended, change states and start timer for next state
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
			MParryBot.idle_timer.current = 0.f;						// Respawn timer has ended, change states and start timer for next state
			MParryBot.hp = MParryBot.max_hp;
			MParryBot.state = Idle;
		}
	}
}

void Attack_DummyBotMelee() 
{
	if (AETestCircleToRect(&player.pos, player.radius, &MParryBot.attackpos, MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight))		// If dummy attacks player
	{
		switch (player.state) {										// Depending on player state, there are different outcomes
		case PARRY:
			// Player parried the bot - Swap states to parried
			MParryBot.stun_timer.current = 0.f;
			MParryBot.state = Parried;
			Parried_Hit();
			break;
		case BLOCK:
			Blocked_Hit(MParryBot.damage);							// Player blocks damage - takes reduced damage
			break;
		case PARRIED:
		case BLOCKED:
		case HIT:
		case DODGE:
			// do nothing, immune
			break;
		default:
			Eat_Hit(MParryBot.damage);								// Player takes full damage
		}
	}
}

void Draw_DummyBotMelee() 
{
	DrawCircleAdvanced(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y - MParryBot.rad * 0.5f), 35.f, 20.f, SetColor(0x92000000));			// Draw shadow under player
	//jq the almighty animation lord please do ur magic these drawrects look UNPLEASANT
	switch (MParryBot.state)										// Placeholder sprites - Draws circles of different colours depending on state its in
	{
	case Idle:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_idle);
		// JQWERTY - Draw Idle Anim
		meleebot_sprite.Set_Row(0);
		meleebot_sprite.Loop(0, 1, 1.f);
		break;
	case WindUp:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_windup);
		// JQWERTY - Draw raising up sword to swing Anim
		meleebot_sprite.Set_Row(1);
		meleebot_sprite.Set_Col(0, 5, MParryBot.windup_timer.current);
		break;
	case Swinging:
		DrawRect(World_To_Screen_x(MParryBot.attackpos.x), World_To_Screen_y(MParryBot.attackpos.y), MParryBot.AttackHitboxWidth, MParryBot.AttackHitboxHeight, color_windup);			// Draw 'hitbox'
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_swing);													// Draw attack range
		// JQWERTY - Draw Swinging Anim (can just rip out of the player swing smear frames)
		meleebot_sprite.Set_Row(2);
		meleebot_sprite.Set_Col(0, 1, MParryBot.attack_timer.current);
		break;
	case Parried:
		DrawRect(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y), MParryBot.rad, MParryBot.rad, color_parried);
		// JQWERTY - Draw stunned SILLY Anim
		meleebot_sprite.Set_Row(4);
		meleebot_sprite.Loop(0, 2, 1.f);
		break;
	}
	if (MParryBot.spark_draw) {
		//std::cout << "draw cool spark like shiiiing sheeeeoonng not the supermarket but like the sound ykwim shiiiing and also play the sound like the one i just made shiiiing ok ty\n";
		//add spark anim here :), its pos is alr synced with enem anim so just need to draw here :)
		// JQWERTY - this is for the parry warning spark
	}

	meleebot_sprite.Draw(World_To_Screen_x(MParryBot.pos.x), World_To_Screen_y(MParryBot.pos.y+50.f), 250.f, 250.f, false);

}


//RANGED PARRY BOT FUNCTIONS:
// Init            - Inits values such as timers etc
// Active (Update) - Call in Update(), handles checking of states
// Attack          - Call in Attacking state, handles logic for parrying, damage detection etc, i know the attack state is 'swinging' but yeah
//Draw             - Render sprites
void Init_DummyBotRange(AEVec2 pos, f32 rad, f32 max_hp) {
	RParryBot.pos = pos;
	RParryBot.FacingDir = RParryBot.FacingDown;								// Init here which dir bot should face!!!
	switch (RParryBot.FacingDir) {											// Depending on direction its facing, attack position will be adjusted!
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
	RParryBot.max_hp = max_hp;					// Dummy starts with max health
	RParryBot.damage = 10.f;					// Adjust damage here!
	RParryBot.projectile_speed = 350.f;			// Adjust projectile speed here! 
	RParryBot.projectile_radius = 20.f;			// Adjust projectile radius here!
	RParryBot.projectile_range = 500.f;			// Adjust distance projectile travels before being destroyed here!
	RParryBot.projectile_parry = false;			// Flags for when projectile is parried and to draw projectile
	RParryBot.projectile_draw = false;
	RParryBot.AttackHitboxWidth = 150.f;		// Adjust Hitbox width and height here!
	RParryBot.AttackHitboxHeight = 75.f;
	RParryBot.state = Idle;						// Starting state = idle

	RParryBot.hitbox = Init_AABB(RParryBot.pos.x, RParryBot.pos.y, RParryBot.rad, RParryBot.rad);		// Init hitbox
	RParryBot.hitboxWidth = RParryBot.hitbox.max.x - RParryBot.hitbox.min.x;
	RParryBot.hitboxHeight = RParryBot.hitbox.max.y - RParryBot.hitbox.min.y;
	RParryBot.projectile_pos = RParryBot.attackpos;

	RParryBot.idle_timer = Create_Timer(1);		// Adjust timer lengths (in seconds) here! More details found in Enemies.h
	RParryBot.windup_timer = Create_Timer(1);
	RParryBot.attack_timer = Create_Timer(3);
	RParryBot.stun_timer = Create_Timer(3);
	RParryBot.respawn_timer = Create_Timer(3);
}


int RParryBot_last_hit = 0;
int RParryBot_new_hit = 0;

void Active_DummyBotRange() {
	if (RParryBot.state != Destroyed) {			// If dummy is still alive

		// Reset last hit if no new melee hit detected
		if (!RParryBot_new_hit)
		{
			RParryBot_last_hit = 0;
		}

		if (Attacked_Melee_rect(RParryBot.pos, RParryBot.hitboxWidth, RParryBot.hitboxHeight, &RParryBot_new_hit, &RParryBot.damage))		// If dummy was hit
		{
			if (RParryBot_last_hit != RParryBot_new_hit)			// Single hit detection
			{
				if (player.ammo < 3)								// Give player a ranged projectile on hit (capped at 3)
				{
					player.ammo++;
				}
				RParryBot.hp -= RParryBot.damage;					// Bot takes damage
				RParryBot_last_hit = RParryBot_new_hit;				// So bot only gets hit once
			}
		}

		if (Attacking_Ranged_Impact(RParryBot.pos, RParryBot.AttackHitboxWidth, RParryBot.AttackHitboxHeight, &RParryBot.damage))			// Same as above but for ranged attack
		{
			RParryBot.hp -= RParryBot.damage;
		}


		if (RParryBot.hp <= 0)										// If dummy is dead :(
		{
			RParryBot.respawn_timer.current = 0.f;					// Start respawn timer AND BRING HIM BACK TO LIFE
			RParryBot.state = Destroyed;
		}

		switch (RParryBot.state)									// Depending on current state
		{
		case Idle:
			if (Update_Timer(&RParryBot.idle_timer)) {
				//timer has not ended
			}
			else 
			{
				RParryBot.windup_timer.current = 0.f;				// Idle timer has ended, change states and start timer for next state
				RParryBot.state = WindUp;
			}
			break;
		case WindUp:
			if (Update_Timer(&RParryBot.windup_timer)) {
				//timer has not ended
			}
			else 
			{
				RParryBot.attack_timer.current = 0.f;				// Windup timer has ended, change states and start timer for next state
				RParryBot.state = Swinging;
			}
			break;
		case Swinging:
			RParryBot.projectile_draw = true;
			Attack_DummyBotRange();
			if (Update_Timer(&RParryBot.attack_timer)) {			// idk why when i or this if statement it dont work so pls pardon this UNPLEASANT code
				if (AEVec2Distance(&RParryBot.pos, &RParryBot.projectile_pos) > RParryBot.projectile_range)		// If projectile travels too far, 
				{
					RParryBot.projectile_draw = false;
					RParryBot.idle_timer.current = 0.f;
					RParryBot.projectile_pos = RParryBot.attackpos;			// Projectile spawns from attack pos
					RParryBot.state = Idle;
				}
			}
			else 
			{
				RParryBot.projectile_draw = false;
				RParryBot.idle_timer.current = 0.f;					// Attacking timer has ended, change states and start timer for next state
				RParryBot.projectile_pos = RParryBot.attackpos;
				RParryBot.state = Idle;
			}
			break;
		case Parried:
			//REPLACE THIS BECAUSE DONT MAKE SENSE FOR PARRIED PROJECTILE TO STUN ENEMY
			Parried_Hit();
			if (Update_Timer(&RParryBot.stun_timer)) {
				//timer has not ended
			}
			else 
			{
				RParryBot.idle_timer.current = 0.f;					// Parried timer has ended, change states and start timer for next state
				RParryBot.state = Idle;
			}
		}

	}
	else {
		//wait for timer to reset, play animation of respawn
		if (Update_Timer(&RParryBot.respawn_timer)) {
			//timer has not ended
			RParryBot.projectile_draw = false;
		}
		else 
		{
			RParryBot.idle_timer.current = 0.f;						// Respawn timer has ended, change states and start timer for next state
			RParryBot.hp = RParryBot.max_hp;
			RParryBot.state = Idle;
		}
	}
}

void Attack_DummyBotRange() {
	//TO DO: ON PARRY - DEFLECT PROJECTILE BACK AT ENEMY
	switch (RParryBot.FacingDir)									// Depending on direction dummy is facing, adjust projectile direction and speed
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

	if (AETestCircleToCircle(&player.pos, player.radius, &RParryBot.projectile_pos, RParryBot.projectile_radius))			// If projectile hits player
	{
		switch (player.state) {						// Depending on player state, there are different outcomes
		case PARRY:
			RParryBot.stun_timer.current = 0.f;
			RParryBot.state = Parried;				// change to just deflect proj, currently bot will get parried upon projectile parry
			RParryBot.projectile_parry = true;
			RParryBot.projectile_draw = false;		// set to true once deflect logic implemented
			break;
		case BLOCK:									// Player blocks damage - takes reduced damage
			Blocked_Hit(RParryBot.damage);
			RParryBot.projectile_draw = false;
			break;
		case PARRIED:
		case BLOCKED:
		case HIT:
		case DODGE:
			// do nothing, immune
			RParryBot.projectile_draw = false;		// Stop drawing
			break;
		default:
			Eat_Hit(RParryBot.damage);				// Player takes full damage
			RParryBot.projectile_draw = false;		// Stop drawing
		}
	}
}

void Draw_DummyBotRange() {
	DrawCircleAdvanced(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y - RParryBot.rad * 0.5f), 35.f, 20.f, SetColor(0x92000000));			// Draw shadow under player
	//jq the almighty animation lord please do ur magic these drawrects look ASS
	switch (RParryBot.state)						// Placeholder sprites - Draws circles of different colours depending on state its in
	{
	case Idle:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_idle);
		// JQWERTY - Draw Idle Anim
		break;
	case WindUp:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_windup);
		// JQWERTY - Draw Raising gun/ Hunkering down before shooting Anim
		break;
	case Swinging:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_swing);
		// JQWERTY - Draw shooting Anim (projecile drawing below)
		break;
	case Parried:
		DrawRect(World_To_Screen_x(RParryBot.pos.x), World_To_Screen_y(RParryBot.pos.y), RParryBot.rad, RParryBot.rad, color_parried);
		// JQWERTY - This one tbh can just ignore for now, we'll discuss the deflecting projectiles another time then implement
	}
	if (RParryBot.projectile_draw)		// If projectile needs to be drawn
	{
		//Temp Projectile Drawing
		DrawCircle(World_To_Screen_x(RParryBot.projectile_pos.x), World_To_Screen_y(RParryBot.projectile_pos.y), RParryBot.projectile_radius, color_stunned);		// Draw circle at projectile's position
		// JQWERTY - Draw steam/ laser bullet sprite here (currently rotation not yet implemented so itll look funky)
	}
}

void Init_Enemy_Indicator()								// For colours!!!!
{
	color_idle = SetColor(0xFFFF6961);
	color_windup = SetColor(0xFFFFCF61);
	color_swing = SetColor(0xFFAFFF61);
	color_stunned = SetColor(0xFF000000);
	color_parried = SetColor(0xFFFF00FF);
}