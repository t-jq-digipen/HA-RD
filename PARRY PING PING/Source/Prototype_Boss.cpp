//---------------------------------------------------------
// file:	SpriteSheet.h
// author:	AVID PARRY ENJOYERS
// email:	-
//
// brief:	Spritesheet class declaration
//
// Copyright © 2025 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "Prototype_Boss.h"
#include "player.h"
#include "UI.h"
#include <vector>
#include "SpriteSheet.h"

// Boss projectile settings
#define MAX_PROJECTILES 3
#define PROJECTILE_DAMAGE 10

// Boss instance
Boss boss_1;

// Boss state colors
Color boss_idle, boss_windup, boss_swing, boss_stunned, boss_parried, boss_projectile_indicator, boss_projectile_color, boss_death_explosion_color;

// Sprite sheets
SpriteSheet_Old boss_sprite_sheet;
SpriteSheet boss_1_sprite;

// Boss spawn position
f32 BOSS_SPAWN_X;
f32 BOSS_SPAWN_Y;

// Boss size
f32 BOSS_WIDTH;
f32 BOSS_HEIGHT;

// Boss draw size
f32 BOSS_DRAW_WIDTH;
f32 BOSS_DRAW_HEIGHT;

// Boss hitbox size
f32 BOSS_HITBOX_WIDTH;
f32 BOSS_HITBOX_HEIGHT;

// Boss attributes
f32 BOSS_MAXHP;
f32 BOSS_DASH_DISTANCE; // Dash attack distance

// Attack timings
const s32 ATTACK_TYPES = 7;
std::vector<f32> BOSS_WINDUP;
std::vector<f32> BOSS_ATTACK;
std::vector<f32> BOSS_RECOVER;
std::vector<f32> BOSS_STUN;

// Attack information
Boss_Melee boss_melee[ATTACK_TYPES];

// RocketFist attributes
std::vector<RocketFist> rocket_fists;
f32 BOSS_PROJECTILE_RADIUS;
f32 BOSS_PROJECTILE_DAMAGE;
f32 BOSS_PROJECTILE_SPEED;

// Attack hitbox settings
f32 SMASH_RADIUS = 200.f;
f32 SMASH_DAMAGE = 30.f;
f32 BIG_SMASH_RADIUS = 350.f;
f32 SWEEP_WIDTH = 600.f;
f32 SWEEP_HEIGHT = 600.f;

// Explosion effects
std::vector<Boss_Explosion> boss_death_explosion;
f32 BOSS_DEATH_EXPLOSION_RADIUS;
Boss_Explosion explosion_particle;

// Boss shadow
f32 BOSS_SHADOW_WIDTH;
f32 BOSS_SHADOW_HEIGHT;

// Miscellaneous boss settings
f32 x_offset = 0.f;
bool will_hit = true;

// Level score update values
int SCORE_MELEE_HIT; // + 100 points (multiplied by damage value) per frame
int SCORE_BOSS_PROJECTILES_PARRIED; // + 5000 points if boss rocket successfully parried
int SCORE_BOSS_MELEE_PARRIED; // + 10000 points if boss successfully stunned via parry
f32 SCORE_TIME_BONUS_CAP; // + bonus points if player manages to beat boss within 1 minute or less
int SCORE_TIME_BONUS; // + 2000 points for every second of time left less than the 1 minute cap
int SCORE_DODGE; // + 2000 points if boss attack successfully dodged
int SCORE_PERFECT; // + 10000 points if player beats boss without taking damage

// File IO
std::vector<float> ProtoBoss_IO;
std::vector<float> ProtoBossScore_IO;

// Load boss assets
void Load_Boss()
{
    // Read File IO
    ReadFromFile("Assets/FileIO/ProtoBoss.txt", ProtoBoss_IO);
    ReadFromFile("Assets/FileIO/PlayerScore.txt", ProtoBossScore_IO);

    boss_sprite_sheet = Init_SpriteSheet("Assets/Proto_Monke/ProtoMonke.png", 5, 8, 1.f);
    boss_1_sprite = SpriteSheet("Assets/Bosses/Boss_1.png", 19, 25);
}

// Initialize boss attack indicators
void Init_Boss_Indicator()
{
    boss_idle       = SetColor(0xFFFF6961);
    boss_windup     = SetColor(0x508B0000);
    boss_swing      = SetColor(0x808B0000);
    boss_stunned     = SetColor(0xFF000000);
    boss_parried    = SetColor(0xFFFF00FF);
    boss_projectile_indicator = SetColor(0xFFFF0000);
    boss_projectile_color = SetColor(0xFFFF13F0);
    boss_death_explosion_color = SetColor(0xFFED6240);
}

// Initialize boss attributes
void Init_Boss()
{
    boss_death_explosion.clear(); // Clear previous explosions

    // Boss spawn position
    BOSS_SPAWN_X = ProtoBoss_IO[0];                                     // BOSS SPAWN X = 0.f
    BOSS_SPAWN_Y = ProtoBoss_IO[1];                                     // BOSS SPAWN Y = 200.f

    // Boss size
    BOSS_WIDTH = ProtoBoss_IO[2];                                       // BOSS WIDTH = 500.f
    BOSS_HEIGHT = ProtoBoss_IO[3];                                      // BOSS HEIGHT = 500.f

    // Boss draw size
    BOSS_DRAW_WIDTH = ProtoBoss_IO[4];                                  // BOSS DRAW WIDTH = 650.f
    BOSS_DRAW_HEIGHT = ProtoBoss_IO[5];                                 // BOSS DRAW HEIGHT = 650.f

    // Boss hitbox size
    BOSS_HITBOX_WIDTH = ProtoBoss_IO[6];                                // BOSS HITBOX WIDTH = 400.f
    BOSS_HITBOX_HEIGHT = ProtoBoss_IO[7];                               // BOSS HITBOX HEIGHT = 300.f

    // Boss attributes
    BOSS_MAXHP = ProtoBoss_IO[8];                                       // BOSS MAX HP = 200.f
    BOSS_DASH_DISTANCE = ProtoBoss_IO[9];                               // BOSS MAX DASH DISTANCE = 250.f

    // Attack timings

    // BOSS_WINDUP = { 0.8f, 0.8f, 0.8f, 0.8f, 1.f, 2.f, 2.f };
    BOSS_WINDUP = { ProtoBoss_IO[10], ProtoBoss_IO[11], ProtoBoss_IO[12], ProtoBoss_IO[13], ProtoBoss_IO[14], ProtoBoss_IO[15], ProtoBoss_IO[16] };

    // BOSS_ATTACK = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 1.5f, 1.5f };
    BOSS_ATTACK = { ProtoBoss_IO[17], ProtoBoss_IO[18], ProtoBoss_IO[19], ProtoBoss_IO[20], ProtoBoss_IO[21], ProtoBoss_IO[22], ProtoBoss_IO[23] };

    // BOSS_RECOVER = { 0.4f, 1.f, 0.4f, 0.4f, 1.f, 1.f, 2.f };
    BOSS_RECOVER = { ProtoBoss_IO[24], ProtoBoss_IO[25], ProtoBoss_IO[26], ProtoBoss_IO[27], ProtoBoss_IO[28], ProtoBoss_IO[29], ProtoBoss_IO[30] };

    // BOSS_STUN = { 0.f, 3.f, 0.f, 0.f, 3.f, 0.f, 0.f };
    BOSS_STUN = { ProtoBoss_IO[31], ProtoBoss_IO[32], ProtoBoss_IO[33], ProtoBoss_IO[34], ProtoBoss_IO[35], ProtoBoss_IO[36], ProtoBoss_IO[37] };

    // RocketFist attributes
    BOSS_PROJECTILE_RADIUS = ProtoBoss_IO[38];                          // BOSS PROJECTILE RADIUS = 30.f
    BOSS_PROJECTILE_DAMAGE = ProtoBoss_IO[39];                          // BOSS PROJECTILE DAMAGE = 20.f
    BOSS_PROJECTILE_SPEED = ProtoBoss_IO[40];                           // BOSS PROJECTILE SPEED = 1000.f

    // Attack hitbox settings
    SMASH_RADIUS = ProtoBoss_IO[41];                                    // BOSS SMASH RADIUS = 200.f
    SMASH_DAMAGE = ProtoBoss_IO[42];                                    // BOSS SMASH DAMAGE = 30.f
    BIG_SMASH_RADIUS = ProtoBoss_IO[43];                                // BOSS BIG SMASH RADIUS = 350.f
    SWEEP_WIDTH = ProtoBoss_IO[44];                                     // BOSS SWEEP WIDTH = 600.f
    SWEEP_HEIGHT = ProtoBoss_IO[45];                                    // BOSS SWEEP HEIGHT = 600.f

    // Explosion effects
    BOSS_DEATH_EXPLOSION_RADIUS = ProtoBoss_IO[46];                     // BOSS DEATH EXPLOSION RADIUS = 25.f

    // Boss starting point, hitbox and size
    boss_1.pos = { BOSS_SPAWN_X, BOSS_SPAWN_Y };
    boss_1.width = BOSS_WIDTH;
    boss_1.height = BOSS_HEIGHT;
    boss_1.hitbox = Init_AABB(boss_1.pos.x, boss_1.pos.y, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT);
    boss_1.hp = boss_1.max_hp = BOSS_MAXHP;
    boss_1.hp = boss_1.max_hp = BOSS_MAXHP;

    // Starting state
    boss_1.state = BOSS_IDLE;
    boss_1.attack_type = BOSS_SMASH;
    boss_1.attackstate = BOSS_ATTACK_WINDUP;

    // Timers
    boss_1.idle_timer = Create_Timer(ProtoBoss_IO[47]);                 // BOSS IDLE TIMER = 1.f
    boss_1.attack_timer = Create_Timer(ProtoBoss_IO[48]);               // BOSS ATTACK TIMER = 1.f
    boss_1.stun_timer = Create_Timer(ProtoBoss_IO[49]);                 // BOSS STUN TIMER = 3.f
    boss_1.explosion_timer = Create_Timer(ProtoBoss_IO[50]);            // BOSS EXPLOSION TIMER = 0.5f
    boss_1.death_explosion_duration = Create_Timer(ProtoBoss_IO[51]);   // BOSS DEATH EXPLOSION DURATION = 4.f

    // Boss shadow
    BOSS_SHADOW_WIDTH = ProtoBoss_IO[52];                               // BOSS SHADOW WIDTH = 120.f
    BOSS_SHADOW_HEIGHT = ProtoBoss_IO[53];                              // BOSS SHADOW HEIGHT = 80.f

    // Explosion effect check
    boss_1.explosion_triggered = 0;
 
    // Initialize melee attack properties
    for (s32 i = 0; i < ATTACK_TYPES; i++)
    {
        boss_melee[i].attack_windup = BOSS_WINDUP[i];
        boss_melee[i].attack_active = boss_melee[i].attack_windup + BOSS_ATTACK[i];
        boss_melee[i].attack_recover = boss_melee[i].attack_active + BOSS_RECOVER[i];
        boss_melee[i].timer = Create_Timer(boss_melee[i].attack_recover);
        boss_melee[i].stun_duration = BOSS_STUN[i];
        boss_melee[i].can_parry = boss_melee[i].stun_duration > 0;
    }

    Init_Boss_UI(boss_1.hp);

    SCORE_MELEE_HIT = static_cast<int>(ProtoBossScore_IO[4]);                    // + 100 points (multiplied by damage value) per frame
    SCORE_BOSS_PROJECTILES_PARRIED = static_cast<int>(ProtoBossScore_IO[5]);     // + 5000 points if boss rocket successfully parried
    SCORE_BOSS_MELEE_PARRIED = static_cast<int>(ProtoBossScore_IO[6]);           // + 10000 points if boss successfully stunned via parry
    SCORE_TIME_BONUS_CAP = static_cast<f32>(ProtoBossScore_IO[7]);               // + bonus points if player manages to beat boss within 1 minute or less
    SCORE_TIME_BONUS = static_cast<int>(ProtoBossScore_IO[8]);                   // + 2000 points for every second of time left less than the 1 minute cap
    SCORE_DODGE = static_cast<int>(ProtoBossScore_IO[9]);                        // + 2000 points if boss attack successfully dodged
    SCORE_PERFECT = static_cast<int>(ProtoBossScore_IO[10]);                     // + 10000 points if player beats boss without taking damage
}

// For damagecheck for player attacks to boss
s32 last_hit = 0;
s32 new_hit = 0;
f32 damage = 0.f;

// Update boss logic
void Update_Boss()
{
    if (boss_1.state != BOSS_DEAD)
    {
        switch (boss_1.state)
        {
        case BOSS_IDLE:
            if (!Update_Timer(&boss_1.idle_timer))
            {
                // Calculate distance between player to check if you wanna do ranged attack
                boss_1.target_dist = AEVec2Distance(&boss_1.pos, &player.pos);

                if (boss_1.target_dist > BOSS_DASH_DISTANCE) // Player is out of melee range
                {
                    f32 randomizer_ranged = AERandFloat();
                    if (randomizer_ranged > 0.5f)
                    {
                        // Do a charge
                        boss_1.attack_timer.current = 0.f;
                        boss_melee[BOSS_CHARGE].timer.current = 0.f;
                        boss_1.attack_type = BOSS_CHARGE;
                        boss_1.state = BOSS_RANGED_CHARGE;
                    }
                    else
                    {
                        // Do a rocketpunch
                        boss_1.attack_timer.current = 0.f;
                        boss_melee[BOSS_ROCKETFIST].timer.current = 0.f;
                        boss_1.attack_type = BOSS_ROCKETFIST;
                        boss_1.attackstate = BOSS_ATTACK_WINDUP;
                        boss_1.state = BOSS_RANGED_ROCKETFIST;
                    }
                }
                else // Player is in melee range
                {
                    f32 randomizer_melee = AERandFloat();
                    if (randomizer_melee > 0.5f)
                    {
                        // Do smash sweep combo
                        boss_1.attack_timer.current = 0.f;
                        boss_1.attack_type = BOSS_SMASH;
                        boss_1.state = BOSS_SMASH_SWEEP;
                    }
                    else
                    {
                        // Do three hit combo
                        boss_1.attack_timer.current = 0.f;
                        boss_1.attack_type = BOSS_LEFT_HIT;
                        boss_1.state = BOSS_THREE_HIT;
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
            if (!Update_Timer(&boss_1.stun_timer))
            {
                boss_1.state = BOSS_IDLE;
            }
            break;

        default:
            boss_1.state = BOSS_IDLE;
            break;
        }

        // Reset last hit if no new melee hit detected
        if (!new_hit)
        {
            last_hit = 0;
        }

        // Check for player melee attack collision
        if (Attacked_Melee_rect(boss_1.pos, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT, &new_hit, &damage))
        {
            if (last_hit != new_hit)
            {
                if (player.ammo < 3)
                {
                    player.ammo++;
                }
                boss_1.hp -= damage;
                last_hit = new_hit;
                Collect_Points(SCORE_MELEE_HIT * static_cast<int>(damage), HIT_SCORE);
                currentCombo += 1;
            }
        }

        // Check for player projectile collisions
        if (Attacking_Ranged_Impact(boss_1.pos, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT, &damage))
        {
            boss_1.hp -= damage;
            std::cout << "Boss hit! HP: " << boss_1.hp << '\n';
        }


        for (auto& fist : rocket_fists) {
            if (fist.active) {
                // Update rocket fist's movement
                fist.pos.x += fist.velocity.x * dt;
                fist.pos.y += fist.velocity.y * dt;
                fist.timer.current += 1.f * dt;

                // Update rocket fist's transformation
                AEMtx33 trans, scale;
                AEMtx33Trans(&trans, fist.pos.x, fist.pos.y);
                AEMtx33Scale(&scale, fist.radius, fist.radius);
                AEMtx33Concat(&fist.transform, &trans, &scale);

                // Deactivate rocket fist after it hits player or after 4 seconds (aka range)
                if (AETestCircleToCircle(&fist.pos, fist.radius, &player.pos, player.radius) || fist.timer.current >= fist.timer.duration) {
                    fist.active = false;
                    if (AETestCircleToCircle(&fist.pos, fist.radius, &player.pos, player.radius)) {
                        switch (player.state) {
                        case PARRY:
                            Parried_Hit();
                            Collect_Points(SCORE_BOSS_PROJECTILES_PARRIED, PARRY_SCORE);
                            currentCombo += 1;
                            break;
                        case BLOCK:
                            Blocked_Hit(BOSS_PROJECTILE_DAMAGE);
                            break;
                        case PARRIED:
                        case BLOCKED:
                        case DODGE:
                            Collect_Points(SCORE_DODGE, DODGE_SCORE);
                            break;
                        case HIT:
                            break;
                        default:
                            Eat_Hit(BOSS_PROJECTILE_DAMAGE);
                        }
                    }
                }
            }
        }

        // Remove inactive rocket fists
        rocket_fists.erase(std::remove_if(rocket_fists.begin(), rocket_fists.end(), [](const RocketFist& f) { return !f.active; }), rocket_fists.end());

        // Clamp HP to prevent over-reduction
        boss_1.hp = AEClamp(boss_1.hp, 0.f, BOSS_MAXHP);

        if (boss_1.hp <= 0.f)
        {
            boss_1.state = BOSS_DEAD;
            boss_1.death_explosion_duration.current = boss_1.explosion_timer.current = 0.f;

            // Adds time bonus (checks if time left is under the 1 minute cap)
            f32 timeLeft = SCORE_TIME_BONUS_CAP - levelTimer;
            if (timeLeft <= 0.f)
            {
                timeLeft = 0.f;
            }
            Collect_Points(static_cast<int>(timeLeft) * SCORE_TIME_BONUS, TIME_BONUS_SCORE);
            if (player.hp == 100.f)
            {
                Collect_Points(SCORE_PERFECT, PERFECT_SCORE);
            }
        }

        // Update Boss UI
        Update_Boss_UI(boss_1.hp);
    }
    else if (boss_1.state == BOSS_DEAD)
    {
        levelState = CLEARED;
        for (auto& fist : rocket_fists)
        {
            fist.active = false;
        }

        if (Update_Timer(&boss_1.death_explosion_duration))
        {
            Loop_Timer(&boss_1.explosion_timer);
            if (boss_1.explosion_timer.current == 0.f)
            {
                boss_1.explosion_triggered = 1;
            }
            if (boss_1.explosion_triggered == 1)
            {
                Draw_Boss_Death_Explosion(boss_1.pos);
                boss_1.explosion_triggered = 0;
            }
        }
        else
        {
            boss_death_explosion.clear();
        }

        // Remove inactive explosion effects
        boss_death_explosion.erase(std::remove_if(boss_death_explosion.begin(), boss_death_explosion.end(), [](const Boss_Explosion& e) { return !e.active; }), boss_death_explosion.end());
    }

    // Update Boss Hitbox
    Update_AABB(&boss_1.hitbox, boss_1.pos.x, boss_1.pos.y, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT);
}

bool flip = false;

void Draw_Boss_Hitboxes()
{
    switch (boss_1.state)
    {
    case BOSS_SMASH_SWEEP:
    {
        switch (boss_1.attack_type)
        {
        case BOSS_SMASH:
        {
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Draw windup animation for smash
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_SMASH].pos.x), World_To_Screen_y(boss_melee[BOSS_SMASH].pos.y), SMASH_RADIUS, boss_windup);
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_SMASH].pos.x), World_To_Screen_y(boss_melee[BOSS_SMASH].pos.y),
                    (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * SMASH_RADIUS, boss_swing);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Draw active smash hitbox
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_SMASH].pos.x), World_To_Screen_y(boss_melee[BOSS_SMASH].pos.y), SMASH_RADIUS, boss_idle);
            }
            break;
        }

        case BOSS_SWEEP:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Draw windup animation for sweep
                DrawRect(World_To_Screen_x(boss_melee[BOSS_SWEEP].pos.x), World_To_Screen_y(boss_melee[BOSS_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, boss_windup);
                DrawRect(World_To_Screen_x(boss_melee[BOSS_SWEEP].pos.x) + ((boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * SWEEP_WIDTH / 2.f - SWEEP_WIDTH / 2.f),
                    World_To_Screen_y(boss_melee[BOSS_SWEEP].pos.y), (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * SWEEP_WIDTH, SWEEP_HEIGHT, boss_swing);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Draw active sweep hitbox
                DrawRect(World_To_Screen_x(boss_melee[BOSS_SWEEP].pos.x), World_To_Screen_y(boss_melee[BOSS_SWEEP].pos.y), SWEEP_WIDTH, SWEEP_HEIGHT, boss_idle);

            }
            break;
        }
        break;
    }

    case BOSS_THREE_HIT:
    {
        switch (boss_1.attack_type)
        {
        case BOSS_LEFT_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Draw windup animation for left hit
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_LEFT_HIT].pos.y), SMASH_RADIUS, boss_windup);
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_LEFT_HIT].pos.y),
                    (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * SMASH_RADIUS, boss_swing);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Draw active left hit hitbox
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_LEFT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_LEFT_HIT].pos.y), SMASH_RADIUS, boss_idle);
            }
            break;

        case BOSS_RIGHT_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Draw windup animation for right hit
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_RIGHT_HIT].pos.y), SMASH_RADIUS, boss_windup);
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_RIGHT_HIT].pos.y),
                    (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * SMASH_RADIUS, boss_swing);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Draw active right hit hitbox
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_RIGHT_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_RIGHT_HIT].pos.y), SMASH_RADIUS, boss_idle);
            }
            break;

        case BOSS_CIRCLE_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Draw windup animation for circle hit
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_CIRCLE_HIT].pos.y), BIG_SMASH_RADIUS, boss_windup);
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_CIRCLE_HIT].pos.y),
                    (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) * BIG_SMASH_RADIUS, boss_swing);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Draw active circle hit hitbox
                DrawCircle(World_To_Screen_x(boss_melee[BOSS_CIRCLE_HIT].pos.x), World_To_Screen_y(boss_melee[BOSS_CIRCLE_HIT].pos.y), BIG_SMASH_RADIUS, boss_idle);
            }
            break;
        }
        break;
    }

    case BOSS_RANGED_CHARGE:
    {
        if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
        {
            // Draw windup animation for charge
            DrawRotatedRect(
                World_To_Screen_x(boss_1.pos.x + boss_1.target_dir.x * (boss_1.target_dist / 2.f)),
                World_To_Screen_y(boss_1.pos.y + boss_1.target_dir.y * (boss_1.target_dist / 2.f)),
                boss_1.target_dist, BOSS_HEIGHT, boss_windup, boss_1.target_rot
            );
            DrawRotatedRect(
                World_To_Screen_x(boss_1.pos.x + boss_1.target_dir.x * (boss_1.target_dist * (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) / 2.f)),
                World_To_Screen_y(boss_1.pos.y + boss_1.target_dir.y * (boss_1.target_dist * (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) / 2.f)),
                boss_1.target_dist * (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup), BOSS_HEIGHT, boss_swing, boss_1.target_rot
            );
        }
        else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
        {
            // Draw active charge hitbox
            DrawRotatedRect(
                World_To_Screen_x(boss_1.pos.x),
                World_To_Screen_y(boss_1.pos.y),
                BOSS_WIDTH, BOSS_HEIGHT, boss_idle, boss_1.target_rot
            );
        }
        break;
    }

    case BOSS_RANGED_ROCKETFIST:
    {
        if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
        {
            // Draw windup animation for rocket fist
            DrawRotatedRect(
                World_To_Screen_x(boss_1.pos.x + boss_1.target_dir.x * (boss_1.target_dist / 2.f)),
                World_To_Screen_y(boss_1.pos.y + boss_1.target_dir.y * (boss_1.target_dist / 2.f)),
                boss_1.target_dist, 10.f, boss_windup, boss_1.target_rot
            );
            DrawRotatedRect(
                World_To_Screen_x(boss_1.pos.x + boss_1.target_dir.x * (boss_1.target_dist * (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) / 2.f)),
                World_To_Screen_y(boss_1.pos.y + boss_1.target_dir.y * (boss_1.target_dist * (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup) / 2.f)),
                boss_1.target_dist * (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup), 10.f, boss_swing, boss_1.target_rot
            );
        }
        break;
    }

    default:
        break;
    }


    // Existing animation and sprite sheet drawing logic (unchanged)
    Color* color = {};
    switch (boss_1.state)
    {
    case BOSS_SMASH_SWEEP:
        color = &boss_swing;
        break;
    case BOSS_THREE_HIT:
        // TBC
        break;
    case BOSS_PARRIED:
        color = &boss_parried;
        break;
    case BOSS_RANGED_CHARGE:
        // TBC
        break;
    case BOSS_RANGED_ROCKETFIST:
        // TBC
        break;
    case BOSS_IDLE:
    default:
        color = &boss_idle;
        break;
    }



}

void Draw_Boss()
{
    DrawCircleAdvanced(World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y - 70.f), BOSS_SHADOW_WIDTH, BOSS_SHADOW_HEIGHT, SetColor(0x92000000));			// Draw shadow under
    //DrawRect(World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y), boss_1.width, boss_1.height, SetColor(0x92000000));

    f32 active_time = boss_melee[boss_1.attack_type].attack_active - boss_melee[boss_1.attack_type].attack_windup;
    f32 recover_time = boss_melee[boss_1.attack_type].attack_recover - boss_melee[boss_1.attack_type].attack_active;
    switch (boss_1.state)
    {
    case BOSS_SMASH_SWEEP:
    {
        flip = false;
        switch (boss_1.attack_type)
        {
        case BOSS_SMASH:
        {
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(2);
                f32 anim_time = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
                boss_1_sprite.Set_Col(0, 7, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(3);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_windup;
                f32 anim_time = active_curr / active_time;
                boss_1_sprite.Set_Col(0, 2, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
            {
                // Set sprite info
                std::cout << "RECOVERING";
                boss_1_sprite.Set_Row(4);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                boss_1_sprite.Set_Col(0, 3, anim_time);
            }
            break;
        }

        case BOSS_SWEEP:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(5);
                f32 anim_time = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
                boss_1_sprite.Set_Col(0, 10, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(6);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_windup;
                f32 anim_time = active_curr / active_time;
                boss_1_sprite.Set_Col(0, 2, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(7);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                boss_1_sprite.Set_Col(0, 10, anim_time);
            }
            break;
        }
        break;
    }

    case BOSS_THREE_HIT:
    {
        flip = false;
        switch (boss_1.attack_type)
        {
        case BOSS_LEFT_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(15);
                f32 anim_time = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
                boss_1_sprite.Set_Col(0, 5, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(16);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_windup;
                f32 anim_time = active_curr / active_time;
                boss_1_sprite.Set_Col(0, 2, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(17);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                boss_1_sprite.Set_Col(0, 2, anim_time);
            }
            break;

        case BOSS_RIGHT_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(18);
                f32 anim_time = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
                boss_1_sprite.Set_Col(0, 4, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(19);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_windup;
                f32 anim_time = active_curr / active_time;
                boss_1_sprite.Set_Col(0, 2, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(20);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                boss_1_sprite.Set_Col(0, 1, anim_time);
            }
            break;

        case BOSS_CIRCLE_HIT:
            if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(21);
                f32 anim_time = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
                boss_1_sprite.Set_Col(0, 5, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(22);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_windup;
                f32 anim_time = active_curr / active_time;
                boss_1_sprite.Set_Col(0, 3, anim_time);
            }
            else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
            {
                // Set sprite info
                boss_1_sprite.Set_Row(23);
                f32 active_curr = (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                boss_1_sprite.Set_Col(0, 3, anim_time);
            }
            break;
        }
        break;
    }

    case BOSS_RANGED_CHARGE:
    {
        flip = (!BossPlayerDirection(boss_1.pos, player.pos));
        if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
        {
            // Set sprite info
            boss_1_sprite.Set_Row(12);
            boss_1_sprite.Set_Col(0, 9, boss_1.attack_timer.current* boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup);
        }
        else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
        {
            // Set sprite info
            boss_1_sprite.Set_Row(13);
            boss_1_sprite.Loop(0, 1, 0.25f);
        }
        break;
    }

    case BOSS_RANGED_ROCKETFIST:
    {
        if (boss_1.attackstate == BOSS_ATTACK_WINDUP)
        {
            flip = (!BossPlayerDirection(boss_1.pos, player.pos));
            // Set sprite info
            boss_1_sprite.Set_Row(9);
            boss_1_sprite.Set_Col(0, 11, boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup);
        }
        else if (boss_1.attackstate == BOSS_ATTACK_ACTIVE)
        {
            // Set sprite info
            //boss_1_sprite.Set_Row(10);
            //boss_1_sprite.Set_Col(0);
        }
        else if (boss_1.attackstate == BOSS_ATTACK_RECOVER)
        {
            // Set sprite info
            boss_1_sprite.Set_Row(11);
            f32 active_curr = (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration) - boss_melee[boss_1.attack_type].attack_active;
            f32 anim_time = active_curr / recover_time;
            boss_1_sprite.Set_Col(0, 18, anim_time);
        }

        break;
    }

    case BOSS_PARRIED:
    {
        boss_1_sprite.Set_Row(8);
        boss_1_sprite.Loop(0, 1, 0.5f);
        break;
    }

    case BOSS_IDLE:
    {
        flip = (!BossPlayerDirection(boss_1.pos, player.pos));
        boss_1_sprite.Set_Row(0);
        boss_1_sprite.Loop(0, 3, 1.f);
        break;
    }

    case BOSS_DEAD:
    {
        boss_1_sprite.Set_Row(24);
        boss_1_sprite.Set_Col(0);
        break;
    }

    default:
        break;
    }

    // Rocket fists
    for (const auto& fist : rocket_fists)
    {
        if (fist.active)
        {
            DrawCircle(World_To_Screen_x(fist.pos.x), World_To_Screen_y(fist.pos.y), fist.radius, boss_projectile_color);
        }
    }


    // Death explosion
    for (auto& exp : boss_death_explosion)
    {
        if (exp.active)
        {
            f32 spawn_x = World_To_Screen_x(exp.pos.x) + (AERandFloat() > 0.5f ? (AERandFloat() * BOSS_HITBOX_WIDTH * 0.5f) : -(AERandFloat() * BOSS_HITBOX_WIDTH * 0.5f));
            f32 spawn_y = World_To_Screen_y(exp.pos.y) + (AERandFloat() > 0.5f ? (AERandFloat() * BOSS_HITBOX_HEIGHT * 0.5f) : -(AERandFloat() * BOSS_HITBOX_HEIGHT * 0.5f));
            DrawCircle(spawn_x, spawn_y, (AERandFloat() + 0.5f) * BOSS_DEATH_EXPLOSION_RADIUS, boss_death_explosion_color);
        }
    }

    // Draw boss sprite
    boss_1_sprite.Draw(World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y), BOSS_DRAW_WIDTH, BOSS_DRAW_HEIGHT, flip);
}

void Unload_Boss()
{
    Unload_SpriteSheet(&boss_sprite_sheet);
    boss_1_sprite.Unload();
}

void Smash_Sweep_Logic()
{
    // Update hitbox positions at the start of the logic
    boss_melee[BOSS_SMASH].pos = { boss_1.pos.x - 200.f, boss_1.pos.y - 150.f }; // Smash position
    boss_melee[BOSS_SWEEP].pos = { boss_1.pos.x, boss_1.pos.y};        // Sweep position

    if (Update_Timer(&boss_1.attack_timer))
    {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup)
        {
            // Attack is winding up
            will_hit = true;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active)
        {
            // Attack is active (check collision)
            boss_1.attackstate = BOSS_ATTACK_ACTIVE;
            switch (boss_1.attack_type)
            {
            case BOSS_SMASH:
                if (AETestCircleToCircle(&boss_melee[BOSS_SMASH].pos, SMASH_RADIUS, &player.pos, player.radius))
                {
                    // Player damage calculation
                    if (will_hit)
                    {
                        Boss_Attack_Test(boss_melee[BOSS_SMASH].can_parry, 30.f);
                    }
                }
                break;

            case BOSS_SWEEP:
                if (AETestCircleToRect(&player.pos, player.radius, &boss_melee[BOSS_SWEEP].pos, SWEEP_WIDTH, SWEEP_HEIGHT))
                {
                    // Parry checks
                    if (will_hit)
                    {
                        Boss_Attack_Test(boss_melee[BOSS_SWEEP].can_parry, 30.f);
                    }
                }
                break;

            default:
                boss_1.attackstate = BOSS_ATTACK_RECOVER;
                break;
            }
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover)
        {
            // Attack is recovering
            boss_1.attackstate = BOSS_ATTACK_RECOVER;
        }
    }
    else
    {
        if (boss_1.attack_type == BOSS_SMASH)
        {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else
        {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Three_Hit_Logic()
{
    // Update hitbox positions at the start of the logic
    boss_melee[BOSS_LEFT_HIT].pos = { boss_1.pos.x - 200.f, boss_1.pos.y - 150.f };  // Left hit position
    boss_melee[BOSS_RIGHT_HIT].pos = { boss_1.pos.x + 200.f, boss_1.pos.y - 150.f }; // Right hit position
    boss_melee[BOSS_CIRCLE_HIT].pos = { boss_1.pos.x, boss_1.pos.y };        // Circle hit position

    if (Update_Timer(&boss_1.attack_timer))
    {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup)
        {
            // Attack is winding up
            will_hit = true;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active)
        {
            // Attack is active (check collision)
            boss_1.attackstate = BOSS_ATTACK_ACTIVE;
            switch (boss_1.attack_type)
            {
            case BOSS_LEFT_HIT:
                if (AETestCircleToCircle(&boss_melee[BOSS_LEFT_HIT].pos, SMASH_RADIUS, &player.pos, player.radius))
                {
                    // Player damage calculation
                    if (will_hit)
                    {
                        Boss_Attack_Test(boss_melee[BOSS_LEFT_HIT].can_parry, 30.f);
                    }
                }
                break;

            case BOSS_RIGHT_HIT:
                if (AETestCircleToCircle(&boss_melee[BOSS_RIGHT_HIT].pos, SMASH_RADIUS, &player.pos, player.radius))
                {
                    // Player damage calculation
                    if (will_hit)
                    {
                        Boss_Attack_Test(boss_melee[BOSS_RIGHT_HIT].can_parry, 30.f);
                    }
                }
                break;

            case BOSS_CIRCLE_HIT:
                if (AETestCircleToCircle(&boss_melee[BOSS_CIRCLE_HIT].pos, BIG_SMASH_RADIUS, &player.pos, player.radius))
                {
                    // Player damage calculation
                    if (will_hit)
                    {
                        Boss_Attack_Test(boss_melee[BOSS_CIRCLE_HIT].can_parry, 30.f);
                    }
                }
                break;

            default:
                boss_1.attackstate = BOSS_ATTACK_RECOVER;
                break;
            }
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover)
        {
            // Attack is recovering
            boss_1.attackstate = BOSS_ATTACK_RECOVER;
        }
    }
    else
    {
        if (boss_1.attack_type == BOSS_LEFT_HIT || boss_1.attack_type == BOSS_RIGHT_HIT)
        {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else
        {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Boss_Dash_Logic()
{
    if (Update_Timer(&boss_1.attack_timer))
    {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup)
        {
            // Attack is winding up
            will_hit = true;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;

            // Set the target position to the player's current position
            boss_1.target_pos = player.pos;

            // Calculate the direction vector
            boss_1.target_dir = { player.pos.x - boss_1.pos.x, player.pos.y - boss_1.pos.y };
            AEVec2Normalize(&boss_1.target_dir, &boss_1.target_dir);

            // Calculate the rotation angle
            boss_1.target_rot = atan2(boss_1.target_dir.y, boss_1.target_dir.x);

            // Calculate the distance
            boss_1.target_dist = AEVec2Distance(&player.pos, &boss_1.pos);
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active)
        {
            // Attack is active (check collision)
            boss_1.attackstate = BOSS_ATTACK_ACTIVE;

            // Calculate the remaining time for the charge attack
            f32 remaining_time = boss_melee[boss_1.attack_type].timer.duration - boss_1.attack_timer.current;

            // Calculate the required speed to reach the target in the remaining time
            f32 required_speed = boss_1.target_dist * 5.f / remaining_time;

            // Move boss towards the target position using the required speed
            boss_1.pos.x += boss_1.target_dir.x * required_speed * dt;
            boss_1.pos.y += boss_1.target_dir.y * required_speed * dt;

            // Check for collision with the player
            if (AETestCircleToRect(&player.pos, player.radius, &boss_1.pos, BOSS_WIDTH, BOSS_HEIGHT))
            {
                // Parry checks
                if (will_hit)
                {
                    Boss_Attack_Test(boss_melee[BOSS_CHARGE].can_parry, 30.f);
                }
            }

            // Check if boss has reached the target position with a slight buffer
            f32 distance_to_target = AEVec2Distance(&boss_1.pos, &boss_1.target_pos);
            if (distance_to_target <= 10.f)  // Adjust this threshold if needed
            {
                // Snap the boss to the target position to ensure it stops exactly
                boss_1.pos = boss_1.target_pos;

                // Transition to the next state 
                boss_1.attack_timer.current = 0.f;
                boss_1.attack_type = BOSS_SMASH;
                boss_1.state = BOSS_SMASH_SWEEP;
            }
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover)
        {
            // Attack is recovering
            boss_1.attackstate = BOSS_ATTACK_RECOVER;
        }
    }
    else
    {
        if (boss_1.attack_type == BOSS_SMASH)
        {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else
        {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Boss_Rocket_Logic() {
    if (Update_Timer(&boss_melee[boss_1.attack_type].timer)) {
        if (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup) {
            std::cout << boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration << " < " << boss_melee[boss_1.attack_type].attack_windup << '\n';
            // Attack is winding up
            will_hit = true;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;

            // Set the target position to the player's current position
            boss_1.target_pos = player.pos;

            // Calculate the direction vector
            boss_1.target_dir = { player.pos.x - boss_1.pos.x, player.pos.y - boss_1.pos.y };
            AEVec2Normalize(&boss_1.target_dir, &boss_1.target_dir);

            // Calculate the rotation angle
            boss_1.target_rot = atan2(boss_1.target_dir.y, boss_1.target_dir.x);

            // Calculate the distance
            boss_1.target_dist = AEVec2Distance(&player.pos, &boss_1.pos);
        }
        else if (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active) {
            // Attack is active (spawn rocket fist)
            boss_1.attackstate = BOSS_ATTACK_ACTIVE;


            // Spawn a rocket fist
            RocketFist fist;
            fist.pos = boss_1.pos;
            fist.velocity = { boss_1.target_dir.x * BOSS_PROJECTILE_SPEED, boss_1.target_dir.y * BOSS_PROJECTILE_SPEED };
            fist.radius = BOSS_PROJECTILE_RADIUS;
            fist.active = true;
            fist.timer = Create_Timer(4.f);
            AEMtx33Identity(&fist.transform);
            rocket_fists.push_back(fist);

            // Immediately transition to the recovery phase
            boss_1.attackstate = BOSS_ATTACK_RECOVER;
            boss_melee[boss_1.attack_type].timer.current = boss_melee[boss_1.attack_type].attack_active / boss_melee[boss_1.attack_type].timer.duration; // Skip to recovery phase
        }
        else if (boss_melee[boss_1.attack_type].timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover) {
            // Attack is recovering
            boss_1.attackstate = BOSS_ATTACK_RECOVER;

        }
    }
    else {
        // If the attack timer is done, transition back to idle
        boss_1.state = BOSS_IDLE;
        boss_1.attack_type = 0;
        boss_1.idle_timer.current = 0.f;
    }
}


void Boss_Attack_Test(bool canparry, f32 boss_damage)
{
    will_hit = false;
    switch (player.state)
    {
    case PARRY:
        // Stun logic
        if (canparry)
        {
            boss_1.stun_timer.current = 0.f;
            boss_1.stun_timer.duration = boss_melee[boss_1.attack_type].stun_duration;
            boss_1.state = BOSS_PARRIED;
            boss_1.attack_type = 0;
            bullet_time.current = 0.f;
            Collect_Points(SCORE_BOSS_MELEE_PARRIED, PARRY_SCORE);
            currentCombo += 1;
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
        Collect_Points(SCORE_DODGE, DODGE_SCORE);
        break;
    default:
        Eat_Hit(boss_damage);
    }
}

void Draw_Boss_Death_Explosion(AEVec2& start)
{
    explosion_particle.pos = start;
    explosion_particle.active = true;
    explosion_particle.timer = Create_Timer(0.5f);
    AEMtx33Identity(&explosion_particle.transform);
    boss_death_explosion.push_back(explosion_particle);
}