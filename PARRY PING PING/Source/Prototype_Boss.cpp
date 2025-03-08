#include "Prototype_Boss.h"
#include "player.h"
#include "UI.h"
#include <vector>

Boss boss_1;

Color boss_idle, boss_windup, boss_swing, boss_stunned, boss_parried, boss_projectile_indicator, boss_projectile_color, boss_death_explosion_color;

SpriteSheet boss_sprite_sheet;

#define MAX_PROJECTILES 3
#define PROJECTILE_DAMAGE 10

const f32 BOSS_SPAWN_X = 0.f;
const f32 BOSS_SPAWN_Y = 200.f;

const f32 BOSS_WIDTH = 500.f;
const f32 BOSS_HEIGHT = 500.f;

const f32 BOSS_HITBOX_WIDTH = 400.f;
const f32 BOSS_HITBOX_HEIGHT = 300.f;

const f32 BOSS_MAXHP = 200.f;

// DASH
const f32 BOSS_DASH_DISTANCE = 300.f;

// Performs an overhead smash, followed by a wide sweep that can be parried
const s32 ATTACK_TYPES = 7;
const f32 BOSS_WINDUP[ATTACK_TYPES] = { 0.8f, 0.8f, 0.8f, 0.8f, 1.f, 2.f, 1.f };
const f32 BOSS_ATTACK[ATTACK_TYPES] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 1.5f, 1.5f };
const f32 BOSS_RECOVER[ATTACK_TYPES] = { 0.4f, 1.f, 0.4f, 0.4f, 1.f, 1.f, 1.f };

const f32 BOSS_STUN[ATTACK_TYPES] = { 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 0.f };

const f32 BOSS_PROJECTILE_RADIUS = 30.f;
const f32 BOSS_PROJECTILE_DAMAGE = 20.f;
const f32 BOSS_PROJECTILE_SPEED = 500.f;

Boss_Melee boss_melee[ATTACK_TYPES];
Boss_Range boss_projectile;
std::vector<Boss_Range> boss_projectiles;

// Boss Explosion Effects and Particles
const f32 BOSS_DEATH_EXPLOSION_RADIUS = 50.f;
Boss_Explosion explosion_particle;
std::vector<Boss_Explosion> boss_death_explosion;

std::vector<RocketFist> rocket_fists;

void Load_Boss()
{
    boss_sprite_sheet = Init_SpriteSheet("Assets/Proto_Monke/ProtoMonke.png", 5, 8, 1.f);
}

void Init_Boss_Indicator()
{
    boss_idle = SetColor(0xFFFF6961);
    boss_windup = SetColor(0xFFFFCF61);
    boss_swing = SetColor(0xFFAFFF61);
    boss_stunned = SetColor(0xFF000000);
    boss_parried = SetColor(0xFFFF00FF);
    boss_projectile_indicator = SetColor(0xFFFF0000);
    boss_projectile_color = SetColor(0xFFFF13F0);
    boss_death_explosion_color = SetColor(0xFFED6240);
}

void Init_Boss()
{
    boss_1.pos = { BOSS_SPAWN_X, BOSS_SPAWN_Y };

    boss_1.width = BOSS_WIDTH;
    boss_1.height = BOSS_HEIGHT;

    boss_1.hitbox = Init_AABB(boss_1.pos.x, boss_1.pos.y, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT);

    boss_1.hp = boss_1.max_hp = BOSS_MAXHP;

    boss_1.state = BOSS_IDLE;
    boss_1.attackstate = BOSS_ATTACK_WINDUP;

    boss_1.idle_timer = Create_Timer(1.f);
    boss_1.attack_timer = Create_Timer(1.f);
    boss_1.stun_timer = Create_Timer(3.f);

    boss_1.attack_type = BOSS_SMASH;

    boss_1.range_warning_timer = Create_Timer(1.f);
    boss_1.range_timer = Create_Timer(3.f);
    boss_1.isShootingWarningActive = 0;
    boss_1.isShooting = 0;

    boss_1.death_explosion_duration = Create_Timer(4.f);

    for (s32 i = 0; i < ATTACK_TYPES; i++)
    {
        boss_melee[i].pos = { 0.f, 0.f };
        boss_melee[i].attack_windup = BOSS_WINDUP[i];
        boss_melee[i].attack_active = boss_melee[i].attack_windup + BOSS_ATTACK[i];
        boss_melee[i].attack_recover = boss_melee[i].attack_active + BOSS_RECOVER[i];

        boss_melee[i].timer = Create_Timer(boss_melee[i].attack_recover);

        boss_melee[i].stun_duration = BOSS_STUN[i];
        if (boss_melee[i].stun_duration)
        {
            boss_melee[i].can_parry = true;
        }
        else
        {
            boss_melee[i].can_parry = false;
        }
    }

    Init_Boss_UI(boss_1.hp);
}

s32 last_hit = 0;
s32 new_hit = 0;
f32 damage = 0.f;

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
                        boss_1.attack_type = BOSS_CHARGE;
                        boss_1.state = BOSS_RANGED_CHARGE;
                    }
                    else
                    {
                        // Do a charge
                        boss_1.attack_timer.current = 0.f;
                        boss_1.attack_type = BOSS_ROCKETFIST;
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

        case BOSS_ROCKET_FIST_ACTIVE:
            // Boss is inactive while rocket fists are active
            if (rocket_fists.empty())
            {
                // No more active rocket fists, return to idle
                boss_1.state = BOSS_IDLE;
            }
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
            }
        }

        // Check for player projectile collisions
        if (Attacking_Ranged_Impact(boss_1.pos, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT, &damage))
        {
            boss_1.hp -= damage;
            std::cout << "Boss hit! HP: " << boss_1.hp << '\n';
        }

        // Update boss projectiles' behavior
        for (auto& proj : boss_projectiles)
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
                AEMtx33Scale(&scale, BOSS_PROJECTILE_RADIUS, BOSS_PROJECTILE_RADIUS);
                AEMtx33Concat(&proj.transform, &trans, &scale);

                // Deactivate projectiles after they hit player or after 4 seconds (aka range)
                if (AETestCircleToCircle(&proj.pos, BOSS_PROJECTILE_RADIUS, &player.pos, player.radius) || proj.timer.current >= proj.timer.duration)
                {
                    proj.active = false;
                    if (AETestCircleToCircle(&proj.pos, BOSS_PROJECTILE_RADIUS, &player.pos, player.radius))
                    {
                        switch (player.state)
                        {
                        case PARRY:
                            Parried_Hit();
                            break;
                        case BLOCK:
                            Blocked_Hit(BOSS_PROJECTILE_DAMAGE);
                            break;
                        case PARRIED:
                        case BLOCKED:
                        case DODGE:
                        case HIT:
                            break;
                        default:
                            Eat_Hit(BOSS_PROJECTILE_DAMAGE);
                        }
                    }
                }
            }
        }

        for (auto& fist : rocket_fists)
        {
            if (fist.active)
            {
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
                if (AETestCircleToCircle(&fist.pos, fist.radius, &player.pos, player.radius) || fist.timer.current >= fist.timer.duration)
                {
                    fist.active = false;
                    if (AETestCircleToCircle(&fist.pos, fist.radius, &player.pos, player.radius))
                    {
                        switch (player.state)
                        {
                        case PARRY:
                            Parried_Hit();
                            break;
                        case BLOCK:
                            Blocked_Hit(BOSS_PROJECTILE_DAMAGE);
                            break;
                        case PARRIED:
                        case BLOCKED:
                        case DODGE:
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

        // Remove inactive boss projectiles
        boss_projectiles.erase(std::remove_if(boss_projectiles.begin(), boss_projectiles.end(), [](const Boss_Range& p) { return !p.active; }), boss_projectiles.end());

        // Clamp HP to prevent over-reduction
        boss_1.hp = AEClamp(boss_1.hp, 0.f, BOSS_MAXHP);
        if (boss_1.hp > 0.f && boss_1.hp <= 0.6 * BOSS_MAXHP && boss_1.state != BOSS_PARRIED)
        {
            std::cout << boss_1.range_timer.current << '\n';
            if (!Update_Timer(&boss_1.range_timer))
            {
                boss_1.isShooting = 1;
                boss_1.range_timer.current = boss_1.range_warning_timer.current = 0.f;
                boss_1.isShootingWarningActive = 1;
            }

            if (boss_1.isShooting == 1)
            {
                boss_1.isShootingWarningActive = 1;
                std::cout << "Boss is shooting!!!" << '\n';
                if (!Update_Timer(&boss_1.range_warning_timer))
                {
                    boss_1.isShootingWarningActive = 0;
                    AEVec2 direction = { player.pos.x - boss_1.pos.x, player.pos.y - boss_1.pos.y };
                    AEVec2Normalize(&direction, &direction);

                    Boss_Range_Attack(boss_1.pos, direction);
                    boss_1.isShooting = 0;
                }
            }
        }
        if (boss_1.hp <= 0.f)
        {
            boss_1.state = BOSS_DEAD;
            boss_1.death_explosion_duration.current = boss_1.explosion_timer.current = 0.f;
        }

        // Update Boss UI
        Update_Boss_UI(boss_1.hp);
    }
    else if (boss_1.state == BOSS_DEAD)
    {
        levelState = CLEARED;
        for (auto& proj : boss_projectiles)
        {
            proj.active = false;
        }
        if (Update_Timer(&boss_1.death_explosion_duration))
        {
            std::cout << boss_1.death_explosion_duration.current << '\n';
            if (!Update_Timer(&boss_1.explosion_timer))
            {
                Draw_Boss_Death_Explosion(boss_1.pos);
            }
        }
    }

    // Update Boss Hitbox
    Update_AABB(&boss_1.hitbox, boss_1.pos.x, boss_1.pos.y, BOSS_HITBOX_WIDTH, BOSS_HITBOX_HEIGHT);

    std::cout << "BOSS STATES: \n0 - IDLE\n1 - ATTACK\n2 - PARRIED\nCURRENT STATE: " << boss_1.state << '\n';
}

void Draw_Boss() {
    // Default color
    Color default_color = SetColor(0xFFFFFFFF); // White as a fallback
    Color* color = &default_color; // Initialize to a valid color

    // Validate boss state
    if (boss_1.state >= 0 && boss_1.state < 8) { 
        switch (boss_1.state) {
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
            color = &boss_windup;
            break;
        case BOSS_RANGED_ROCKETFIST:
            color = &boss_windup;
            break;
        case BOSS_IDLE:
        default:
            color = &boss_idle;
            break;
        }
    }

    // Draw smash attack if active
    if (boss_1.is_smash_active) {
        DrawCircle(World_To_Screen_x(boss_1.smash_pos.x), World_To_Screen_y(boss_1.smash_pos.y), boss_1.smash_radius, *color);
    }

    // Draw sweep attack if active
    if (boss_1.is_sweep_active) {
        DrawRect(World_To_Screen_x(boss_1.sweep_pos.x), World_To_Screen_y(boss_1.sweep_pos.y), boss_1.sweep_width, boss_1.sweep_height, *color);
    }

    // Draw dash attack if active
    if (boss_1.is_dash_active && boss_1.state == BOSS_RANGED_CHARGE) {
        // Draw the base rectangle (full area of the attack)
        DrawRotatedRect(
            World_To_Screen_x(boss_1.dash_start_pos.x + boss_1.dash_dir.x * (boss_1.dash_distance / 2.f)),  // Center X (offset by half the distance)
            World_To_Screen_y(boss_1.dash_start_pos.y + boss_1.dash_dir.y * (boss_1.dash_distance / 2.f)),  // Center Y (offset by half the distance)
            boss_1.dash_distance,  // Length of the rectangle (full distance to target)
            BOSS_HEIGHT,           // Height of the rectangle
            boss_windup,           // Color for the base rectangle
            boss_1.dash_rotation   // Rotation angle
        );

        // Draw the growing rectangle (progress of the dash)
        DrawRotatedRect(
            World_To_Screen_x(boss_1.dash_start_pos.x + boss_1.dash_dir.x * (boss_1.dash_distance * boss_1.dash_progress / 2.f)),  // Center X (offset by half the progress)
            World_To_Screen_y(boss_1.dash_start_pos.y + boss_1.dash_dir.y * (boss_1.dash_distance * boss_1.dash_progress / 2.f)),  // Center Y (offset by half the progress)
            boss_1.dash_distance * boss_1.dash_progress,  // Length of the rectangle (scaled by progress)
            BOSS_HEIGHT,                                  // Height of the rectangle
            boss_swing,                                   // Color for the growing rectangle
            boss_1.dash_rotation                          // Rotation angle
        );
    }

    // Draw rocket windup if active
    if (boss_1.is_rocket_windup_active && boss_1.state == BOSS_RANGED_ROCKETFIST) {
        // Draw the base rectangle (full area of the attack)
        DrawRotatedRect(
            World_To_Screen_x(boss_1.rocket_start_pos.x + boss_1.rocket_dir.x * (boss_1.rocket_distance / 2.f)),  // Center X (offset by half the distance)
            World_To_Screen_y(boss_1.rocket_start_pos.y + boss_1.rocket_dir.y * (boss_1.rocket_distance / 2.f)),  // Center Y (offset by half the distance)
            boss_1.rocket_distance,  // Length of the rectangle (full distance to target)
            BOSS_HEIGHT,             // Height of the rectangle
            boss_windup,            // Color for the base rectangle
            boss_1.rocket_rotation  // Rotation angle
        );

        // Draw the growing rectangle (progress of the windup)
        DrawRotatedRect(
            World_To_Screen_x(boss_1.rocket_start_pos.x + boss_1.rocket_dir.x * (boss_1.rocket_distance * boss_1.rocket_progress / 2.f)),  // Center X (offset by half the progress)
            World_To_Screen_y(boss_1.rocket_start_pos.y + boss_1.rocket_dir.y * (boss_1.rocket_distance * boss_1.rocket_progress / 2.f)),  // Center Y (offset by half the progress)
            boss_1.rocket_distance * boss_1.rocket_progress,  // Length of the rectangle (scaled by progress)
            BOSS_HEIGHT,                                      // Height of the rectangle
            boss_swing,                                       // Color for the growing rectangle
            boss_1.rocket_rotation                            // Rotation angle
        );
    }

    // Draw rocket fist if active
    if (boss_1.is_rocket_fist_active) {
        DrawCircle(World_To_Screen_x(boss_1.rocket_fist_pos.x), World_To_Screen_y(boss_1.rocket_fist_pos.y), boss_1.rocket_fist_radius, *color);
    }

    // Ranged attack warning indicator
    if (boss_1.isShootingWarningActive && boss_1.state != BOSS_DEAD) {
        DrawCircle(World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y), BOSS_PROJECTILE_RADIUS, boss_projectile_indicator);
        DrawCircle(World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y), (boss_1.range_warning_timer.current / boss_1.range_warning_timer.duration) * BOSS_PROJECTILE_RADIUS, boss_projectile_color);
    }

    // Ranged attack projectiles
    for (const auto& proj : boss_projectiles) {
        if (proj.active) {
            DrawCircle(World_To_Screen_x(proj.pos.x), World_To_Screen_y(proj.pos.y), 25.f, boss_projectile_color); // Example projectile size and color
        }
    }

    // Rocket fists
    for (const auto& fist : rocket_fists) {
        if (fist.active) {
            DrawCircle(World_To_Screen_x(fist.pos.x), World_To_Screen_y(fist.pos.y), fist.radius, boss_projectile_color);
        }
    }

    // Death explosion
    for (auto& exp : boss_death_explosion) {
        if (exp.active) {
            f32 spawn_x = World_To_Screen_x(exp.pos.x) + (AERandFloat() > 0.5f ? (AERandFloat() * BOSS_HITBOX_WIDTH * 0.8f) : -(AERandFloat() * BOSS_HITBOX_WIDTH * 0.8f));
            f32 spawn_y = World_To_Screen_y(exp.pos.y) + (AERandFloat() > 0.5f ? (AERandFloat() * BOSS_HITBOX_HEIGHT * 0.8f) : -(AERandFloat() * BOSS_HITBOX_HEIGHT * 0.8f));
            DrawCircle(spawn_x, spawn_y, (AERandFloat() + 0.5f) * BOSS_DEATH_EXPLOSION_RADIUS, boss_death_explosion_color);
            if (exp.timer.current >= exp.timer.duration) {
                exp.active = false;
            }
        }
        // Remove inactive explosion particles
        boss_death_explosion.erase(std::remove_if(boss_death_explosion.begin(), boss_death_explosion.end(), [](const Boss_Explosion& p) { return !p.active; }), boss_death_explosion.end());
    }

    // Draw the boss sprite
    Draw_SpriteSheet_test(boss_sprite_sheet, World_To_Screen_x(boss_1.pos.x), World_To_Screen_y(boss_1.pos.y), boss_1.width, boss_1.height, 0.f);
}

void Unload_Boss()
{
    Unload_SpriteSheet(&boss_sprite_sheet);
}

const f32 SMASH_RADIUS = 200.f;
const f32 SMASH_DAMAGE = 30.f;
const f32 BIG_SMASH_RADIUS = 500.f;

const f32 SWEEP_WIDTH = 500.f;
const f32 SWEEP_HEIGHT = 500.f;

f32 x_offset = 0.f;

bool will_hit = true;

void Smash_Sweep_Logic() {
    if (Update_Timer(&boss_1.attack_timer)) {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup) {
            // Store the position and radius for drawing
            boss_1.smash_pos = { boss_1.pos.x + x_offset, boss_1.pos.y - 150.f };
            boss_1.smash_radius = SMASH_RADIUS;
            boss_1.is_smash_active = true;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active) {
            // Store the position and radius for drawing
            boss_1.smash_pos = { boss_1.pos.x + x_offset, boss_1.pos.y - 150.f };
            boss_1.smash_radius = SMASH_RADIUS;
            boss_1.is_smash_active = true;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover) {
            // Attack is recovering, no drawing needed
            boss_1.is_smash_active = false;
        }
    }
    else {
        if (boss_1.attack_type == BOSS_SMASH) {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Three_Hit_Logic() {
    if (Update_Timer(&boss_1.attack_timer)) {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup) {
            // Store the position and radius for drawing
            boss_1.smash_pos = { boss_1.pos.x + x_offset, boss_1.pos.y - 150.f };
            boss_1.smash_radius = SMASH_RADIUS;
            boss_1.is_smash_active = true;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active) {
            // Store the position and radius for drawing
            boss_1.smash_pos = { boss_1.pos.x + x_offset, boss_1.pos.y - 150.f };
            boss_1.smash_radius = SMASH_RADIUS;
            boss_1.is_smash_active = true;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover) {
            // Attack is recovering, no drawing needed
            boss_1.is_smash_active = false;
        }
    }
    else {
        if (boss_1.attack_type == BOSS_LEFT_HIT || boss_1.attack_type == BOSS_RIGHT_HIT) {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Boss_Dash_Logic() {
    if (Update_Timer(&boss_1.attack_timer)) {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup) {
            // Attack is winding up (store state for drawing)
            boss_1.is_dash_active = true; // Activate the dash attack
            boss_1.dash_start_pos = boss_1.pos; // Store the starting position
            boss_1.dash_target_pos = player.pos; // Store the target position

            // Calculate the direction and distance of the dash
            boss_1.dash_dir = { player.pos.x - boss_1.pos.x, player.pos.y - boss_1.pos.y };
            AEVec2Normalize(&boss_1.dash_dir, &boss_1.dash_dir);
            boss_1.dash_distance = AEVec2Distance(&boss_1.pos, &boss_1.dash_target_pos);

            // Calculate the rotation angle for drawing
            boss_1.dash_rotation = atan2(boss_1.dash_dir.y, boss_1.dash_dir.x);

            // Reset dash progress
            boss_1.dash_progress = 0.f;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active) {
            // Attack is active (move boss and check collision)
            boss_1.is_dash_active = true; // Keep the dash attack active for drawing

            // Calculate the remaining time for the dash
            f32 remaining_time = boss_melee[boss_1.attack_type].timer.duration - boss_1.attack_timer.current;

            // Calculate the required speed to reach the target in the remaining time
            f32 required_speed = boss_1.dash_distance * 5.f / remaining_time;

            // Move boss towards the target position using the required speed
            boss_1.pos.x += boss_1.dash_dir.x * required_speed * dt;
            boss_1.pos.y += boss_1.dash_dir.y * required_speed * dt;

            // Update dash progress for drawing
            boss_1.dash_progress = AEVec2Distance(&boss_1.pos, &boss_1.dash_start_pos) / boss_1.dash_distance;

            // Check for collision with the player
            if (AETestCircleToRect(&player.pos, player.radius, &boss_1.pos, BOSS_WIDTH, BOSS_HEIGHT)) {
                // Parry checks
                if (will_hit) {
                    Boss_Attack_Test(boss_melee[BOSS_CHARGE].can_parry, 30.f);
                }
            }

            // Check if boss has reached the target position with a slight buffer
            f32 distance_to_target = AEVec2Distance(&boss_1.pos, &boss_1.dash_target_pos);
            if (distance_to_target <= 10.f) { // Adjust this threshold if needed
                // Snap the boss to the target position to ensure it stops exactly
                boss_1.pos = boss_1.dash_target_pos;

                // Transition to the next state (e.g., BOSS_SMASH_SWEEP)
                boss_1.attack_timer.current = 0.f;
                boss_1.attack_type = BOSS_SMASH;
                boss_1.state = BOSS_SMASH_SWEEP;
            }
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover) {
            // Attack is recovering (update anim)
            boss_1.is_dash_active = false; // Stop drawing the dash attack
        }
    }
    else {
        if (boss_1.attack_type == BOSS_SMASH) {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
    }
}

void Boss_Rocket_Logic() {
    if (Update_Timer(&boss_1.attack_timer)) {
        if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_windup) {
            // Attack is winding up (store state for drawing)
            boss_1.is_rocket_windup_active = true; // Activate the rocket windup
            boss_1.rocket_start_pos = boss_1.pos; // Store the starting position
            boss_1.rocket_target_pos = player.pos; // Store the target position

            // Calculate the direction and distance of the rocket attack
            boss_1.rocket_dir = { player.pos.x - boss_1.pos.x, player.pos.y - boss_1.pos.y };
            AEVec2Normalize(&boss_1.rocket_dir, &boss_1.rocket_dir);
            boss_1.rocket_distance = AEVec2Distance(&boss_1.pos, &boss_1.rocket_target_pos);

            // Calculate the rotation angle for drawing
            boss_1.rocket_rotation = atan2(boss_1.rocket_dir.y, boss_1.rocket_dir.x);

            // Calculate the progress of the windup (0 to 1)
            boss_1.rocket_progress = boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration / boss_melee[boss_1.attack_type].attack_windup;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_active) {
            // Attack is active (spawn rocket fist)
            boss_1.is_rocket_windup_active = false; // Deactivate the windup

            // Spawn a rocket fist
            RocketFist fist;
            fist.pos = boss_1.pos;
            fist.velocity = { boss_1.rocket_dir.x * BOSS_PROJECTILE_SPEED, boss_1.rocket_dir.y * BOSS_PROJECTILE_SPEED };
            fist.radius = BOSS_PROJECTILE_RADIUS;
            fist.active = true;
            fist.timer = Create_Timer(4.f);
            AEMtx33Identity(&fist.transform);
            rocket_fists.push_back(fist);

            // Set the boss state to inactive while the rocket fist is active
            boss_1.state = BOSS_ROCKET_FIST_ACTIVE;

            // Transition to the next state (e.g., BOSS_SMASH_SWEEP)
            boss_1.attack_timer.current = 0.f;
            boss_1.attack_type = BOSS_SMASH;
        }
        else if (boss_1.attack_timer.current * boss_melee[boss_1.attack_type].timer.duration < boss_melee[boss_1.attack_type].attack_recover) {
            // Attack is recovering (update anim)
            boss_1.is_rocket_windup_active = false; // Stop drawing the windup
        }
    }
    else {
        if (boss_1.attack_type == BOSS_SMASH) {
            boss_1.attack_type++;
            boss_melee[boss_1.attack_type].timer.current = 0.f;
            boss_1.attack_timer = boss_melee[boss_1.attack_type].timer;
            boss_1.attackstate = BOSS_ATTACK_WINDUP;
        }
        else {
            boss_1.state = BOSS_IDLE;
            boss_1.attack_type = 0;
            boss_1.idle_timer.current = 0.f;
        }
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

void Boss_Range_Attack(const AEVec2& start, const AEVec2& direction)
{
    boss_projectile.pos = start;
    boss_projectile.velocity = { direction.x * BOSS_PROJECTILE_SPEED, direction.y * BOSS_PROJECTILE_SPEED };
    boss_projectile.active = true;
    boss_projectile.timer = Create_Timer(4.f);
    AEMtx33Identity(&boss_projectile.transform);
    boss_projectiles.push_back(boss_projectile);
}

void Draw_Boss_Death_Explosion(AEVec2& start)
{
    explosion_particle.pos = start;
    explosion_particle.active = true;
    explosion_particle.timer = Create_Timer(0.5f);
    AEMtx33Identity(&explosion_particle.transform);
    boss_death_explosion.push_back(explosion_particle);
}