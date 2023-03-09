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

void printc_color(char c, unsigned char foreground_color)
{
    __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
    if (c=='\n')
    {
        x = 0;
        y=(y+1)%NUM_ROWS;
    }
    else
    {
        Word ch = (Word) (c & 0x00FF) | (foreground_color << 8);
        Word *screen = (Word *)0xb8000;
        screen[(y * NUM_COLUMNS + x)] = ch;
        if (++x >= NUM_COLUMNS)
        {
            x = 0;
            y=(y+1)%NUM_ROWS;
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


void printk_color(char *string, unsigned char foreground_color)
{
  int i;
  for (i = 0; string[i]; i++)
    printc_color(string[i], foreground_color);
}
