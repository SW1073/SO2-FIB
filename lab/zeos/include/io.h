/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

#define BLACK           0x00
#define BLUE            0x01
#define GREEN           0x02
#define CYAN            0x03
#define RED             0x04
#define MAGENTA         0x05
#define BROWN           0x06
#define LIGHT_GRAY      0x07
#define DARK_GRAY       0x08
#define LIGHT_BLUE      0x09
#define LIGHT_GREEN     0x0A
#define LIGHT_CYAN      0x0B
#define LIGHT_RED       0x0C
#define LIGHT_MAGENTA   0x0D
#define YELLOW          0x0E
#define WHITE           0x0F

/** Screen functions **/
/**********************/

// === CIRCULAR BUFFER ===
#define TAM_BUF 4

void circ_buff_append(char c);
char circ_buff_read();
char circ_buff_is_full();

Byte inb (unsigned short port);
void printc(char c);
void printc_color(char c, Byte foreground_color, Byte background_color, Byte blink);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
void printk_color(char *string, Byte foreground_color, Byte background_color, Byte blink);

#endif  /* __IO_H__ */
