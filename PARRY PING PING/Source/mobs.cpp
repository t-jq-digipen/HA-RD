#include "mobs.h"


static void add_mobs(mobs_list_type* list, f32 x, f32 y, f32 size, Color color)
{
	if (list->count < array_count(list->mobs)) 
	{
		mobs_type* mob = list->mobs + list->count++;
		mob->x = x;
		mob->y = y;
		mob->size = size;
		mob->color = color;
	}
}
static void Update_mobs(mobs_list_type*list,Color color) {
	int count = get_current_mobs(list);
	for (int i = 0; i < count; ++i) 
	{
		mobs_type* mob = find_mob_by_index(list, i);
		DrawCircle(World_To_Screen_x(mob->x), World_To_Screen_y(mob->y), mob->size, color);
		/*if (mob_collide_player(mob, player) == true)
		{
			DrawCircle(mob->x, mob->y, mob->size, color);
		}
		else
		{
			if(mob->x>player.x)
			DrawCircle(mob->x, mob->y, mob->size, color);
		}*/
	}
}
static void clear_mobs(mobs_list_type* list) 
{
	list->count = 0;
}
static void init_mobs(mobs_list_type* list) 
{
	clear_mobs(list);
}
static mobs_type* find_mob_by_index(mobs_list_type* list, int index)
{
	if (index < 0 || index >= list->count)
		return 0;
	return list->mobs + index;
}
static int get_current_mobs(mobs_list_type* list)
{
	return list->count;
}