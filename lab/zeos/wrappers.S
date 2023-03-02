#include <asm.h>

ENTRY (write)
    pushl %ebp
    mov %esp, %ebp

    pushl %ebx
    pushl %ecx
    pushl %edx

    mov 8(%ebp), %ebx       // first argument to ebx
    mov 12(%ebp), %ecx      // second argument to ecx
    mov 16(%ebp), %edx      // third argument to edx

    mov $0x04, %eax         // index to sys_call_table -> 4 is write()
    int $0x80 
    // after this the return value of the syscall will be in eax.

    // if eax<0 -> neg eax, save eax to errno and return -1
    // else return eax
    cmpl $0, %eax
    jge wr_fin

    // save -eax to errno here. i dont know how to do that yet.
    neg %eax
    movl %eax, errno
    mov $-1, %eax

wr_fin:
    popl %edx
    popl %ecx
    popl %ebx

    leave
    ret

ENTRY (gettime)
    pushl %ebp
    mov %esp, %ebp

    mov $10, %eax         // index to sys_call_table -> 10 is getticks()
    int $0x80 
    // after this the return value of the syscall will be in eax.

    // if eax<0 -> neg eax, save eax to errno and return -1
    // else return eax
    cmpl $0x00, %eax
    jge tic_fin

    // save -eax to errno here. i dont know how to do that yet.
    neg %eax
    movl %eax, errno
    mov $-1, %eax

    // save -eax to errno here. i dont know how to do that yet.
    mov $-1, %eax
tic_fin:

    leave
    ret
