/*
 * entry.h - Definició del punt d'entrada de les crides al sistema
 */

#ifndef __ENTRY_H__
#define __ENTRY_H__

void system_call_handler();

void keyboard_handler();
void clock_handler();

#endif  /* __ENTRY_H__ */
