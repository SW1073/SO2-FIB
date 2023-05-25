#ifndef _MAP_H
#define _MAP_H

#include <libc.h>
#include <graphics.h>

#define MAP_HEIGHT 21
#define MAP_WIDTH 80
#define MAP_X_OFFSET 0
#define MAP_Y_OFFSET 3

#define MAP_WALL_COLOR GREEN
#define MAP_FLOOR_COLOR BLACK

#define PLAYER_COLOR YELLOW

#define EQ_WALL(b) ((b) == 0)

typedef unsigned char Map[MAP_HEIGHT*MAP_WIDTH/8];

void map_draw(Map map_bmp);

int map_is_wall(Map map, unsigned char x, unsigned char y);

#endif
