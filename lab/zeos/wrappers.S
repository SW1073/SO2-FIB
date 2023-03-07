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
    movl %esp, %ebp

    pushl %ecx
    pushl %edx
    pushl time_fin
    
    movl $10, %eax
    sysenter

time_fin:
    popl %ebp
    //popl %edx   // pop of return address. i dont care about it so put it in edx and overwrite it with actual edx just below
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jge time_no_err
    
    neg %eax
    movl %eax, errno
    mov $-1, %eax

time_no_err:
    leave
    ret
    
