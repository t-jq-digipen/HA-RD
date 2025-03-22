//---------------------------------------------------------
// file:	SpriteSheet.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Spritesheet class declaration
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "Boss2.h"

// Boss projectile settings
#define MAX_PROJECTILES 3
#define PROJECTILE_DAMAGE 10

// Boss instance
Boss2 boss_2;

// Boss state colors
//Color boss_idle, boss_windup, boss_swing, boss_stunned, boss_parried, boss_projectile_indicator, boss_projectile_color, boss_death_explosion_color;
Color red2 = SetColor(0xFFFF0000);
Color green2 = SetColor(0xFF00FF00);
Color blue2 = SetColor(0xFF0000FF);
// Sprite sheets
/*
/ fill here
*/

// Boss spawn position
const f32 BOSS2_SPAWN_X = 0.f;
const f32 BOSS2_SPAWN_Y = 200.f;

// Boss size
const f32 BOSS2_WIDTH = 250.f;
const f32 BOSS2_HEIGHT = 250.f;

// Boss draw size
/*
/ fill here
*/

// Boss hitbox size
const f32 BOSS2_HITBOX_WIDTH = 200.f;
const f32 BOSS2_HITBOX_HEIGHT = 150.f;

// Boss attributes
const f32 BOSS2_MAXHP = 150.f;
const f32 BOSS2_VEL = 10.f;

// Attack timings
const s32 ATTACK_TYPES = {5};
const f32 BOSS2_WINDUP[ATTACK_TYPES] = {2.f,0.f,0.5f,0.5f,0.5f};
const f32 BOSS2_ATTACK[ATTACK_TYPES] = {5.f,0.f,1.f,1.f,1.f};
const f32 BOSS2_RECOVER[ATTACK_TYPES] = {0.5f,0.f,1.f,1.f,1.f };
const f32 BOSS2_STUN[ATTACK_TYPES] = {0.f,0.f,3.f,3.f,0.f};

// Attack information
Boss2_Attack boss_attack[ATTACK_TYPES];


// Attack hitbox settings
const f32 OFFSET_RANGE_X = 250.f / 2;  // Half of player width
const f32 OFFSET_RANGE_Y = 250.f / 2; // Half of player height

const f32 SLASHWIDTH = 250.f;
const f32 SLASHHEIGHT = 250.f;

const f32 DASH_OFFSET = 100.f;

// Miscellaneous boss settings
bool will_hit2 = true;
std::vector<Missile> missiles(4);  // Vector to store active missiles
bool prepdone = false;
bool rocketset = false;
bool prepdash = false;
bool dashed = false;
bool dashright = false;
bool dashleft = false;
// Load boss assets
void Load_Boss2() 
{

}

// Init boss2 indicators
void Init_Boss2_Indicator() 
{

}
void Init_Boss2() 
{
    // Boss starting point, hitbox and size
    boss_2.pos = { BOSS2_SPAWN_X, BOSS2_SPAWN_Y };
    boss_2.width = BOSS2_WIDTH;
    boss_2.height = BOSS2_HEIGHT;
    boss_2.hitbox = Init_AABB(boss_2.pos.x, boss_2.pos.y, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT);
    boss_2.hp = boss_2.max_hp = BOSS2_MAXHP;

    // Starting state & phase
    boss_2.phase = PHASE1;
    boss_2.state = BOSS2_IDLE;
   /* boss_1.attack_type = */
    boss_2.attackstate = BOSS2_ATTACK_WINDUP;

    // Timers
    boss_2.idle_timer = Create_Timer(1.f);
    boss_2.attack_timer = Create_Timer(3.f);
    boss_2.stun_timer = Create_Timer(3.f);
    boss_2.missile_prep = Create_Timer(1.f);

    // Initialize melee attack properties
    for (s32 i = 0; i < ATTACK_TYPES; i++)
    {
        boss_attack[i].attack_windup = BOSS2_WINDUP[i];
        boss_attack[i].attack_active = boss_attack[i].attack_windup + BOSS2_ATTACK[i];
        boss_attack[i].attack_recover = boss_attack[i].attack_active + BOSS2_RECOVER[i];
        boss_attack[i].timer = Create_Timer(boss_attack[i].attack_recover);
        boss_attack[i].stun_duration = BOSS2_STUN[i]; 
        boss_attack[i].can_parry = boss_attack[i].stun_duration > 0;
    }

    Init_Boss_UI(boss_2.hp);
}

// For damagecheck for player attacks to boss
s32 last_hit2 = 0;
s32 new_hit2 = 0;
f32 damage2 = 0.f;

void Update_Boss2()
{
    if (boss_2.state != BOSS2_DEAD)
    {
        switch (boss_2.phase)
        {
        case(PHASE1):
            switch (boss_2.state)
            {
            case BOSS_IDLE:
                if (!Update_Timer(&boss_2.idle_timer))
                {
                    // Set randomizer
                    f32 randomizer = AERandFloat();

                    if (randomizer <= 0.4f) // 40% probability for Missile Barrage
                    {
                        // Do Missile Barrage
                        /*boss_2.attack_timer.current = 0.f;
                        boss_attack[MISSILE_BARRAGE].timer.current = 0.f;
                        boss_2.missile_prep.current = 0.f;
                        boss_2.attack_type = MISSILE_BARRAGE;
                        boss_2.state = BOSS2_MISSILE_BARRAGE;*/
                        boss_2.attack_timer.current = 0.f;
                        boss_attack[DASH_SLASH1].timer.current = 0.f;
                        boss_attack[DASH_SLASH2].timer.current = 0.f;
                        boss_2.attack_type = DASH;
                        boss_2.state = BOSS2_DASH_SLASH;
                    }
                    else if (randomizer <= 0.8f) // 40% probability for Dash & Slash
                    {
                        // Do Dash & Slash
                        boss_2.attack_timer.current = 0.f;
                        boss_attack[DASH_SLASH1].timer.current = 0.f;
                        boss_attack[DASH_SLASH2].timer.current = 0.f;
                        boss_2.attack_type = DASH;
                        boss_2.state = BOSS2_DASH_SLASH;
                        
                    }
                    else // 20% probability for Lightning Wave
                    {
                        // Do Lightning Wave
                        /*boss_2.attack_timer.current = 0.f;
                        boss_attack[LIGHTNING_WAVE].timer.current = 0.f;
                        boss_2.attack_type = LIGHTNING_WAVE;
                        boss_2.state = BOSS2_LIGHTNING_WAVE;*/
                        boss_2.attack_timer.current = 0.f;
                        boss_attack[DASH_SLASH1].timer.current = 0.f;
                        boss_attack[DASH_SLASH2].timer.current = 0.f;
                        boss_2.attack_type = DASH;
                        boss_2.state = BOSS2_DASH_SLASH;
                    }
                }
                break;

            case BOSS2_DASH_SLASH:
                Dash_Slash_Logic();
                break;

            case BOSS2_MISSILE_BARRAGE:
                Dash_Slash_Logic();
                // Missile_Barrage_Logic();
                break;

            case BOSS2_LIGHTNING_WAVE:
                Dash_Slash_Logic();
                //Lightning_Wave_Logic(); 
                break;

            case BOSS2_PARRIED:
                if (!Update_Timer(&boss_2.stun_timer))
                {
                    boss_2.state = BOSS2_IDLE;
                }
                break;

            default:
                boss_2.state = BOSS2_IDLE;
                break;
            }
        case(PHASE2):
            break;
        case(PHASE3):
            break;
        default:
            break;
        }

        // Reset last hit if no new melee hit detected
        if (!new_hit2)
        {
            last_hit2 = 0;
        }

        // Check for player melee attack collision
        if (Attacked_Melee_rect(boss_2.pos, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT, &new_hit2, &damage2))
        {
            if (last_hit2 != new_hit2)
            {
                if (player.ammo < 3)
                {
                    player.ammo++;
                }
                boss_2.hp -= damage2;
                last_hit2 = new_hit2;
            }
        }

        // Check for player projectile collisions
        if (Attacking_Ranged_Impact(boss_2.pos, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT, &damage2))
        {
            boss_2.hp -= damage2;
            std::cout << "Boss hit! HP: " << boss_2.hp << '\n';
        }


        // Clamp HP to prevent over-reduction
        boss_2.hp = AEClamp(boss_2.hp, 0.f, BOSS2_MAXHP);

        if (boss_2.hp <= 0.f)
        {
            boss_2.state = BOSS2_DEAD;
        }

        // Update Boss UI
        Update_Boss_UI(boss_2.hp);
    }

    // Update Boss Hitbox
    Update_AABB(&boss_2.hitbox, boss_2.pos.x, boss_2.pos.y, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT);
}
void Draw_Boss2_Hitboxes() 
{
    f32 timer_progress = boss_2.attack_timer.current * boss_attack[boss_2.attack_type].timer.duration;
    switch (boss_2.state) 
    {
    case (BOSS2_MISSILE_BARRAGE):
        if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
        {
            for (const auto& missile : missiles)
            {
                if (missile.active)
                {
                    DrawCircle(World_To_Screen_x(missile.targetpos.x), World_To_Screen_y(missile.targetpos.y), missile.radius, red2);
                }
            }
        }
        if(boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
        {
            for (const auto& missile : missiles) 
            {
                if (missile.active) 
                {
                    DrawCircle(World_To_Screen_x(missile.targetpos.x), World_To_Screen_y(missile.targetpos.y), missile.radius, red2);
                }
            }
        }
        break;
    case (BOSS2_DASH_SLASH):
        switch (boss_2.attack_type)
        {
        case (DASH_SLASH1):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for sweep
                DrawRect(World_To_Screen_x(boss_2.target_pos.x), World_To_Screen_y(boss_2.target_pos.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_pos.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHHEIGHT / 2.f),
                    World_To_Screen_y(boss_2.target_pos.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, green2);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active sweep hitbox
                DrawRect(World_To_Screen_x(boss_2.target_pos.x), World_To_Screen_y(boss_2.target_pos.y), SLASHWIDTH, SLASHHEIGHT, blue2);

            }
            break;
        case (DASH_SLASH2):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for sweep
                DrawRect(World_To_Screen_x(boss_2.target_pos.x), World_To_Screen_y(boss_2.target_pos.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_pos.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHHEIGHT / 2.f),
                    World_To_Screen_y(boss_2.target_pos.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, green2);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active sweep hitbox
                DrawRect(World_To_Screen_x(boss_2.target_pos.x), World_To_Screen_y(boss_2.target_pos.y), SLASHWIDTH, SLASHHEIGHT, blue2);
            }
            break;
        }
        break;
    default:
        break;
    }
}
void Draw_Boss2()
{
    for (const auto& missile : missiles) {
        if (missile.active) {
            DrawCircle(World_To_Screen_x(missile.pos.x), World_To_Screen_y(missile.pos.y), missile.radius, green2);
        }
    }

    DrawRect(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS2_WIDTH, BOSS2_HEIGHT, red2);
}
void Unload_Boss2()
{

}
void Boss2_Attack_Test(bool canparry, f32 boss_damage)
{
    will_hit2 = false;
    switch (player.state)
    {
    case PARRY:
        // Stun logic
        if (canparry)
        {
            boss_2.stun_timer.current = 0.f;
            boss_2.stun_timer.duration = boss_attack[boss_2.attack_type].stun_duration;
            boss_2.state = BOSS2_PARRIED;
            //boss_2.attack_type = 0;
            bullet_time.current = 0.f;
            std::cout << "Boss parried\n";
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
        // Do nothing, immune
        break;
    default:
        Eat_Hit(boss_damage);
    }
}

void Dash_Slash_Logic() 
{
    if (!prepdash)
    {
        std::cout << "PREPPING DASH \n";
        f32 randomizer = AERandFloat();
        if (randomizer < 0.5f)
        {
            // Set the target position to the player's right
            boss_2.target_pos.y = player.pos.y;
            boss_2.target_pos.x = player.pos.x + DASH_OFFSET;
            dashright = true;
        }
        else
        {
            // Set the target position to the player's left
            boss_2.target_pos.y = player.pos.y;
            boss_2.target_pos.x = player.pos.x - DASH_OFFSET;
            dashleft = true;
        }
        // Calculate the direction vector
        boss_2.target_dir = { player.pos.x - boss_2.pos.x, player.pos.y - boss_2.pos.y };
        AEVec2Normalize(&boss_2.target_dir, &boss_2.target_dir);
        boss_2.vel.x = boss_2.target_dir.x * BOSS2_VEL;
        boss_2.vel.y = boss_2.target_dir.y * BOSS2_VEL;
        prepdash = true;
    }
    if (prepdash && !dashed)
    {
        boss_2.pos.x += boss_2.vel.x;
        boss_2.pos.y += boss_2.vel.y;
        f32 distance_to_target = AEVec2Distance(&boss_2.pos, &boss_2.target_pos) - 100.f;
        std::cout << distance_to_target<< '\n';
        if (distance_to_target <= 5.f /*buffer*/)
        {
            std::cout << "DASH STOPP \n";
            boss_2.pos = boss_2.target_pos;
            if (dashleft)
            {
                boss_2.target_pos.x = boss_2.pos.x + DASH_OFFSET;
                boss_2.target_pos.y = boss_2.pos.y;
            }
            else if (dashright)
            {
                boss_2.target_pos.x = boss_2.pos.x - DASH_OFFSET;
                boss_2.target_pos.y = boss_2.pos.y;
            }
            boss_2.attack_type = DASH_SLASH1;
            dashed = true;
        }
    }
    if (dashed && Update_Timer(&boss_2.attack_timer))
    {
        f32 timer_progress = boss_2.attack_timer.current * boss_attack[boss_2.attack_type].timer.duration;
        if (timer_progress < boss_attack[boss_2.attack_type].attack_windup)
        {
            // Attack is winding up
            will_hit2 = true;
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;

        }
        else if (dashed && timer_progress < boss_attack[boss_2.attack_type].attack_active)
        {
            // Attack is active (check collision)
            boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

            // Check for collision with the player
            if (AETestCircleToRect(&player.pos, player.radius, &boss_2.target_pos, SLASHWIDTH, SLASHHEIGHT))
            {
                // Parry checks
                if (will_hit2)
                {
                    Boss_Attack_Test(boss_attack[boss_2.attack_type].can_parry, 30.f);
                }
            }
        }
        else if (timer_progress < boss_attack[boss_2.attack_type].attack_recover)
        {
            // Attack is recovering
            boss_2.attackstate = BOSS2_ATTACK_RECOVER;
        }
    }
    if(dashed && !Update_Timer(&boss_2.attack_timer))
    {
        if (boss_2.attack_type == DASH_SLASH1)
        {
            boss_2.attack_type = DASH_SLASH2;
            boss_attack[boss_2.attack_type].timer.current = 0.f;
            boss_2.attack_timer = boss_attack[boss_2.attack_type].timer;
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
        }
        else
        {
            boss_2.state = BOSS2_IDLE;
            prepdash = false;
            dashed = false;
            dashright = false;
            dashleft = false;
            boss_2.idle_timer.current = 0.f;
        }
    }
}

void Missile_Barrage_Logic() {
    
    if (rocketset == false) 
    {
        std::cout << "INITIALIZING ROCKETS \n";
        // Initialize missile positions and velocities during prep phase
        for (size_t i = 0; i < missiles.size(); ++i) {
            missiles[i].pos = boss_2.pos; // Start from the boss's center
            missiles[i].active = true;   // Activate the missile
            missiles[i].radius = 50.f;
            // Set velocities based on missile index
            switch (i) {
            case 0: 
                missiles[i].velocity = { -1.f, 0.f }; // Move left
                break;
            case 1: 
                missiles[i].velocity = { -0.707f, 0.707f }; // Move top-left (normalized diagonal)
                break;
            case 2: 
                missiles[i].velocity = { 0.707f, 0.707f }; // Move top-right (normalized diagonal)
                break;
            case 3: 
                missiles[i].velocity = { 1.f, 0.f }; // Move right
                break;
            default:
                break;
            }

            // Normalize the velocity to ensure consistent speed
            AEVec2Normalize(&missiles[i].velocity, &missiles[i].velocity);

            missiles[i].velocity.x *= 5.f; // Adjust speed as needed
            missiles[i].velocity.y *= 5.f; // Adjust speed as needed
        }
        rocketset = true;
    }
    if (rocketset && !prepdone && Update_Timer(&boss_2.missile_prep))
    {
        std::cout << "SHOOTING OUT ROCKETS \n";
        for (size_t i = 0; i < missiles.size(); ++i) {
            missiles[i].pos.x += missiles[i].velocity.x;
            missiles[i].pos.y += missiles[i].velocity.y;
        }
    }
    else prepdone = true; // Mark prep as done

    if (prepdone&&Update_Timer(&boss_2.attack_timer)) {
        f32 timer_progress = boss_2.attack_timer.current * boss_attack[boss_2.attack_type].timer.duration;

        if (timer_progress < boss_attack[boss_2.attack_type].attack_windup) {
            std::cout << "WINDUP \n";
            will_hit2 = true;
            // Attack is winding up
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;

            // Set the target position to the player's current position
            for (size_t i = 0; i < missiles.size(); ++i) {
                missiles[i].targetpos = GetRandomTargetPosAroundPlayer();
            }

            // Calculate the direction vector for each missile
            for (size_t i = 0; i < missiles.size(); ++i) {
                missiles[i].velocity = { missiles[i].targetpos.x - missiles[i].pos.x, missiles[i].targetpos.y - missiles[i].pos.y };
                AEVec2Normalize(&missiles[i].velocity, &missiles[i].velocity);
                missiles[i].velocity.x *= 20.f; // Speed
                missiles[i].velocity.y *= 20.f; 
            }
        }
        else if (timer_progress < boss_attack[boss_2.attack_type].attack_active && !missiles.empty()) 
        {
            std::cout << "ACTIVE \n";
            // Attack is active (missiles fly to their targets)
            boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

            // Update missile positions
            for (size_t i = 0; i < missiles.size(); ++i) 
            {
                if (missiles[i].active) 
                {
                    // Move the missile based on its velocity
                    missiles[i].pos.x += missiles[i].velocity.x;
                    missiles[i].pos.y += missiles[i].velocity.y;

                    // Check if missile reaches its target position
                    if (AEVec2Distance(&missiles[i].pos, &missiles[i].targetpos) < 10.f /*buffer*/)
                    { 
                        if (AETestCircleToCircle(&missiles[i].targetpos, missiles[i].radius, &player.pos, player.radius))
                        {
                            // Player damage calculation
                            if (will_hit2)
                            {
                                Boss2_Attack_Test(boss_attack[MISSILE_BARRAGE].can_parry, 30.f);
                            }
                        }
                        missiles[i].active = false; // Deactivate missile if it reaches the target
                        break;
                    }
                }
            }
        }
        else if (prepdone && timer_progress < boss_attack[boss_2.attack_type].attack_recover) {
            std::cout << "Recover \n";
            // Attack is recovering
            boss_2.attackstate = BOSS2_ATTACK_RECOVER;

            // Deactivate missiles
            for (auto& missile : missiles) {
                missile.active = false;
            }
        }
    }
    else if (prepdone) {
        // If the attack timer is done, transition back to idle
        boss_2.state = BOSS2_IDLE;
        boss_2.idle_timer.current = 0.f;
        missiles.clear(); // Clear missiles for the next attack
        missiles.resize(4); // Reinitialize with 4 missiles
        rocketset = false;
        prepdone = false; // Reset prep flag
    }
}

void Lightning_Wave_Logic()
{

}

AEVec2 GetRandomTargetPosAroundPlayer() {
    f32 offsetX = AERandFloat() * OFFSET_RANGE_X * 2 - OFFSET_RANGE_X; // Random X offset
    f32 offsetY = AERandFloat() * OFFSET_RANGE_Y * 2 - OFFSET_RANGE_Y; // Random Y offset
    return { player.pos.x + offsetX, player.pos.y + offsetY };
}