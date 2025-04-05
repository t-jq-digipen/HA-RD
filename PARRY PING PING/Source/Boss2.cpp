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

Color red2      = SetColor(0x508B0000);
Color red3      = SetColor(0x808B0000);
Color blue2     = SetColor(0xFF0000FF);
Color black2    = SetColor(0xFF000000);

// Sprite sheets
SpriteSheet boss_2_sprite;
SpriteSheet lightningwave_sprite;
SpriteSheet lightningball_sprite;

// Boss spawn position
f32 BOSS2_SPAWN_X;
f32 BOSS2_SPAWN_Y;

// Boss size
f32 BOSS2_WIDTH;
f32 BOSS2_HEIGHT;

// Boss draw size
f32 BOSS2_DRAW_WIDTH;
f32 BOSS2_DRAW_HEIGHT;

// Boss shadow
f32 BOSS2_SHADOW_WIDTH;
f32 BOSS2_SHADOW_HEIGHT;

// Boss hitbox size
f32 BOSS2_HITBOX_WIDTH;
f32 BOSS2_HITBOX_HEIGHT;

// Boss attributes
f32 BOSS2_MAXHP_P1;
f32 BOSS2_MAXHP_P2;
f32 BOSS2_VEL;

// Attack timings
const s32 ATTACK_TYPES = 6;
std::vector<f32> BOSS2_WINDUP;
std::vector<f32> BOSS2_ATTACK;
std::vector<f32> BOSS2_RECOVER;
std::vector<f32> BOSS2_STUN;

// Attack information
Boss2_Attack boss_attack[ATTACK_TYPES];


// Attack hitbox settings
f32 OFFSET_RANGE_X;     // Half of player width
f32 OFFSET_RANGE_Y;     // Half of player height

f32 SLASHWIDTH;
f32 SLASHHEIGHT;

f32 DASH_OFFSET;

// Missiles settings
f32 BOSS2_MISSILE_PREP_SPEED;
f32 BOSS2_MISSILE_HOMING_SPEED;

f32 MISSILE_DASH_OFFSET_X;
f32 MISSILE_DASH_OFFSET_Y;

// Slash setting
f32 SLASH_DAMAGE;

// Lightning Wave settings
f32 LIGHTNING_STAB_DAMAGE;
f32 LIGHTNING_STAB_RADIUS;
f32 LIGHTNING_WAVE_DAMAGE;
f32 LIGHTNING_WAVE_SPEED;
f32 LIGHTNING_WAVE_DURATION;
LightningWave lightningwave;
bool lightning_wave_damaged = false;   // Whether the player has been damaged by the wave
bool lightning_stab_damaged = false;
f32 LIGHTNING_WAVE_MAX_RADIUS;



// Miscellaneous boss settings
bool will_hit2 = true;
std::vector<Missile> missiles;  // Vector to store active missiles
bool prepdone = false;
bool rocketset = false;
bool prepdash = false;
bool dashed = false;
bool dashright = false;
bool dashleft = false;

int P2dash = 0;

// Level score update values
int BOSS2_SCORE_PLAYER_MELEE_HIT; // + 100 points (multiplied by damage value) per frame
int BOSS2_SCORE_BOSS_PROJECTILES_PARRIED; // + 5000 points if boss rocket successfully parried
int BOSS2_SCORE_BOSS_MELEE_PARRIED; // + 10000 points if boss successfully stunned via parry
f32 BOSS2_SCORE_TIME_BONUS_CAP; // + bonus points if player manages to beat boss within 1 minute or less
int BOSS2_SCORE_TIME_BONUS; // + 2000 points for every second of time left less than the 1 minute cap
int BOSS2_SCORE_DODGE; // + 2000 points if boss attack successfully dodged
int BOSS2_SCORE_PERFECT; // + 10000 points if player beats boss without taking damage
int BOSS2_SCORE_MULTIPLIER; // x2 points in Level 2

// File IO
std::vector<float> Boss2_IO;
std::vector<float> Boss2Score_IO;

// Load boss assets
void Load_Boss2() 
{
    // Read File IO
    ReadFromFile("Assets/FileIO/Boss2.txt", Boss2_IO);

    // Boss spawn position
    BOSS2_SPAWN_X = Boss2_IO[0];                        // BOSS 2 SPAWN X = 0.f
    BOSS2_SPAWN_Y = Boss2_IO[1];                        // BOSS 2 SPAWN Y = 200.f

    // Boss size
    BOSS2_WIDTH  = Boss2_IO[2];                         // BOSS 2 WIDTH = 100.f
    BOSS2_HEIGHT = Boss2_IO[3];                         // BOSS 2 HEIGHT = 100.f

    // Boss draw size
    BOSS2_DRAW_WIDTH    = Boss2_IO[4];                  // BOSS 2 DRAW WIDTH = 650.f
    BOSS2_DRAW_HEIGHT   = Boss2_IO[5];                  // BOSS 2 DRAW HEIGHT = 650.f

    // Boss hitbox size
    BOSS2_HITBOX_WIDTH  = Boss2_IO[6];                  // BOSS 2 HITBOX WIDTH = 200.f
    BOSS2_HITBOX_HEIGHT = Boss2_IO[7];                  // BOSS 2 HITBOX HEIGHT = 380.f

    // Boss attributes
    BOSS2_MAXHP_P1 = Boss2_IO[8];                       // BOSS 2 MAX HP = 150.f
    BOSS2_MAXHP_P2 = Boss2_IO[9];
    BOSS2_VEL      = Boss2_IO[10];                      // BOSS 2 VELOCITY = 10.f

    // Attack timings
    BOSS2_WINDUP  = { Boss2_IO[11], Boss2_IO[12], Boss2_IO[13], Boss2_IO[14], Boss2_IO[15], Boss2_IO[16] };   // { 1.f, 0.2f, 0.5f, 0.5f, 0.5f, 0.8f }
    BOSS2_ATTACK  = { Boss2_IO[17], Boss2_IO[18], Boss2_IO[19], Boss2_IO[20], Boss2_IO[21], Boss2_IO[22] };   // { 0.1f, 0.3f, 0.1f, 0.1f, 0.1f, 1.f }
    BOSS2_RECOVER = { Boss2_IO[23], Boss2_IO[24], Boss2_IO[25], Boss2_IO[26], Boss2_IO[27], Boss2_IO[28] };   // { 2.f, 0.05f, 0.4f, 1.5f, 0.3f, 2.f }
    BOSS2_STUN    = { Boss2_IO[29], Boss2_IO[30], Boss2_IO[31], Boss2_IO[32], Boss2_IO[33], Boss2_IO[34] };   // { 0.f, 0.f, 3.f, 3.f, 3.f, 0.f }

    // Attack information

    // Attack hitbox settings
    OFFSET_RANGE_X = Boss2_IO[35] / 2;                  // Half of player width (125.f)
    OFFSET_RANGE_Y = Boss2_IO[36] / 2;                  // Half of player height (125.f)

    SLASHWIDTH = Boss2_IO[37];                          // BOSS 2 SLASH WIDTH = 250.f
    SLASHHEIGHT = Boss2_IO[38];                         // BOSS 2 SLASH HEIGHT = 250.f

    DASH_OFFSET = Boss2_IO[39];                         // BOSS 2 DASH OFFSET = 100.f

    // Missiles settings
    BOSS2_MISSILE_PREP_SPEED   = Boss2_IO[40];          // BOSS 2 MISSILE PREP SPEED = 500.f
    BOSS2_MISSILE_HOMING_SPEED = Boss2_IO[41];          // BOSS 2 MISSILE HOMING SPEED = 800.f
    MISSILE_DASH_OFFSET_X      = Boss2_IO[55];
    MISSILE_DASH_OFFSET_Y      = Boss2_IO[56];

    // Slash setting
    SLASH_DAMAGE = Boss2_IO[42];                        // BOSS 2 SLASH DAMAGE = 30.f

    // Lightning Wave settings
    LIGHTNING_STAB_DAMAGE       = Boss2_IO[43];         // BOSS 2 LIGHTNING STAB DAMAGE = 30.f
    LIGHTNING_STAB_RADIUS       = Boss2_IO[44];         // BOSS 2 LIGHTNING STAB RADIUS = 100.f
    LIGHTNING_WAVE_DAMAGE       = Boss2_IO[45];         // BOSS 2 LIGHTNING WAVE DAMAGE = 30.f
    LIGHTNING_WAVE_SPEED        = Boss2_IO[46];         // BOSS 2 LIGHTNING WAVE SPEED = 10.f
    LIGHTNING_WAVE_MAX_RADIUS   = Boss2_IO[47];         // BOSS 2 LIGHTNING WAVE MAX RADIUS = 800.f
    LIGHTNING_WAVE_DURATION     = Boss2_IO[48];         // BOSS 2 LIGHTNING WAVE DURATION = 4.f

    lightningwave.timer = Create_Timer(LIGHTNING_WAVE_DURATION);

    // Timers
    boss_2.idle_timer   = Create_Timer(Boss2_IO[49]);   // BOSS 2 IDLE TIMER = 1.f
    boss_2.attack_timer = Create_Timer(Boss2_IO[50]);   // BOSS 2 ATTACK TIMER = 3.f
    boss_2.stun_timer   = Create_Timer(Boss2_IO[51]);   // BOSS 2 STUN TIMER = 3.f
    boss_2.death_timer  = Create_Timer(Boss2_IO[52]);   // BOSS 2 DEATH TIMER = 5.f

    // Shadow
    BOSS2_SHADOW_WIDTH = Boss2_IO[53];
    BOSS2_SHADOW_HEIGHT = Boss2_IO[54];

    Boss2_IO.clear();


    ReadFromFile("Assets/FileIO/PlayerScore.txt", Boss2Score_IO);

    // Score system
    BOSS2_SCORE_MULTIPLIER = static_cast<int>(Boss2Score_IO[11]);

    // Scores are doubled in Level 2
    BOSS2_SCORE_PLAYER_MELEE_HIT         = static_cast<int>(Boss2Score_IO[4]) * BOSS2_SCORE_MULTIPLIER;     // + 200 points (multiplied by damage value) per frame
    BOSS2_SCORE_BOSS_PROJECTILES_PARRIED = static_cast<int>(Boss2Score_IO[5]) * BOSS2_SCORE_MULTIPLIER;     // + 10000 points if boss rocket successfully parried
    BOSS2_SCORE_BOSS_MELEE_PARRIED       = static_cast<int>(Boss2Score_IO[6]) * BOSS2_SCORE_MULTIPLIER;     // + 20000 points if boss successfully stunned via parry
    BOSS2_SCORE_TIME_BONUS_CAP           = static_cast<f32>(Boss2Score_IO[12]);                             // + bonus points if player manages to beat boss within 1 minute or less
    BOSS2_SCORE_TIME_BONUS               = static_cast<int>(Boss2Score_IO[8]);                              // + 2000 points for every second of time left less than the 1 minute cap
    BOSS2_SCORE_DODGE                    = static_cast<int>(Boss2Score_IO[9]) * BOSS2_SCORE_MULTIPLIER;     // + 4000 points if boss attack successfully dodged
    BOSS2_SCORE_PERFECT                  = static_cast<int>(Boss2Score_IO[10]) * BOSS2_SCORE_MULTIPLIER;    // + 20000 points if player beats boss without taking damage

    Boss2Score_IO.clear();


    // Sprites
    boss_2_sprite        = SpriteSheet("Assets/Bosses/Boss_2.png", 30, 22);
    lightningwave_sprite = SpriteSheet("Assets/Bosses/lightningwave.png", 4, 1);
    lightningball_sprite = SpriteSheet("Assets/Bosses/Lightningball.png", 10, 1);
    Load_Parry_Spark();
}

// Init boss2 indicators
void Init_Boss2_Indicator() 
{
    Init_Parry_Spark();
}

void Init_Boss2() 
{
    std::cout << "INIT BOSS2\n---------------\n";

    // Reset missiles
    missiles.clear();  // Clear any existing
    missiles.reserve(8); // Reserve space (but doesn't construct)

    lightningwave.timer.current = 1.f;

    // Boss starting point, hitbox and size
    boss_2.pos = { BOSS2_SPAWN_X, BOSS2_SPAWN_Y };
    boss_2.width = BOSS2_WIDTH;
    boss_2.height = BOSS2_HEIGHT;
    boss_2.hitbox = Init_AABB(boss_2.pos.x, boss_2.pos.y, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT);
    boss_2.hp = boss_2.max_hp = BOSS2_MAXHP_P1;

    // Starting state & phase
    boss_2.idle_timer.current = 0.f;
    boss_2.phase        = PHASE1;
    boss_2.state        = BOSS2_IDLE;
    boss_2.attackstate  = BOSS2_ATTACK_WINDUP;

    P2dash = 0;


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

    if (boss_2.state != BOSS2_DEAD && boss_2.state != BOSS2_PHASEBREAK)
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
                    boss_2.idle_timer.current = 0.f;
                }
                break;
            case BOSS2_IDLE:
                if (!Update_Timer(&boss_2.idle_timer))
                {
                    if (player.state != DEAD)
                    {
                        // Set randomizer
                        f32 randomizer = AERandFloat();

                        if (randomizer <= .3f) // 40% probability for Missile Barrage
                        {
                            // Do Missile Barrage
                            boss_2.attack_timer.current = 0.f;
                            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                            boss_2.missile_prep.current = 0.f;
                            boss_2.attack_type = MISSILE_BARRAGE;
                            boss_2.state = BOSS2_MISSILE_BARRAGE;
                            rocketset = false;
                        }
                        else if (randomizer <= .7f) // 40% probability for Dash & Slash
                        {
                            // Do Dash & Slash
                            boss_2.attack_timer.current = 0.f;
                            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                            boss_2.attack_type = DASH;
                            boss_2.state = BOSS2_DASH_SLASH;
                            dashed = false;
                            prepdash = false;

                        }
                        else // 20% probability for Lightning Wave
                        {
                            // Do Lightning Wave
                            boss_2.attack_timer.current = 0.f;
                            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                            boss_attack[LIGHTNING_STAB].timer.current = 0.f; // Reset stab timer
                            boss_attack[LIGHTNING_WAVE].timer.current = 0.f;
                            boss_2.attack_type = LIGHTNING_STAB;
                            boss_2.state = BOSS2_LIGHTNING_WAVE;
                        }

                    }
                    else // if player DEAD
                    {
                        boss_2.idle_timer.current = 0.f;
                    }
                }
                break;

            case BOSS2_DASH_SLASH:
                Dash_Slash_Logic();
                break;

            case BOSS2_MISSILE_BARRAGE:
                Missile_Barrage_Logic();
                break;

            case BOSS2_LIGHTNING_WAVE:
                Lightning_Wave_Logic();
                break;

            default:
                boss_2.state = BOSS2_IDLE;
                break;
            }

            break;
        case(PHASE2):
            switch (boss_2.state)
            {
            case BOSS2_PARRIED:
                if (!Update_Timer(&boss_2.stun_timer))
                {
                    boss_2.state = BOSS2_IDLE;
                    boss_2.idle_timer.current = 0.f;
                }
                break;
            case BOSS2_IDLE:
                if (!Update_Timer(&boss_2.idle_timer))
                {
                    // Set randomizer
                    f32 randomizer = AERandFloat();

                    if (randomizer <= .4f) // 40% probability for Missile Barrage
                    {
                        // Do Missile Barrage
                        boss_2.attack_timer.current = 0.f;
                        boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                        boss_attack[MISSILE_BARRAGE].timer.current = 0.f;
                        boss_2.missile_prep.current = 0.f;
                        boss_2.attack_type = MISSILE_BARRAGE;
                        boss_2.state = BOSS2_MISSILE_BARRAGE;
                        rocketset = false;
                        P2dash = 0;
                    }
                    else
                    {
                        // Do Dash & Slash
                        boss_2.attack_timer.current = 0.f;
                        boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                        boss_2.attack_type = DASH;
                        boss_2.state = BOSS2_DASH_SLASH;
                        dashed = false;
                        prepdash = false;
                        P2dash = 0;
                    }
                }
                break;

            case BOSS2_DASH_SLASH:
                Dash_Slash_Logic();
                break;

            case BOSS2_MISSILE_BARRAGE:
                Missile_Barrage_Logic();
                break;

            default:
                boss_2.state = BOSS2_IDLE;
                break;
            }
            break;
        default:
            break;
        }

        Update_Missiles();

        // Remove inactive missiles
        /*missiles.erase(std::remove_if(missiles.begin(), missiles.end(),
            [](const Missile& m) { return !m.active; }), missiles.end());*/

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
                Collect_Points(BOSS2_SCORE_PLAYER_MELEE_HIT * static_cast<int>(damage2), HIT_SCORE);
                currentCombo += 1;
            }
        }

        // Check for player projectile collisions
        if (Attacking_Ranged_Impact(boss_2.pos, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT, &damage2))
        {
            boss_2.hp -= damage2;
            std::cout << "Boss hit! HP: " << boss_2.hp << '\n';
        }


        // Clamp HP to prevent over-reduction
        boss_2.hp = AEClamp(boss_2.hp, 0.f, BOSS2_MAXHP_P1);

        if (boss_2.hp <= 0.f && (boss_2.state != BOSS2_DEAD && boss_2.state != BOSS2_PHASEBREAK))
        {
            boss_2.state = (boss_2.phase == PHASE2) ? BOSS2_DEAD : BOSS2_PHASEBREAK;

            boss_2.death_timer.current = 0.f;
            boss_2.attack_timer.current = 1.f;
            End_Spark();

            missiles.clear();
            lightningwave.timer.current = 1.f;

            if (boss_2.state == BOSS2_DEAD)
            {
                // Adds time bonus (checks if time left is under the 1 minute cap)
                f32 timeLeft = BOSS2_SCORE_TIME_BONUS_CAP - levelTimer;
                if (timeLeft <= 0.f)
                {
                    timeLeft = 0.f;
                }

                Collect_Points(static_cast<int>(timeLeft) * BOSS2_SCORE_TIME_BONUS, TIME_BONUS_SCORE);
                if (player.hp == 100.f)
                {
                    Collect_Points(BOSS2_SCORE_PERFECT, PERFECT_SCORE);
                }
            }
            
        }

        // Cheat code to drop boss to 0hp
        if (AEInputCheckTriggered(AEVK_N))
        {
            boss_2.hp = 0.f;
        }

        if (boss_2.state != BOSS2_IDLE && boss_2.attackstate == BOSS2_ATTACK_WINDUP && boss_attack[boss_2.attack_type].can_parry)
        {
            Start_Spark(boss_attack[boss_2.attack_type].attack_windup);
            Set_Spark_Loc(boss_2.pos.x, boss_2.pos.y + 50.f);
        }

        // Update Boss UI
        Update_Boss_UI(boss_2.hp);
    }
    else if (boss_2.state == BOSS2_PHASEBREAK)
    {
        if (!Update_Timer(&boss_2.death_timer))
        {
            boss_2.state = BOSS2_IDLE;
            boss_2.idle_timer.current = 0.f;
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
            boss_2.phase = PHASE2;
        }

        boss_2.hp = boss_2.max_hp = BOSS2_MAXHP_P2;
    }
    else    // boss_2.state == BOSS2_DEAD
    {
        levelState = CLEARED;
        std::cout << "DEDTIME: " << boss_2.death_timer.current << " | DURATION: " << boss_2.death_timer.duration << '\n';
        Update_Timer(&boss_2.death_timer);
    }

    Update_Lightning_Wave();


    // Update Boss Hitbox
    Update_AABB(&boss_2.hitbox, boss_2.pos.x, boss_2.pos.y, BOSS2_HITBOX_WIDTH, BOSS2_HITBOX_HEIGHT);
}

void Draw_Boss2_Hitboxes() 
{

    f32 timer_progress = boss_2.attack_timer.current * boss_attack[boss_2.attack_type].timer.duration;

    // Draw missiles if target locked
    for (const auto& missile : missiles)
    {
        if (missile.locked)
        {
            DrawCircle(World_To_Screen_x(missile.targetpos.x), World_To_Screen_y(missile.targetpos.y), missile.targetradius, red2);
        }
    }

    switch (boss_2.state) 
    {
    case (BOSS2_DASH_SLASH):
    {
        switch (boss_2.attack_type)
        {
        case (DASH_SLASH1):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for slash
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_dir.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHWIDTH / 2.f),
                    World_To_Screen_y(boss_2.target_dir.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, red3);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slash hitbox
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red3);
            }
            break;
        case (DASH_SLASH2):
            if (boss_2.attackstate == BOSS2_ATTACK_WINDUP)
            {
                // Draw windup animation for slash
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red2);
                DrawRect(World_To_Screen_x(boss_2.target_dir.x) + ((timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH / 2.f - SLASHWIDTH / 2.f),
                    World_To_Screen_y(boss_2.target_dir.y), (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * SLASHWIDTH, SLASHHEIGHT, red3);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slash hitbox
                DrawRect(World_To_Screen_x(boss_2.target_dir.x), World_To_Screen_y(boss_2.target_dir.y), SLASHWIDTH, SLASHHEIGHT, red3);
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
                DrawCircle(World_To_Screen_x(lightningwave.pos.x), World_To_Screen_y(lightningwave.pos.y), LIGHTNING_STAB_RADIUS, red2);
                DrawCircle(World_To_Screen_x(lightningwave.pos.x), World_To_Screen_y(lightningwave.pos.y),
                    (timer_progress / boss_attack[boss_2.attack_type].attack_windup) * LIGHTNING_STAB_RADIUS, red3);
            }
            else if (boss_2.attackstate == BOSS2_ATTACK_ACTIVE)
            {
                // Draw active slam hitbox
                DrawCircle(World_To_Screen_x(lightningwave.pos.x), World_To_Screen_y(lightningwave.pos.y), LIGHTNING_STAB_RADIUS, red3);

            }
            break;
        }
        break;
    }
    default:
        break;
    }

    if (lightningwave.radius)
    {
        //DrawCircle(World_To_Screen_x(lightningwave.pos.x), World_To_Screen_y(lightningwave.pos.y), lightningwave.radius, blue2);

        lightningwave_sprite.Set_Row(0);
        lightningwave_sprite.Loop(0, 3, .5f);
        lightningwave_sprite.Draw(World_To_Screen_x(lightningwave.pos.x), World_To_Screen_y(lightningwave.pos.y), lightningwave.radius * 2.f, lightningwave.radius * 2.f);

    }
}

bool flip_sprite = false;

void Draw_Boss2()
{
    DrawCircleAdvanced(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y - 150.f), BOSS2_SHADOW_WIDTH, BOSS2_SHADOW_HEIGHT, SetColor(0x92000000));			// Draw shadow under

    // Draw missiles first (so they appear behind the boss)
    for (const auto& missile : missiles)
    {
        if (missile.active)
        {
            lightningball_sprite.Set_Row(0);
            lightningball_sprite.Loop(0, 9, 1.f);
            lightningball_sprite.Draw(World_To_Screen_x(missile.pos.x), World_To_Screen_y(missile.pos.y), missile.width, missile.height);
        }
    }


    f32 windup_time = BOSS2_WINDUP[boss_2.attack_type];
    f32 active_time = boss_attack[boss_2.attack_type].attack_active - boss_attack[boss_2.attack_type].attack_windup;
    f32 recover_time = boss_attack[boss_2.attack_type].attack_recover - boss_attack[boss_2.attack_type].attack_active;

    // PHASE CHECK
    switch (boss_2.phase)
    {
    case PHASE1:
    case PHASE2:
    {
        switch (boss_2.state)
        {
        case BOSS2_PARRIED:
        {
            boss_2_sprite.Set_Row(1);
            boss_2_sprite.Set_Col(0, 13, boss_2.stun_timer.current);
            break;
        }   // BOSS2_PARRIED
            // ==============================================================================

        case BOSS2_MISSILE_BARRAGE:
        {
            switch (boss_2.attack_type)
            {
            case DASH:
            {
                flip_sprite = (boss_2.target_pos.x > boss_2.pos.x);
                boss_2_sprite.Set_Row(21);
                f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - BOSS2_WINDUP[boss_2.attack_type];
                f32 anim_time = active_curr / active_time;
                boss_2_sprite.Set_Col(0, 7, anim_time);
            }
            break;  // DASH

            case MISSILE_BARRAGE:
            {
                flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
                switch (boss_2.attackstate)
                {
                case BOSS2_ATTACK_WINDUP:
                {
                    boss_2_sprite.Set_Row(2);
                    f32 anim_time = boss_2.attack_timer.current * boss_2.attack_timer.duration / boss_attack[boss_2.attack_type].attack_windup;

                    std::cout << "ANIMTIME " << anim_time << "\n";
                    boss_2_sprite.Set_Col(0, 2, anim_time);
                    break;
                }   // WINDUP SLASH 1
                case BOSS2_ATTACK_ACTIVE:
                {
                    boss_2_sprite.Set_Row(3);
                    boss_2_sprite.Set_Col(0);
                    break;
                }   // ACTIVE SLASH 1
                case BOSS2_ATTACK_RECOVER:
                {
                    boss_2_sprite.Set_Row(4);
                    f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - boss_attack[boss_2.attack_type].attack_active;
                    f32 anim_time = active_curr / recover_time;
                    std::cout << "ANIMTIME " << anim_time << "\n";
                    boss_2_sprite.Set_Col(0, 7, anim_time);
                    break;
                }   // RECOVER SLASH 1
                }   // switch attack state
                break;
            }
            break;  // MISSILE_BARRAGE

            default:
                break;
            }

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
                flip_sprite = (boss_2.target_dir.x > boss_2.pos.x);
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
                flip_sprite = (boss_2.target_dir.x > boss_2.pos.x);
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
                    boss_2_sprite.Set_Col(0, 1, anim_time);
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
            switch (boss_2.attack_type)
            {
            case LIGHTNING_STAB:
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
            }   // LIGHTNING STAB
                // -------------------------------------------------
            case LIGHTNING_WAVE:
            {
                switch (boss_2.attackstate)
                {
                case BOSS2_ATTACK_WINDUP:
                {
                    boss_2_sprite.Set_Row(15);
                    f32 anim_time = boss_2.attack_timer.current * boss_2.attack_timer.duration / windup_time;
                    boss_2_sprite.Set_Col(0, 12, anim_time);
                    break;
                }   // WINDUP WAVE
                case BOSS2_ATTACK_ACTIVE:
                {
                    boss_2_sprite.Set_Row(16);
                    boss_2_sprite.Set_Col(0);
                    break;
                }   // ACTIVE WAVE
                case BOSS2_ATTACK_RECOVER:
                {
                    boss_2_sprite.Set_Row(17);
                    f32 active_curr = boss_2.attack_timer.current * boss_2.attack_timer.duration - BOSS2_ATTACK[boss_2.attack_type];
                    f32 anim_time = active_curr / recover_time;
                    boss_2_sprite.Set_Col(0, 14, anim_time);
                    break;
                }   // RECOVER WAVE
                }   // switch attack state
                break;
            }   // LIGHTNING WAVE
                // -------------------------------------------------
            }   // switch attack type
                // WAVE
                // -------------------------------------------------
            flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
            break;
        }   // BOSS2_LIGHTNING_WAVE
            // ==============================================================================

        case BOSS2_PHASEBREAK:
        {
            boss_2_sprite.Set_Row(19);
            boss_2_sprite.Set_Col(0, 29, boss_2.death_timer.current);
            break;
        }   // BOSS2_PHASEBREAK
            // ==============================================================================

        case BOSS2_DEAD:
        {
            boss_2_sprite.Set_Row(18);
            boss_2_sprite.Set_Col(0, 14, boss_2.death_timer.current);
            break;
        }   // BOSS2_DEAD
            // ==============================================================================

        case BOSS2_IDLE:
        default:
        {
            flip_sprite = (!BossPlayerDirection(boss_2.pos, player.pos));
            boss_2_sprite.Set_Row(0);
            boss_2_sprite.Set_Col(0, 12, boss_2.idle_timer.current);
            break;
        }   // BOSS2_IDLE
            // ==============================================================================

        }   // switch boss_2.state
    }
    break;  // end PHASE1 ===================================================================

    }   // switch boss_2.phase


    
    boss_2_sprite.Draw(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS2_DRAW_WIDTH, BOSS2_DRAW_HEIGHT, flip_sprite);
    // Lightning fx if phase 2
    if (boss_2.phase == PHASE2)
    {
        boss_2_sprite.Set_Row(20);
        boss_2_sprite.Loop(0, 16, 1.f);
        boss_2_sprite.Draw(World_To_Screen_x(boss_2.pos.x), World_To_Screen_y(boss_2.pos.y), BOSS2_DRAW_WIDTH, BOSS2_DRAW_HEIGHT, flip_sprite);
    }

    Draw_Spark();

}

void Unload_Boss2()
{
    boss_2_sprite.Unload();
    lightningwave_sprite.Unload();
    lightningball_sprite.Unload();
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
            boss_2.attack_type = MISSILE_BARRAGE;
            boss_2.attack_timer.current = 1.f;

            bullet_time.current = 0.f;
            std::cout << "Boss parried. State: " << boss_2.state << '\n'; // Debug log
        }
        
        Parried_Hit();
        Collect_Points(BOSS2_SCORE_BOSS_MELEE_PARRIED, PARRY_SCORE);
        currentCombo += 1;
        break;
    case BLOCK:
        Blocked_Hit(boss_damage);
        break;
    case PARRIED:
    case BLOCKED:
    case HIT:
    case DODGE:
        // Do nothing, immune
        Collect_Points(BOSS2_SCORE_DODGE, DODGE_SCORE);
        break;
    default:
        Eat_Hit(boss_damage);
    }
}

void Dash_Slash_Logic()
{

    // Set the attack timer duration to match current attack type
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
                        Boss2_Attack_Test(boss_attack[boss_2.attack_type].can_parry, SLASH_DAMAGE);
                    }
                }
            }
            // Slash Recovery
            else if (timer_progress < boss_attack[boss_2.attack_type].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
                if (boss_2.phase == PHASE2 && P2dash < 2)
                {
                    boss_2.attack_timer.current = 1.f;
                }
            }
        }
        else // Timer finished
        {
                
            // Transition to second slash or back to idle
            if (boss_2.attack_type == DASH_SLASH1) {

                // If phase 2, check if already dashed twice
                if (boss_2.phase == PHASE2 && P2dash < 2)
                {
                    // Transition back to dash
                    boss_2.attack_timer.current = 0.f;
                    boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                    boss_2.attack_type = DASH;
                    dashed = false;
                    prepdash = false;
                    dashright = false;
                    dashleft = false;
                    P2dash++;
                }
                else
                {
                    // Slash 2
                    boss_2.attack_type = DASH_SLASH2;
                    boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                    boss_2.attack_timer.current = 0.f;
                    will_hit2 = true;

                    // For second slash, use same side as first slash
                    // (target_dir already set correctly from first slash)
                }
            }
            else {
                boss_2.state = BOSS2_IDLE;
                prepdash = false;
                dashed = false;
                dashright = false;
                dashleft = false;
                boss_2.idle_timer.current = 0.f;
                P2dash = 0;
            }
        }
    }
}

void Update_Missiles()
{
    // Update active missiles every frame
    for (Missile& missile : missiles) {

        // Velocity updates
        if (missile.timer.current < .25f) 
        {
            AEVec2Normalize(&missile.velocity, &missile.velocity);
            f32 decel_factor = (0.25f - missile.timer.current) / .25f;
            AEVec2Scale(&missile.velocity, &missile.velocity, BOSS2_MISSILE_PREP_SPEED * decel_factor);
        }

        // Set homing targets for each active missile
        if (missile.timer.current > .25f && !missile.locked)
        {
            missile.targetpos = GetRandomTargetPosAroundPlayer();
            AEVec2Sub(&missile.velocity, &missile.targetpos, &missile.pos);
            AEVec2Normalize(&missile.velocity, &missile.velocity);
            AEVec2Scale(&missile.velocity, &missile.velocity, BOSS2_MISSILE_HOMING_SPEED);
            missile.locked = true;
        }

        // Position, movement, and damage checks
        
        // Update position based on velocity
        missile.pos.x += missile.velocity.x * dt;
        missile.pos.y += missile.velocity.y * dt;
        Update_Timer(&missile.timer);

        // Check if missile reached target position
        f32 distanceToTarget = AEVec2Distance(&missile.pos, &missile.targetpos);
        if (distanceToTarget < 10.f)    // Small threshold for "reached target"
        { 
            // Check for player collision
            if (AETestCircleToCircle(&missile.targetpos, missile.targetradius, &player.pos, player.radius)) {
                Boss_Attack_Test(boss_attack[MISSILE_BARRAGE].can_parry, 30.f);
                missile.active = false;
                continue;
            }
            missile.active = false;
            continue; // Skip other checks since we deactivated it
        }

        // Deactivate if missile goes too far or times out
        if (AEVec2Distance(&missile.pos, &boss_2.pos) > 1000.f || missile.timer.current >= 1.0f) {
            missile.active = false;
        }
        
    }

    missiles.erase(
        std::remove_if(missiles.begin(), missiles.end(), [](const Missile& missile) {return !missile.active; }),
        missiles.end()
    );
}


void Missile_Barrage_Logic()
{
    // Set the attack timer duration to match current attack type
    boss_2.attack_timer.duration = boss_attack[boss_2.attack_type].timer.duration;

    if (Update_Timer(&boss_2.attack_timer))
    {
        f32 timer_progress = boss_2.attack_timer.current * boss_2.attack_timer.duration;


        switch (boss_2.attack_type)
        {
        case MISSILE_BARRAGE:
        {
            // Windup Phase (missile preparation)
            if (timer_progress < boss_attack[MISSILE_BARRAGE].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                will_hit2 = true;
            }
            // Active Phase (missile homing)
            else if (timer_progress < boss_attack[MISSILE_BARRAGE].attack_active)
            {
                boss_2.attackstate = BOSS2_ATTACK_ACTIVE;

                // Only initialize missiles once at the end of windup
                if (!rocketset)
                {
                    for (size_t i = 0; i < 4; ++i) { // Create 4 new missiles
                        Missile newMissile;

                        newMissile.pos = boss_2.pos;

                        // Offset missile spawns, 2 from left pack two from right pack
                        if (i < 2)
                        {
                            newMissile.pos.x -= 50.f;
                        }
                        else
                        {
                            newMissile.pos.x += 50.f;
                        }
                        newMissile.pos.y += 150.f;

                        newMissile.active = true;
                        newMissile.locked = false;
                        newMissile.width = 150.f;
                        newMissile.height = 150.f;
                        newMissile.targetradius = 150.f;
                        newMissile.timer = Create_Timer(4.0f);
                        newMissile.timer.current = 0.f;

                        // Set initial spread directions
                        switch (i)
                        {
                        case 0: newMissile.velocity = { -0.5f, 0.7f }; break;
                        case 1: newMissile.velocity = { -0.2f, 1.f }; break;
                        case 2: newMissile.velocity = { 0.2f, 1.f }; break;
                        case 3: newMissile.velocity = { 0.5f, 0.7f }; break;
                        }

                        AEVec2Normalize(&newMissile.velocity, &newMissile.velocity);
                        AEVec2Scale(&newMissile.velocity, &newMissile.velocity, BOSS2_MISSILE_PREP_SPEED);

                        missiles.push_back(newMissile); // Add to vector
                    }
                    rocketset = true;
                }

                boss_2.attack_timer.current = boss_attack[MISSILE_BARRAGE].attack_active / boss_attack[MISSILE_BARRAGE].timer.duration;
            }
            // Recover Phase
            else if (timer_progress < boss_attack[MISSILE_BARRAGE].attack_recover)
            {
                boss_2.attackstate = BOSS2_ATTACK_RECOVER;
            }
        }
        break;

        case DASH:
        {
            // Dash Windup
            if (timer_progress < boss_attack[DASH].attack_windup)
            {
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;

                if (!prepdash)
                {
                    // Store initial position in target_dir (reusing this vector)
                    boss_2.target_dir = boss_2.pos;

                    // Setup dash destination in target_pos
                    if (AERandFloat() < 0.5f) {
                        boss_2.target_pos = { boss_2.pos.x + MISSILE_DASH_OFFSET_X, player.pos.y + MISSILE_DASH_OFFSET_Y };
                        dashright = true;
                    }
                    else {
                        boss_2.target_pos = { boss_2.pos.x - MISSILE_DASH_OFFSET_X, player.pos.y + MISSILE_DASH_OFFSET_Y};
                        dashleft = true;
                    }

                    //boss_2.target_pos.x = AEClamp(boss_2.target_pos.x,0,1);
                    //boss_2.target_pos.y = AEClamp(boss_2.target_pos.y,0,1);
                    // I NEED HELP FOR MAP CLAMPS
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

                // Transition to missile attack
                boss_2.attack_type = MISSILE_BARRAGE;
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                boss_2.attack_timer.current = 0.f;
                rocketset = false;
            }
        }
        break;

        default:
            break;
        }
        
    }
    else
    {
        if (boss_2.phase == PHASE2)
        {
            if (P2dash < 2)
            {
                // Transition back to dash
                boss_2.attack_timer.current = 0.f;
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                boss_2.attack_type = DASH;
                dashed = false;
                prepdash = false;
                dashright = false;
                dashleft = false;
                P2dash++;
            }
            else
            {
                // Transition to dash and double slash
                boss_2.attack_timer.current = 0.f;
                boss_2.attackstate = BOSS2_ATTACK_WINDUP;
                boss_2.attack_type = DASH;
                boss_2.state = BOSS2_DASH_SLASH;
                dashed = false;
                prepdash = false;
                dashright = false;
                dashleft = false;
            }
        }
        else
        {
            // return to idle
            boss_2.state = BOSS2_IDLE;
            boss_2.idle_timer.current = 0.f;
            rocketset = false;
            P2dash = 0;
        }
        
    }
}

void Lightning_Wave_Logic()
{
    // Set the attack timer duration to match current attack type
    boss_2.attack_timer.duration = boss_attack[boss_2.attack_type].timer.duration;
    lightningwave.pos = boss_2.pos;
    lightningwave.pos.y -= 150.f;

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
                if (AETestCircleToCircle(&lightningwave.pos, LIGHTNING_STAB_RADIUS, &player.pos, player.radius))
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
            boss_2.attackstate = BOSS2_ATTACK_WINDUP;
            boss_2.attack_timer.current = 0.f;
            boss_2.attack_timer.duration = boss_attack[LIGHTNING_WAVE].timer.duration;
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

                // Start the wave
                lightningwave.timer.current = 0.f;

                boss_2.attack_timer.current = boss_attack[LIGHTNING_WAVE].attack_active / boss_attack[LIGHTNING_WAVE].timer.duration;

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
        }
        break;
    }
    }
}

void Update_Lightning_Wave()
{
    if (Update_Timer(&lightningwave.timer))
    {
        lightningwave.radius += LIGHTNING_WAVE_SPEED;

        if (AETestCircleToCircle(&lightningwave.pos, lightningwave.radius, &player.pos, player.radius) &&
            !lightning_wave_damaged)
        {
            Boss2_Attack_Test(false, LIGHTNING_WAVE_DAMAGE);
            lightning_wave_damaged = true;
        }
    }
    else
    {
        lightningwave.radius = 0.f;
        lightning_stab_damaged = false;
        lightning_wave_damaged = false;
        lightningwave.radius = 0.f;
    }

}


AEVec2 GetRandomTargetPosAroundPlayer() 
{
    f32 offsetX = AERandFloat() * OFFSET_RANGE_X * 2 - OFFSET_RANGE_X; // Random X offset
    f32 offsetY = AERandFloat() * OFFSET_RANGE_Y * 2 - OFFSET_RANGE_Y; // Random Y offset
    return { player.pos.x + offsetX, player.pos.y + offsetY };
}