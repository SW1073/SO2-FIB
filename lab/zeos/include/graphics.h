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

typedef unsigned char Color;

void g_draw_ij(char c, unsigned char i, unsigned char j, Color fg, Color bg);
void g_draw_xy(char c, unsigned char x, unsigned char y, Color fg, Color bg);
void g_erase_xy(unsigned char x, unsigned char y);
void g_erase_ij(unsigned char i, unsigned char j);
void g_fill_screen(char c, Color fg, Color bg);
void g_erase_screen();
void g_draw_line(char c, int x0, int y0, int x1, int y1, Color fg, Color bg);
void g_draw_num(int n, int x, int y, Color fg, Color bg);
void g_write_xy(char *buffer, int size, unsigned char x, unsigned char y, Color fg, Color bg);

#endif
