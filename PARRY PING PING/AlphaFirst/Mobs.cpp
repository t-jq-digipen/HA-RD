#include "mobs.h"

static void spawn_mobs(mobs_list* list, f32 x, f32 y, f32 size, Color color) 
{
	if (list->count < array_count(list->maxmobs)) 
	{
		mobs* mob = list->maxmobs + list->count++;
		mob->x = x;
		mob->y = y;
		mob->size = size;
		mob->color = color;
	}
}
static void clear_mobs(mobs_list* list) 
{
	list->count = 0;
}
static void init_mobs(mobs_list* list) 
{
	clear_mobs(list);
}