#include <map.h>

void map_draw(Map map_bmp) {
    unsigned char mask, jj, color = 0;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        jj = 0;
        for (int j = 0; j < MAP_WIDTH/8; j++) {
            mask = 0x80;
            while (mask != 0) {
                if (EQ_WALL(map_bmp[i*(MAP_WIDTH/8) + j] & mask)) // Draw a wall
                    color = MAP_WALL_COLOR;
                else // Draw a floor
                    color = MAP_FLOOR_COLOR;
                g_draw_ij(' ', i+MAP_Y_OFFSET, jj+MAP_X_OFFSET, color, color);
                mask >>= 1;
                jj++;
            }
        }
    }
}

int map_is_wall(Map map, unsigned char x, unsigned char y) {
    unsigned char j = x/8;
    unsigned char i = y;
    unsigned char mask = 0x80 >> (x%8);
    if (EQ_WALL(map[i*MAP_WIDTH/8 + j] & mask))
        return 1;
    else
        return 0;
}
