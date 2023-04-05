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
      movl %edx, %es;   \

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
      popl %gs; \

#define EOI \
    movb $0x20, %al ; \
    outb %al, $0x20; \

ENTRY (clock_handler)
    SAVE_ALL
    EOI
    call stats_user_to_sys  // STATS
    movl 0x18(%esp), %eax   // Recuperar el valor de EAX
    call clock_routine
    call stats_sys_to_user  // STATS
    RESTORE_ALL
    iret

ENTRY (keyboard_handler)
    SAVE_ALL
    EOI
    call stats_user_to_sys  // STATS
    movl 0x18(%esp), %eax   // Recuperar el valor de EAX
    call keyboard_routine
    call stats_sys_to_user  // STATS
    RESTORE_ALL
    iret

// the wrapper for the syscall called this handler with the [int 0x80] instruction,
// and saved in EBX, ECX, EDX, ESI, EDI the arguments of the syscall, and in EAX the
// index of the syscall that wants to be executed.
// sys_call_table behaves as an "array", where %eax is the index of the syscall.
ENTRY (system_call_handler)
    SAVE_ALL // Save the current context
    cmpl $0, %eax // Is syscall number negative?
    jl err // If it is, jump to return an error
    cmpl $MAX_SYSCALL, %eax // Is syscall greater than MAX_SYSCALL?
    jg err // If it is, jump to return an error
    call *sys_call_table(, %eax, 0x04) // Call the corresponding service routine
    jmp fin // Finish
err:
    movl $0xFFFFFFD6, %eax // Move to EAX the ENOSYS error

fin:
    // this guy overwrites the value in the stack, because otherwise EAX would have the value that it had
    // before coming into this function (which is always the index to the syscall we want, so it dont matter we kill it).
    // in the restore_all macro, eax takes the value of the one in the stack.
    // essentially we hacked x86 into returning a value through eax while popping the stack ;)
    movl %eax, 0x18(%esp) // Change the EAX value in the stack
    RESTORE_ALL // Restore the context
    iret

ENTRY(syscall_handler_sysenter)
    push $__USER_DS
    push %EBP // User stack address
    pushfl
    push $__USER_CS
    push 4(%EBP) // User return address
    SAVE_ALL
    
    call stats_user_to_sys  // STATS
    movl 0x18(%esp), %eax   // Recuperar el valor de EAX

    cmpl $0, %EAX
    jl sysenter_err
    cmpl $MAX_SYSCALL, %EAX
    jg sysenter_err
    call *sys_call_table(, %EAX, 0x04)
    jmp sysenter_fin
sysenter_err:
    movl $-38, %EAX
sysenter_fin:
    movl %EAX, 0x18(%ESP)

    call stats_sys_to_user  // STATS

    RESTORE_ALL
    movl (%ESP), %EDX // Return address
    movl 12(%ESP), %ECX // User stack address
    sti // Enable interrupts again
    sysexit

ENTRY (pf_handler)
    call pf_routine
    iret


ENTRY (task_switch)
    pushl %ebp
    movl %esp, %ebp
    
    pushl %esi
    pushl %edi
    pushl %ebx

    pushl 8(%ebp)
    call inner_task_switch

    addl $4, %esp

    popl %ebx
    popl %edi
    popl %esi
    
    movl %ebp, %esp
    popl %ebp
    
    ret
