#pragma once
#include "utils.h"


void Init_Camera();

void Update_Camera();

f32 World_To_Screen_x(f32 pos_x);
f32 World_To_Screen_y(f32 pos_y);

f32 Screen_To_World_x(f32 pos_x);
f32 Screen_To_World_y(f32 pos_y);

