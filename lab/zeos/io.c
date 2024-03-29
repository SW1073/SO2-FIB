/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

/**************/
/** Screen  ***/
/**************/

const Byte  DEFAULT_FG_COLOR = GREEN;
const Byte  DEFAULT_BG_COLOR = BLACK;
const Byte  DEFAULT_BLINK = 0;
const Byte  DEFAULT_BRIGHT = 1;

const short DEFAULT_COLOR = ((DEFAULT_BLINK ? 1 : 0) << 7) | ((DEFAULT_BG_COLOR & 0x7) << 4) | (DEFAULT_FG_COLOR & 0xF);

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=19;

Word* screen = (Word*) 0xb8000;

char circ_buffer[TAM_BUF];
char *circ_buff_head = &circ_buffer[0];
char *circ_buff_tail = &circ_buffer[0];
int circ_buff_num_items = 0;

void circ_buff_append(char c) {
    if (circ_buff_is_full()) {
        return;
    }

    *circ_buff_head = c;
    circ_buff_head++;
    circ_buff_num_items++;

    if (circ_buff_head == &circ_buffer[TAM_BUF]) {
        circ_buff_head = &circ_buffer[0];
    }

    return;
}

char circ_buff_read() {
    // se mira esta vacio
    if (circ_buff_num_items == 0) {
        return '\0';
    }

    char c = *circ_buff_tail;

    circ_buff_tail++;
    circ_buff_num_items--;

    if (circ_buff_tail == &circ_buffer[TAM_BUF]) {
        circ_buff_tail = &circ_buffer[0];
    }

    return c;
}

char circ_buff_is_full() {
    return circ_buff_num_items == TAM_BUF;
}

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
    Byte v;

    __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
    return v;
}

void scroll_screen() { 
    unsigned char i,j; // valors max: 80 i 25
    Word *prev_line, *curr_line;
    for (i = 1; i < NUM_ROWS; ++i) {
        prev_line = screen + (i-1)*NUM_COLUMNS;
        curr_line = screen + i*NUM_COLUMNS;
        for (j = 0; j < NUM_COLUMNS; ++j) {
            prev_line[j] = curr_line[j];
        }
    }
    // aqui, current line apunta a la last line.
    // Aprovechamos para borrar los caracteres
    // en esa lina. De lo contrario, estaran
    // repetidos.
    for (j = 0; j < NUM_COLUMNS; ++j)
        curr_line[j] = (Word) ('\0' & 0x00FF) | 0x0200;
}

void printc(char c)
{
    __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
    if (c=='\n')
    {
        x = 0;
        if (y+1 >= NUM_ROWS) {
            scroll_screen();
        }
        else {
            y=(y+1); //%NUM_ROWS;
        }
    }
    else
    {
        Word ch = (Word) (c & 0x00FF) | (DEFAULT_COLOR << 8);
        screen[(y * NUM_COLUMNS + x)] = ch;
        if (++x >= NUM_COLUMNS) {
            x = 0;
            if (y+1 >= NUM_ROWS) {
                scroll_screen();
            }
            else {
                y=(y+1); //%NUM_ROWS;
            }
        }
    }
}

void printc_color(char c, Byte foreground_color, Byte background_color, Byte blink)
{
    __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
    if (c=='\n')
    {
        x = 0;
        if (y+1 >= NUM_ROWS) {
            scroll_screen();
        }
        else {
            y=(y+1); //%NUM_ROWS;
        }
    }
    else
    {
        Byte color = ((blink ? 1 : 0) << 7) | ((background_color & 0x7) << 4) | (foreground_color & 0xF);
        Word ch = (Word) (c & 0x00FF) | (color << 8);
        screen[(y * NUM_COLUMNS + x)] = ch;
        if (++x >= NUM_COLUMNS)
        {
            x = 0;
            if (y+1 >= NUM_ROWS) {
                scroll_screen();
            }
            else {
                y=(y+1); //%NUM_ROWS;
            }
        }
    }
}

void printc_xy(Byte mx, Byte my, char c)
{
    Byte cx, cy;
    cx=x;
    cy=y;
    x=mx;
    y=my;
    printc(c);
    x=cx;
    y=cy;
}

void printk(char *string)
{
    int i;
    for (i = 0; string[i]; i++)
        printc(string[i]);
}


void printk_color(char *string, Byte foreground_color, Byte background_color, Byte blink)
{
    int i;
    for (i = 0; string[i]; i++)
        printc_color(string[i], foreground_color, background_color, blink);
}

void erase_current_char() {
    Word ch = (Word) ('\0' & 0x00FF) | 0x0200;
    if (--x < 0) {
        x = NUM_COLUMNS - 1;
        y = (y - 1) % NUM_ROWS;
    }
    screen[(y * NUM_COLUMNS + x)] = ch;

}

void set_cursor(Byte new_x, Byte new_y) {
    if (x >= NUM_COLUMNS || y >= NUM_ROWS) {
        return;
    }
    x = new_x;
    y = new_y;
}
