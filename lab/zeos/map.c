#include <map.h>

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

void draw_map(char *map_bmp) {
    unsigned char mask, jj, color = 0;
    for (int i = 0; i < HEIGHT; i++) {
        jj = 0;
        for (int j = 0; j < WIDTH/8; j++) {
            mask = 0x80;
            while (mask != 0) {
                if (!(map_bmp[i*(WIDTH/8) + j] & mask))
                    // Draw a wall
                    color = 6;
                else
                    // Draw a space
                    color = 0;
                draw_ij(' ', i+MAP_Y_OFFSET, jj+MAP_X_OFFSET, color, color);
                mask >>= 1;
                jj++;
            }
        }
    }
}

void erase_xy(unsigned char x, unsigned char y) {
    draw_xy(' ', x, y, 0, 0);
}

void erase_ij(unsigned char i, unsigned char j) {
    draw_ij(' ', i, j, 0, 0);
}

void erase_screen() {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            erase_ij(i, j);
}

void draw_player_xy(unsigned char x, unsigned char y) {
    draw_xy('P', x+MAP_X_OFFSET, y+MAP_Y_OFFSET, 2, 0);
}

void erase_player_xy(unsigned char x, unsigned char y) {
    erase_xy(x+MAP_X_OFFSET, y+MAP_Y_OFFSET);
}
