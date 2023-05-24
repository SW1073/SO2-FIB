#ifndef _MAP_H
#define _MAP_H

#include <libc.h>
#include <graphics.h>

#define MAP_HEIGHT 21
#define MAP_WIDTH 80
#define MAP_X_OFFSET 0
#define MAP_Y_OFFSET 3

typedef unsigned char Map[MAP_HEIGHT*MAP_WIDTH/8];

void draw_map(Map map_bmp);
void draw_player_xy(unsigned char x, unsigned char y);
void erase_player_xy(unsigned char x, unsigned char y);

#endif
