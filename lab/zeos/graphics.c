#include <graphics.h>

#define FORMAT_BUFFER_SIZE 15
char format_buffer[FORMAT_BUFFER_SIZE] = "\[00;03H\[40;30m ";

void draw_ij(char c, unsigned char i, unsigned char j, unsigned char fg, unsigned char bg) {
    format_buffer[1] = j / 10 + '0';
    format_buffer[2] = j % 10 + '0';
    format_buffer[4] = i / 10 + '0';
    format_buffer[5] = i % 10 + '0';
    format_buffer[9] = fg + '0';
    format_buffer[12] = bg + '0';
    format_buffer[FORMAT_BUFFER_SIZE - 1] = c;
    write(1, format_buffer, FORMAT_BUFFER_SIZE);
}

void draw_xy(char c, unsigned char x, unsigned char y, unsigned char fg, unsigned char bg) {
    format_buffer[1] = x / 10 + '0';
    format_buffer[2] = x % 10 + '0';
    format_buffer[4] = y / 10 + '0';
    format_buffer[5] = y % 10 + '0';
    format_buffer[9] = fg + '0';
    format_buffer[12] = bg + '0';
    format_buffer[FORMAT_BUFFER_SIZE - 1] = c;
    write(1, format_buffer, FORMAT_BUFFER_SIZE);
}

void erase_xy(unsigned char x, unsigned char y) {
    draw_xy(' ', x, y, 0, 0);
}

void erase_ij(unsigned char i, unsigned char j) {
    draw_ij(' ', i, j, 0, 0);
}

void erase_screen() {
    for (int i = 0; i < SCREEN_HEIGHT; i++)
        for (int j = 0; j < SCREEN_WIDTH; j++)
            erase_ij(i, j);
}

