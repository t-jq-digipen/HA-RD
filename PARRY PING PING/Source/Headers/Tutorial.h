#pragma once
#include "utils.h"
#include "SpriteSheet.h"

extern SpriteSheet WASD_sprite;
extern bool tutorialActive;

void Init_Tutorial();

void Load_Tutorial();

void Unload_Tutorial();

void Update_Tutorial();

void Draw_Tutorial();