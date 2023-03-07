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
