/*
 * interrupt.c -
 */

#include "list.h"
#include "sched.h"
#include <entry.h>
#include <libc.h>
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <system.h>
#include <sched.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;
unsigned int zeos_ticks = 0;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','?','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','?',
  '\0','?','\0','?','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}


void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();
  
  // writemsr aqui x3 por los tres registros.
  writeMSR(0x174, 0, __KERNEL_CS);
  writeMSR(0x175, 0, INITIAL_ESP);
  writeMSR(0x176, 0, (long int)syscall_handler_sysenter);

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
    // setTrapHandler(0x80, system_call_handler, 3);
    setInterruptHandler(14, pf_handler, 0);
    setInterruptHandler(32, clock_handler, 0);
    setInterruptHandler(33, keyboard_handler, 0);

  set_idt_reg(&idtR);
}

void keyboard_routine () {
    int scan_code;
    char key, is_break, c;
    const char  msb_mask = 0x80,
                scan_code_mask = 0x7F,
                not_ascii_char = 'C';

    key = inb(0x60); // Llegim el regisre
    is_break = (key & msb_mask) >> 7; // Make = 0, Break = 1
    if (!is_break) { //When the action of the keyboard is Make
        scan_code = key & scan_code_mask;
        c = char_map[scan_code];
        if (c == '\0') // not ASCII
            printc_xy(0, 0, not_ascii_char);
        else // is ASCII
            printc_xy(0, 0, c);
    }
} 


void pf_routine(int error_code, int eip) {
    char *error_msg = "\nProcess generates a PAGE FAULT exception at EIP: @";
    char itoa_eip[30];
    itoa(eip, itoa_eip);
    printk(error_msg);
    printk(itoa_eip);
    while(1);
}

void change_task() {
    if (list_empty(&readyqueue)) {
        printk("nada por ejecutar!\n");
        task_switch((union task_union*)idle_task);
    } else {
        printk("cambiando...\n");
        task_switch((union task_union*)list_head_to_task_struct(list_first(&readyqueue)));
    }
}

void clock_routine(void) {
    ++zeos_ticks;
    zeos_show_clock();
    if (zeos_ticks != 0 && zeos_ticks%5000 == 0) {
        change_task();
    }
}

