/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
    pushl %gs; \
    pushl %fs; \
    pushl %es; \
    pushl %ds; \
    pushl %eax; \
    pushl %ebp; \
    pushl %edi; \
    pushl %esi; \
    pushl %edx; \
    pushl %ecx; \
    pushl %ebx; \
    movl $__KERNEL_DS, %edx;    \
    movl %edx, %ds;           \
    movl %edx, %es

#define RESTORE_ALL \
    popl %ebx; \
    popl %ecx; \
    popl %edx; \
    popl %esi; \
    popl %edi; \
    popl %ebp; \
    popl %eax; \
    popl %ds; \
    popl %es; \
    popl %fs; \
    popl %gs;

#define EOI \
    movb $0x20, %al ; \
    outb %al, $0x20 ;


ENTRY(system_call_handler)
    SAVE_ALL                            // Save the current context
    cmpl $0, %EAX                       // Is syscall number negative?
    jl err                              // If it is, jump to return an error
    cmpl $MAX_SYSCALL, %EAX             // Is syscall greater than MAX_SYSCALL (4)?
    jg err                              // If it is, jump to return an error
    call *sys_call_table(, %EAX, 0x04)  // Call the corresponding service routine
    jmp fin                             // Finish
err:
    movl $-38, %EAX                     // Move to EAX the ENOSYS error
fin:
    movl %EAX, 0x18(%esp)               // Change the EAX value in the stack
    RESTORE_ALL                         // Restore the context
    iret


ENTRY(keyboard_handler)
    SAVE_ALL
    EOI
    call keyboard_routine
    RESTORE_ALL    
    iret

ENTRY(clock_handler)
    SAVE_ALL
    EOI
    call clock_routine
    RESTORE_ALL
    iret

ENTRY(pf_handler)
    call pf_routine
    iret
