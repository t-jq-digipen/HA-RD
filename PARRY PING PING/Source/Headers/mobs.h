#pragma once
#include "utils.h"


#define array_count(x) sizeof(x)/sizeof(*x)

typedef struct mobs_type {
	f32 x, y, size, attackrange;
	bool isattacking;
	Color color;
}mobs_type;

typedef struct mobs_list_type {
	mobs_type mobs[100];
	u32 count;
}mobs_list_type;

static void add_mobs(mobs_list_type* list, f32 x, f32 y, f32 size, Color color);
static void clear_mobs(mobs_list_type* list);
static void init_mobs(mobs_list_type* list);
static mobs_type* find_mob_by_index(mobs_list_type* list, int index);
static int get_current_mobs(mobs_list_type* list);
static void Update_mobs(mobs_list_type* list, Color color);
//bool mob_collide_player(mobs_type* mob, Player player);