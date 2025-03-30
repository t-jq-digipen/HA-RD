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
#include "SpriteSheet.h"
#include "Enemies.h"

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
Color black2 = SetColor(0xFF000000);

// Sprite sheets
SpriteSheet boss_2_sprite;

// Boss spawn position
const f32 BOSS2_SPAWN_X = 0.f;
const f32 BOSS2_SPAWN_Y = 200.f;

// Boss size
const f32 BOSS2_WIDTH = 100.f;
const f32 BOSS2_HEIGHT = 100.f;

// Boss draw size
const f32 BOSS_DRAW_WIDTH = 650.f;
const f32 BOSS_DRAW_HEIGHT = 650.f;

// Boss hitbox size
const f32 BOSS2_HITBOX_WIDTH = 200.f;
const f32 BOSS2_HITBOX_HEIGHT = 150.f;

// Boss attributes
const f32 BOSS2_MAXHP = 150.f;
const f32 BOSS2_VEL = 10.f;

// Attack timings
const s32 ATTACK_TYPES = {6};
const f32 BOSS2_WINDUP[ATTACK_TYPES]    = { 2.f , .2f, 0.5f, 0.5f ,0.5f ,0.8f};
const f32 BOSS2_ATTACK[ATTACK_TYPES]    = { 0.5f,0.3f,  .1f,  .1f ,0.1f  ,2.f};
const f32 BOSS2_RECOVER[ATTACK_TYPES]   = { 2.f ,.05f,  .4f, 1.5f ,0.3f  ,2.f};
const f32 BOSS2_STUN[ATTACK_TYPES]      = { 0.f , 0.f, 3.f ,  3.f ,0.f   ,0.f};

// Attack information
Boss2_Attack boss_attack[ATTACK_TYPES];


// Attack hitbox settings
const f32 OFFSET_RANGE_X = 250.f / 2;  // Half of player width
const f32 OFFSET_RANGE_Y = 250.f / 2; // Half of player height

const f32 SLASHWIDTH = 250.f;
const f32 SLASHHEIGHT = 250.f;

const f32 DASH_OFFSET = 100.f;

// Missiles settings
const f32 BOSS2_MISSILE_PREP_SPEED = 20.f;
const f32 BOSS2_MISSILE_HOMING_SPEED = 40.f;

// Lightning Wave settings
const f32 LIGHTNING_STAB_DAMAGE = 30.f;
const f32 LIGHTNING_STAB_RADIUS = 200.f;
const f32 LIGHTNING_WAVE_DAMAGE = 30.f;
const f32 LIGHTNING_WAVE_SPEED = 50.f;
LightningWave lightningwave;
bool lightning_wave_damaged = false;   // Whether the player has been damaged by the wave
bool lightning_stab_damaged = false;
#define LIGHTNING_WAVE_MAX_RADIUS 800.f



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
    boss_2_sprite = SpriteSheet("Assets/Bosses/Boss_2.png", 15, 18);
    Load_Parry_Spark();
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
    boss_2.phasebreak_timer = Create_Timer(5.f);

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
            case BOSS2_PARRIED:
                if (!Update_Timer(&boss_2.stun_timer))
                {
                    boss_2.state = BOSS2_IDLE;
                }
                break;
            case BOSS_IDLE:
                if (!Update_Timer(&boss_2.idle_timer))
                {
                    // Set randomizer
                    f32 randomizer = AERandFloat();

                    if (randomizer <= 0.f) // 40% probability for Missile Barrage
                    {
                        // Do Missile Barrage
                        boss_2.attack_timer.current = 0.f;
                        boss_attack[MISSILE_BARRAGE].timer.current = 0.f;
                        boss_2.missile_prep.current = 0.f;
                        boss_2.attack_type = MISSILE_BARRAGE;
                        boss_2.state = BOSS2_MISSILE_BARRAGE;
                     
                    }
                    else if (randomizer <= 0.f) // 40% probability for Dash & Slash
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
                        boss_2.attack_timer.current = 0.f;
                        boss_attack[LIGHTNING_STAB].timer.current = 0.f; // Reset stab timer
                        boss_attack[LIGHTNING_WAVE].timer.current = 0.f;
                        boss_2.attack_type = LIGHTNING_STAB;
                        boss_2.state = BOSS2_LIGHTNING_WAVE;
                        
                    }
                }
                break;

            case BOSS2_DASH_SLASH:
                Dash_Slash_Logic();
                break;

            case BOSS2_MISSILE_BARRAGE:
                Missile_Barrage_Logic();
                std::cout << "BOSS_2_MISSILE: " << boss_2.attack_timer.current << "------------\n";
                break;

            case BOSS2_LIGHTNING_WAVE:
                Lightning_Wave_Logic(); 
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

        // Update active missiles every frame
        for (auto& missile : missiles) {
            if (missile.active) {
                // Update position based on velocity
                missile.pos.x += missile.velocity.x * dt;
                missile.pos.y += missile.velocity.y * dt;
                missile.timer.current += dt;

                // Check for player collision
                if (AETestCircleToCircle(&missile.pos, missile.radius, &player.pos, player.radius)) {
                    if (will_hit2) {
                        Boss_Attack_Test(boss_attack[MISSILE_BARRAGE].can_parry, 30.f);
                    }
                    missile.active = false;
                }

                // Deactivate if missile goes too far or times out
                if (AEVec2Distance(&missile.pos, &boss_2.pos) > 1000.f || missile.timer.current >= 4.0f) {
                    missile.active = false;
                }
            }
        }

        // Remove inactive missiles
        missiles.erase(std::remove_if(missiles.begin(), missiles.end(),
            [](const Missile& m) { return !m.active; }), missiles.end());

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

        if (boss_2.hp <= 0.f && boss_2.phase == PHASE1)
        {
            boss_2.phase = PHASE2;
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
    {
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
        if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
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
    }
    case (BOSS2_DASH_SLASH):
    {
        switch (boss_2.attack_type)
        {
        case (DASH_SLASH1):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for slash
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_dir.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHHEIGHT / 2.f),
                    World_To_Screen_y(boss_2.target_dir.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, green2);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slash hitbox
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, blue2);

            }
            break;
        case (DASH_SLASH2):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for slash
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_dir.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHHEIGHT / 2.f),
                    World_To_Screen_y(boss_2.target_dir.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, green2);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slash hitbox
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, blue2);
            }
            break;
        }
        break;
    }
    case (BOSS2_LIGHTNING_WAVE):
    {
        switch (boss_2.attack_type)
        {
        case (LIGHTNING_STAB):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for slam
                DrawCircle(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), LIGHTNING_STAB_RADIUS, red2);
                DrawCircle(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y),
                    (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * LIGHTNING_STAB_RADIUS, green2);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slam hitbox
                DrawCircle(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), LIGHTNING_STAB_RADIUS, red2);

            }
            break;
        case (LIGHTNING_WAVE):
            {
                DrawCircle(World_To_Screen_x(lightningwave.pos.x),World_To_Screen_y(lightningwave.pos.y),lightningwave.radius,blue2);
            }
            break;
        }
        break;
    }
    default:
        break;
    }
}

bool flip_sprite = false;

void Draw_Boss2()
{
    // Draw missiles first
    for (const auto& missile : missiles) {
        if (missile.active) {
            DrawCircle(World_To_Screen_x(missile.pos.x), World_To_Screen_y(missile.pos.y),
                missile.radius, green2);
        }
    }

    if (boss_2.state != BOSS2_PARRIED)
    {
        DrawRect(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS2_WIDTH, BOSS2_HEIGHT, red2);
    }
    else DrawRect(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS2_WIDTH, BOSS2_HEIGHT, black2);

    f32 windup_time = BOSS2_WINDUP[boss_2.attack_type];
    f32 active_time = boss_attack[boss_2.attack_type].attack_active - boss_attack[boss_2.attack_type].attack_windup;
    f32 recover_time = boss_attack[boss_2.attack_type].attack_recover - boss_attack[boss_2.attack_type].attack_active;

    switch (boss_2.state)
    {
    case BOSS2_IDLE:
    {
        flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
        boss_2_sprite.Set_Row(0);
        boss_2_sprite.Loop(0,12,1.f);
        break;
    }   // BOSS2_IDLE
        // ==============================================================================

    case BOSS2_PARRIED:
    {
        boss_2_sprite.Set_Row(1);
        boss_2_sprite.Set_Col(0, 13, boss_2.stun_timer.current);
        break;
    }   // BOSS2_PARRIED
        // ==============================================================================

    case BOSS2_MISSILE_BARRAGE:
    {
        flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
        break;
    }   // BOSS2_MISSILE_BARRAGE
        // ==============================================================================

    case BOSS2_DASH_SLASH:
    {
        // Check if boss is dashing or slashing
        switch (boss_2.attack_type)
        {
        case DASH:
        {
            flip_sprite = (boss_2.target_pos.x > boss_2.pos.x);
            boss_2_sprite.Set_Row(5);
            f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - BOSS2_WINDUP[boss_2.attack_type];
            f32 anim_time = active_curr / active_time;
            boss_2_sprite.Set_Col(0, 7, anim_time);
            break;
        }   // DASH
            // -------------------------------------------------
        case DASH_SLASH1:
        {
            switch (boss_2.attackstate)
            {
            case BOSS2_ATTACK_WINDUP:
            {
                boss_2_sprite.Set_Row(6);
                f32 anim_time = boss_2.attack_timer.current * boss_2.attack_timer.duration / boss_attack[boss_2.attack_type].attack_windup;

                std::cout << "ANIMTIME " << anim_time << "\n";
                boss_2_sprite.Set_Col(0, 5, anim_time);
                break;
            }   // WINDUP SLASH 1
            case BOSS2_ATTACK_ACTIVE:
            {
                boss_2_sprite.Set_Row(7);
                boss_2_sprite.Set_Col(0);
                break;
            }   // ACTIVE SLASH 1
            case BOSS2_ATTACK_RECOVER:
            {
                boss_2_sprite.Set_Row(8);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - boss_attack[boss_2.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                std::cout << "ANIMTIME " << anim_time << "\n";
                boss_2_sprite.Set_Col(0, 2, anim_time);
                break;
            }   // RECOVER SLASH 1
            }   // switch attack state
            break;
        }   // SLASH 1
            // -------------------------------------------------

        case DASH_SLASH2:
        {
            switch (boss_2.attackstate)
            {
            case BOSS2_ATTACK_WINDUP:
            {
                boss_2_sprite.Set_Row(9);
                f32 anim_time = boss_2.attack_timer.current * boss_2.attack_timer.duration / windup_time;
                std::cout << "ANIMTIME " << anim_time << "\n";
                boss_2_sprite.Set_Col(0, 6, anim_time);
                break;
            }   // WINDUP SLASH 1
            case BOSS2_ATTACK_ACTIVE:
            {
                boss_2_sprite.Set_Row(10);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - windup_time;
                f32 anim_time = active_curr / active_time;
                std::cout << "ANIMTIME " << anim_time << "\n";
                boss_2_sprite.Set_Col(0,1,anim_time);
                break;
            }   // ACTIVE SLASH 1
            case BOSS2_ATTACK_RECOVER:
            {
                boss_2_sprite.Set_Row(11);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - boss_attack[boss_2.attack_type].attack_active;
                f32 anim_time = active_curr / recover_time;
                std::cout << "ANIMTIME " << anim_time << "\n";
                boss_2_sprite.Set_Col(0, 8, anim_time);
                break;
            }   // RECOVER SLASH 1
            }   // switch attack state
            break;
        }   // SLASH 2
            // -------------------------------------------------
        }   // switch attack type

        break;
    }   // BOSS2_DASH_SLASH
        // ==============================================================================

    case BOSS2_LIGHTNING_WAVE:
    {
        {
            switch (boss_2.attackstate)
            {
            case BOSS2_ATTACK_WINDUP:
            {
                boss_2_sprite.Set_Row(12);
                f32 anim_time = boss_2.attack_timer.current * boss_2.attack_timer.duration / windup_time;
                boss_2_sprite.Set_Col(0, 9, anim_time);
                break;
            }   // WINDUP WAVE
            case BOSS2_ATTACK_ACTIVE:
            {
                boss_2_sprite.Set_Row(13);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - windup_time;
                f32 anim_time = active_curr / active_time;
                boss_2_sprite.Set_Col(0, 1, anim_time);
                break;
            }   // ACTIVE WAVE
            case BOSS2_ATTACK_RECOVER:
            {
                boss_2_sprite.Set_Row(14);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - BOSS2_ATTACK[boss_2.attack_type];
                f32 anim_time = active_curr / recover_time;
                boss_2_sprite.Set_Col(0, 2, anim_time);
                break;
            }   // RECOVER WAVE
            }   // switch attack state
            break;
        }   // WAVE
            // -------------------------------------------------
        flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
        break;
    }   // BOSS2_LIGHTNING_WAVE
        // ==============================================================================

    }   // switch boss_2.state
    boss_2_sprite.Draw(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS_DRAW_WIDTH, BOSS_DRAW_HEIGHT, flip_sprite);
    Draw_Spark();
}

void Unload_Boss2()
{
    boss_2_sprite.Unload();
    Unload_Parry_Spark();
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
            std::cout << "Boss parried. State: " << boss_2.state << '\n'; // Debug log
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
    boss_2.attack_timer.duration = boss_attack[boss_2.attack_type].timer.duration;
    // Dash Phase
    if (!dashed)
    {
        if (Update_Timer(&boss_2.attack_timer))
        {
            f32 timer_progress = boss_2.attack_timer.current * boss_attack[DASH].timer.duration;

            // Dash Windup
            if (timer_progress < boss_attack[DASH].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;

                if (!prepdash)
                {
                    std::cout << "DASH WINDUP\n";
                    // Store initial position in target_dir (reusing this vector)
                    boss_2.target_dir = boss_2.pos;

                    // Setup dash destination in target_pos
                    if (AERandFloat() < 0.5f) {
                        boss_2.target_pos = { player.pos.x + DASH_OFFSET, player.pos.y };
                        dashright = true;
                    }
                    else {
                        boss_2.target_pos = { player.pos.x - DASH_OFFSET, player.pos.y };
                        dashleft = true;
                    }
                    prepdash = true;
                }
            }
            // Dash Active (0.5s duration)
            else if (timer_progress < boss_attack[DASH].attack_active)
            {
                boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

                // Calculate progress through active phase (0 to 1)
                f32 dash_active_progress =
                    (timer_progress - boss_attack[DASH].attack_windup) /
                    (boss_attack[DASH].attack_active - boss_attack[DASH].attack_windup);

                // Move boss along dash path
                boss_2.pos.x = boss_2.target_dir.x +
                    (boss_2.target_pos.x - boss_2.target_dir.x) * dash_active_progress;
                boss_2.pos.y = boss_2.target_dir.y +
                    (boss_2.target_pos.y - boss_2.target_dir.y) * dash_active_progress;
            }
            // Dash Recovery
            else if (timer_progress < boss_attack[DASH].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
                dashed = true;

                // Snap to exact target position
                boss_2.pos = boss_2.target_pos;

                // Setup slash position (opposite side of dash direction)
                if (dashleft) {
                    // If dashed left, put slash hitbox to the right
                    boss_2.target_dir = { boss_2.pos.x + DASH_OFFSET, boss_2.pos.y };
                }
                else if (dashright) {
                    // If dashed right, put slash hitbox to the left
                    boss_2.target_dir = { boss_2.pos.x - DASH_OFFSET, boss_2.pos.y };
                }

                // Transition to slash attack
                boss_2.attack_type = DASH_SLASH1;
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                boss_2.attack_timer.current = 0.f;
            }
        }
    }
    // Slash Phase
    else
    {
        if (Update_Timer(&boss_2.attack_timer))
        {
            f32 timer_progress = boss_2.attack_timer.current * boss_2.attack_timer.duration;
                //boss_attack[boss_2.attack_type].timer.duration;
            std::string state;
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                state = "WINDUP ";
            }
            if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                state = "ACTIVE ";
            }
            if (boss_2.attackstate == BOSS2_ATTACK_RECOVER)
            {
                state = "RECOVER ";
            }
            std::cout << state << "TIMERPROG " << timer_progress << "\n";
            // Slash Windup
            if (timer_progress < boss_attack[boss_2.attack_type].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                will_hit2 = true;
            }
            // Slash Active
            else if (timer_progress < boss_attack[boss_2.attack_type].attack_active)
            {
                boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

                // Check slash collision (using target_dir as slash position)
                if (AETestCircleToRect(&player.pos, player.radius,
                    &boss_2.target_dir, SLASHWIDTH, SLASHHEIGHT))
                {
                    if (will_hit2) {
                        Boss_Attack_Test(boss_attack[boss_2.attack_type].can_parry, 30.f);
                    }
                }
            }
            // Slash Recovery
            else if (timer_progress < boss_attack[boss_2.attack_type].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
            }
        }
        else
        {
            // Transition to second slash or back to idle
            if (boss_2.attack_type == DASH_SLASH1) {
                boss_2.attack_type = DASH_SLASH2;
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                boss_2.attack_timer.current = 0.f;
                will_hit2 = true;

                // For second slash, use same side as first slash
                // (target_dir already set correctly from first slash)
            }
            else {
                boss_2.state = BOSS2_IDLE;
                prepdash = false;
                dashed = false;
                dashright = false;
                dashleft = false;
                boss_2.idle_timer.current = 0.f;
            }
        }
    }
}

void Missile_Barrage_Logic() {
    if (Update_Timer(&boss_2.attack_timer)) {
        f32 timer_progress = boss_2.attack_timer.current * boss_attack[boss_2.attack_type].timer.duration;

        // Windup Phase (2 seconds)
        if (timer_progress < boss_attack[boss_2.attack_type].attack_windup) {
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
            will_hit2 = true;

            // Only initialize missiles once at the start of windup
            if (!rocketset) {
                std::cout << "INITIALIZING MISSILES\n";
                for (size_t i = 0; i < missiles.size(); ++i) {
                    missiles[i].pos = boss_2.pos;
                    missiles[i].active = true;
                    missiles[i].radius = 50.f;
                    missiles[i].timer = Create_Timer(4.0f);

                    // Set initial spread directions
                    switch (i) {
                    case 0: missiles[i].velocity = { -1.f, 0.f }; break;
                    case 1: missiles[i].velocity = { -0.707f, 0.707f }; break;
                    case 2: missiles[i].velocity = { 0.707f, 0.707f }; break;
                    case 3: missiles[i].velocity = { 1.f, 0.f }; break;
                    }

                    AEVec2Normalize(&missiles[i].velocity, &missiles[i].velocity);
                    missiles[i].velocity.x *= BOSS2_MISSILE_PREP_SPEED;
                    missiles[i].velocity.y *= BOSS2_MISSILE_PREP_SPEED;
                }
                rocketset = true;
            }
        }
        // Active Phase (very brief - just to set homing)
        else if (timer_progress < boss_attack[boss_2.attack_type].attack_active) {
            boss_2.attackstate = BOSS2_ATTACK_ACTIVE;
            std::cout << "SETTING HOMING TARGETS\n";

            // Set homing targets for each missile
            for (size_t i = 0; i < missiles.size(); ++i) {
                if (missiles[i].active) {
                    missiles[i].targetpos = GetRandomTargetPosAroundPlayer();
                    missiles[i].velocity = {
                        missiles[i].targetpos.x - missiles[i].pos.x,
                        missiles[i].targetpos.y - missiles[i].pos.y
                    };
                    AEVec2Normalize(&missiles[i].velocity, &missiles[i].velocity);
                    missiles[i].velocity.x *= BOSS2_MISSILE_HOMING_SPEED;
                    missiles[i].velocity.y *= BOSS2_MISSILE_HOMING_SPEED;
                }
            }

            // Immediately transition to recovery
            boss_2.attackstate = BOSS2_ATTACK_RECOVER;
            boss_2.attack_timer.current = boss_attack[boss_2.attack_type].attack_recover /
                boss_attack[boss_2.attack_type].timer.duration;
        }
        // Recovery Phase
        else if (timer_progress < boss_attack[boss_2.attack_type].attack_recover) {
            boss_2.attackstate = BOSS2_ATTACK_RECOVER;
        }
    }
    else {
        // Only end attack when all missiles are gone
        bool anyMissilesActive = false;
        for (const auto& missile : missiles) {
            if (missile.active) {
                anyMissilesActive = true;
                break;
            }
        }

        if (!anyMissilesActive) {
            boss_2.state = BOSS2_IDLE;
            boss_2.idle_timer.current = 0.f;
            missiles.clear();
            missiles.resize(4);
            rocketset = false;
        }
        else {
            // Keep attack going until missiles are done
            boss_2.attack_timer.current = boss_attack[boss_2.attack_type].attack_recover /
                boss_attack[boss_2.attack_type].timer.duration;
        }
    }
}

void Lightning_Wave_Logic()
{
    // Set the attack timer duration to match current attack type
    boss_2.attack_timer.duration = boss_attack[boss_2.attack_type].timer.duration;

    switch (boss_2.attack_type)
    {
    case LIGHTNING_STAB:
    {
        if (Update_Timer(&boss_2.attack_timer))
        {
            f32 timer_progress = boss_2.attack_timer.current * boss_2.attack_timer.duration;

            // Windup Phase
            if (timer_progress < boss_attack[LIGHTNING_STAB].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                will_hit2 = true;
                lightning_stab_damaged = false;
            }
            // Active Phase
            else if (timer_progress < boss_attack[LIGHTNING_STAB].attack_active)
            {
                boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

                // Damage check
                if (AETestCircleToCircle(&boss_2.pos, LIGHTNING_STAB_RADIUS, &player.pos, player.radius))
                {
                    if (will_hit2 && !lightning_stab_damaged)
                    {
                        Boss2_Attack_Test(boss_attack[LIGHTNING_STAB].can_parry, LIGHTNING_STAB_DAMAGE);
                        lightning_stab_damaged = true;
                    }
                }
            }
            // Recover Phase
            else if (timer_progress < boss_attack[LIGHTNING_STAB].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
            }

        }
        else
        {
            // Transition to wave phase
            boss_2.attack_type = LIGHTNING_WAVE;
            boss_2.attack_timer.current = 0.f;
            boss_2.attack_timer.duration = boss_attack[LIGHTNING_WAVE].timer.duration;
            lightningwave.pos = boss_2.pos;
            lightningwave.radius = 0.f;
            lightning_wave_damaged = false;
        }
        break;

    }
    case LIGHTNING_WAVE:
    {
        std::cout << "Timer Progress: " << boss_2.attack_timer.current << " / " << boss_2.attack_timer.duration << "\n";
        if (Update_Timer(&boss_2.attack_timer))
        {
            f32 timer_progress = boss_2.attack_timer.current * boss_2.attack_timer.duration;

            // Windup Phase
            if (timer_progress < boss_attack[LIGHTNING_WAVE].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
            }
            // Active Phase
            else if (timer_progress < boss_attack[LIGHTNING_WAVE].attack_active)
            {
                boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

                // Expand the wave
                lightningwave.radius += LIGHTNING_WAVE_SPEED;

                // Damage check
                if (AETestCircleToCircle(&lightningwave.pos, lightningwave.radius, &player.pos, player.radius) &&
                    !lightning_wave_damaged)
                {
                    Boss2_Attack_Test(false, LIGHTNING_WAVE_DAMAGE);
                    lightning_wave_damaged = true;
                }
            }
            // Recover Phase
            else if (timer_progress < boss_attack[LIGHTNING_WAVE].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
            }
        }
        else
        {
            // Attack complete
            boss_2.state = BOSS2_IDLE;
            boss_2.idle_timer.current = 0.f;
            lightningwave.radius = 0.f;
            lightning_stab_damaged = false;
            lightning_wave_damaged = false;
        }
        break;
    }
    }
}

AEVec2 GetRandomTargetPosAroundPlayer() 
{
    f32 offsetX = AERandFloat() * OFFSET_RANGE_X * 2 - OFFSET_RANGE_X; // Random X offset
    f32 offsetY = AERandFloat() * OFFSET_RANGE_Y * 2 - OFFSET_RANGE_Y; // Random Y offset
    return { player.pos.x + offsetX, player.pos.y + offsetY };
}