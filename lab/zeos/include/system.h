/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>


extern TSS         tss;
extern Descriptor* gdt;
extern unsigned int zeos_ticks;

// Definidos en sys_utils.S
void writeMSR(long int msr, long int value_high, long int value_low);

// Retorna el valor acual de %ebp
DWord* get_ebp();

// Hace set del esp actual, pop del ebp del nuevo stack y
// retorna a la address a la que apunta el nuevo stack.
void set_esp_and_switch(DWord* new_esp);

#endif  /* __SYSTEM_H__ */
