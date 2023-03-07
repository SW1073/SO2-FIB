/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>


extern TSS         tss;
extern Descriptor* gdt;

void writeMSR(long int a, long int b, long int c);

#endif  /* __SYSTEM_H__ */
