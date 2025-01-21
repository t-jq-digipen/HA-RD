#pragma once
#include <utils.h>

#define array_count(x) sizeof(x)/sizeof(*x)

typedef struct mobs {
	f32 x, y, size, attackrange;
	bool isattacking;
	Color color;
}mobs;

typedef struct mobs_list {
	mobs maxmobs[100];
	u32 count;
}mobs_list;

static void spawn_mobs(mobs_list* list, f32 x, f32 y, f32 size, Color color);
static void clear_mobs(mobs_list* list);
static void init_mobs(mobs_list* list);