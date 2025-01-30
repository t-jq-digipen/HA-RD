//---------------------------------------------------------
// file:	Boss1.cpp
// author:	MANFRED SIM
// email:	manfred.sim@digipen.edu
//
// brief:	Implementation of boss 1 structs and functions
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "Boss1.h"
#include "player.h"
#include "Camera.h"
#include <vector>

Boss1 boss1;
Color proj_color;

typedef struct Boss1_Projectile 
{
	AEVec2 pos;
	AEVec2 velocity;
	AEMtx33 transform;
	Timer timer;
	bool active;
};

const float PROJECTILE_SPEED = 500.f;
std::vector<Boss1_Projectile> projs;

void Init_Boss1()
{
	proj_color = SetColor(0xFFFF13F0);

	boss1.pos = { 0,450 };
	boss1.current_hp = 100.f;
	boss1.max_hp = 100.f;
	boss1.size = 200.f;
	boss1.isattacking = 0;
	boss1.attackInterval = Create_Timer(3.f);
	boss1.color = SetColor(0xFF800080);
	boss1.phase = PHASE_1;
}

void Draw_Boss1() 
{
	// Draw the boss' projectiles
	for (const auto& proj : projs)
	{
		if (proj.active)
		{
			DrawCircle(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 25.f, proj_color); // Example projectile size and color
		}
	}

	// Draw the boss
	if (boss1.current_hp > 0.f)
	{
		DrawCircle(World_To_Screen_x(boss1.pos.x), World_To_Screen_y(boss1.pos.y), boss1.size, boss1.color);
	}
}

void Update_Boss1()
{
	// Delta time
	dt = (f32)AEFrameRateControllerGetFrameTime();

	boss1.current_hp -= 3.f * dt;
	Update_Timer(&boss1.attackInterval);

	//std::cout << "Player x: " << player.pos.x << ", y: " << player.pos.y << '\n';


	boss1.attackInterval.current += 1.f * dt;
	if (boss1.attackInterval.current >= boss1.attackInterval.duration && boss1.phase != DEFEAT)
	{
		boss1.isattacking = 1;
		boss1.attackInterval.current = 0.f;
	}
	

	// trigger boss attack after a certain interval
	if (boss1.isattacking == 1 && boss1.phase != DEFEAT)
	{
		AEVec2 direction = { player.pos.x - boss1.pos.x, player.pos.y - boss1.pos.y };
		AEVec2Normalize(&direction, &direction);

		Boss1_ShootBullet(boss1.pos, direction);
		//std::cout << "Boss has attacked!!!" << '\n';
		boss1.isattacking = 0;
	}

	// when boss hp is depleted to 0
	if (boss1.phase == DEFEAT)
	{
		//std::cout << "Boss is DEAD (rest in pepperonis)" << '\n';
	}

	// determine attack speed based on phase
	switch (boss1.phase)
	{
		case PHASE_1:
			boss1.attackInterval.duration = 3.f;
			break;
		case PHASE_2:
			boss1.attackInterval.duration = 1.5f;
			break;
		case PHASE_3:
			boss1.attackInterval.duration = 0.5f;
			break;
		case DEFEAT:
			break;
		default:
			break;
	}

	// Update boss projectile movement
	for (auto& proj : projs) 
	{
		if (proj.active) 
		{
			proj.pos.x += proj.velocity.x * dt;
			proj.pos.y += proj.velocity.y * dt;
			proj.timer.current += 1.f * dt;

			// Update boss projectile transformation
			AEMtx33 trans, scale;
			AEMtx33Trans(&trans, proj.pos.x, proj.pos.y);
			AEMtx33Scale(&scale, 25.f, 25.f);
			AEMtx33Concat(&proj.transform, &trans, &scale);

			// Deactivate projectiles after 4 seconds (aka range)
			if (proj.timer.current >= proj.timer.duration) 
			{
				proj.active = false;
			}
		}
	}

	// Remove inactive boss projectiles
	projs.erase(std::remove_if(projs.begin(), projs.end(), [](const Boss1_Projectile& p) { return !p.active; }), projs.end());
}

void Boss1_ShootBullet(const AEVec2& start, const AEVec2& direction)
{
	Boss1_Projectile proj;
	proj.pos = start;
	proj.velocity = { direction.x * PROJECTILE_SPEED, direction.y * PROJECTILE_SPEED };
	proj.active = true;
	proj.timer = Create_Timer(4.f);
	AEMtx33Identity(&proj.transform);
	projs.push_back(proj);
}