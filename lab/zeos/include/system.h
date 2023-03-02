/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>


extern TSS         tss;
extern Descriptor* gdt;
extern unsigned int zeos_ticks;

#endif  /* __SYSTEM_H__ */
