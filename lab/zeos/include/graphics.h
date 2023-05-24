#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <libc.h>

#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

void draw_ij(char c, unsigned char i, unsigned char j, unsigned char fg, unsigned char bg);
void draw_xy(char c, unsigned char x, unsigned char y, unsigned char fg, unsigned char bg);
void erase_xy(unsigned char x, unsigned char y);
void erase_ij(unsigned char i, unsigned char j);
void erase_screen();

#endif
