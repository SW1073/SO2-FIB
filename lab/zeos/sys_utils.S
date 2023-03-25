#include <asm.h>

// MSR[ECX] = EDX:EAX --> EDX high, EAX low
// @param ECX
// @param EDX
// @param EAX
ENTRY (writeMSR)
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %ecx 
    movl 12(%ebp), %edx 
    movl 16(%ebp), %eax 

    wrmsr

    leave
    ret


ENTRY (get_ebp)
    movl %ebp, %eax
    ret


// @param new_esp
ENTRY (set_esp_and_switch)
    movl 4(%esp), %esp
    popl %ebp
    ret
