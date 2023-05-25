#include <graphics.h>

#define FORMAT_BUFFER_SIZE 15
char format_buffer[FORMAT_BUFFER_SIZE] = "\[00;03H\[40;30m ";

void g_draw_ij(char c, unsigned char i, unsigned char j, Color fg, Color bg) {
    format_buffer[1] = j / 10 + '0';
    format_buffer[2] = j % 10 + '0';
    format_buffer[4] = i / 10 + '0';
    format_buffer[5] = i % 10 + '0';
    format_buffer[9] = fg + '0';
    format_buffer[12] = bg + '0';
    format_buffer[FORMAT_BUFFER_SIZE - 1] = c;
    write(1, format_buffer, FORMAT_BUFFER_SIZE);
}

void g_draw_xy(char c, unsigned char x, unsigned char y, Color fg, Color bg) {
    g_draw_ij(c, y, x, fg, bg);
}

void g_erase_xy(unsigned char x, unsigned char y) {
    g_draw_xy(' ', x, y, 0, 0);
}

void g_erase_ij(unsigned char i, unsigned char j) {
    g_draw_ij(' ', i, j, 0, 0);
}

void g_fill_screen(char c, Color fg, Color bg) {
    for (int i = 0; i < SCREEN_HEIGHT; i++)
        for (int j = 0; j < SCREEN_WIDTH; j++)
            g_draw_ij(' ', i, j, BLACK, BLACK);
}

void g_erase_screen() {
    g_fill_screen(' ', BLACK, BLACK);
}

int abs(int x) {
    if (x < 0)
        return -x;
    return x;
}

void g_draw_line(char c, int x0, int y0, int x1, int y1, Color fg, Color bg) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int x = x0;
    int y = y0;

    int dir_x = dx < 0 ? -1 : 1;
    int dir_y = dy < 0 ? -1 : 1;
    
    
    if(abs(dx) > abs(dy)) {//this is the case when slope(m) < 1
        g_draw_xy(c,x,y,fg,bg);
        int pk = (2*abs(dy)) - abs(dx);
        for(int i = 0; i < abs(dx) ; i++) {
            x += dir_x;
            if(pk < 0)
                pk = pk + (2*abs(dy));
            else
            {
                y += dir_y;
                pk = pk + (2*abs(dy)) - (2*abs(dx));
            }
            g_draw_xy(c,x,y,fg,bg);
        }
    }
    else { //this is the case when slope is greater than or equal to 1  i.e: m>=1
        g_draw_xy(c,x,y,fg,bg);
        int pk = (2*abs(dx)) - abs(dy);
        for(int i = 0; i < abs(dy) ; i++) {
            y += dir_y;
            if(pk < 0)
                pk = pk + (2*abs(dx));
            else {
                x += dir_x;
                pk = pk + (2*abs(dx)) - (2*abs(dy));
            }
            g_draw_xy(c,x,y,fg,bg);
        }
    }
}
