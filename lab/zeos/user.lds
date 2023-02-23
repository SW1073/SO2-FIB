/*
 *  ZeOS - jcosta septembre 2006
 *  user.lds - Linker Script to create user memory image
 */

ENTRY(main)
SECTIONS
{

  . = 0x100000; /* User DATA will start at this address */
  .rodata : { *(.rodata) }      /* Read Only Data */
  .data : { *(.data) }          /* Normal Data */
  .bss : { *(.bss) }            /* Uninitialized Data */

  . = 0x114000; /* User CODE will start at this address */
  .text : {
       *(.text.main);
       *(.text)
  }
}
