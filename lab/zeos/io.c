/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

char circ_buffer[TAM_BUF];
char *circ_buff_head = &circ_buffer[0];
char *circ_buff_tail = &circ_buffer[0];

char circ_buff_to_copy[MAX_CHARS_TO_COPY];
int chars_to_copy = -1;

void circ_buff_append(char c) {
    if (circ_buff_head+1 == circ_buff_tail) {
        // printk("buffer lleno\n");
        return;
    }

    *circ_buff_head = c;
    circ_buff_head++;

    if (circ_buff_head == &circ_buffer[TAM_BUF]) {
        circ_buff_head = &circ_buffer[0];
    }

    // if (circ_buff_head == circ_buff_tail) {
    //     // buffer full
    //     circ_buff_tail++;
    //     if (circ_buff_tail == &circ_buffer[TAM_BUF]) {
    //         circ_buff_tail = &circ_buffer[0];
    //     }
    // }
}

char circ_buff_read() {
    if (circ_buff_tail == circ_buff_head) {
        // buffer empty
        return '\0';
    }

    char c = *circ_buff_tail;

    circ_buff_tail++;
    if (circ_buff_tail == &circ_buffer[TAM_BUF]) {
        circ_buff_tail = &circ_buffer[0];
    }

    return c;
}

Byte x, y=19;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
    Byte v;

    __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
    return v;
}

void scroll_screen(Word* screen) { 
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
    Word *screen = (Word *)0xb8000;
    if (c=='\n')
    {
        x = 0;
        if (y+1 >= NUM_ROWS) {
            scroll_screen(screen);
        }
        else {
            y=(y+1); //%NUM_ROWS;
        }
    }
    else
    {
        Word ch = (Word) (c & 0x00FF) | 0x0200;
        screen[(y * NUM_COLUMNS + x)] = ch;
        if (++x >= NUM_COLUMNS) {
            x = 0;
            if (y+1 >= NUM_ROWS) {
                scroll_screen(screen);
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
    Word *screen = (Word *)0xb8000;
    if (c=='\n')
    {
        x = 0;
        if (y+1 >= NUM_ROWS) {
            scroll_screen(screen);
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
                scroll_screen(screen);
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
