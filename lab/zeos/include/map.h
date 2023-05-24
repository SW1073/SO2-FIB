#ifndef _MAP_H
#define _MAP_H

#include <libc.h>

#define HEIGHT 21
#define WIDTH 80
#define MAP_X_OFFSET 0
#define MAP_Y_OFFSET 3

typedef char Map[HEIGHT*WIDTH/8];

void draw_ij(char c, unsigned char i, unsigned char j, unsigned char fg, unsigned char bg);
void draw_xy(char c, unsigned char x, unsigned char y, unsigned char fg, unsigned char bg);
void draw_map(char *map_bmp);
void erase_xy(unsigned char x, unsigned char y);
void erase_ij(unsigned char i, unsigned char j);
void erase_screen();
void draw_player_xy(unsigned char x, unsigned char y);
void erase_player_xy(unsigned char x, unsigned char y);

#endif
