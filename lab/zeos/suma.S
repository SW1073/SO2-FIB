#include <asm.h>

ENTRY (addASM)
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax
    add 0xc(%ebp), %eax
    #mov %ebp, %esp     innecesaria, ya que no hemos modificado %esp
    pop %ebp
    ret
