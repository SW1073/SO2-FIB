/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>


extern TSS         tss;
extern Descriptor* gdt;
extern unsigned int zeos_ticks;

void writeMSR(long int msr, long int value_high, long int value_low);
DWord get_ebp();
void set_esp_and_switch(DWord* new_esp);
int ret_from_fork();

#endif  /* __SYSTEM_H__ */
