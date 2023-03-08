#include <asm.h>

ENTRY (write)
    pushl %ebp
    mov %esp, %ebp

    pushl %ebx

    mov 8(%ebp), %ebx       // first argument to ebx
    mov 12(%ebp), %ecx      // second argument to ecx
    mov 16(%ebp), %edx      // third argument to edx

    pushl %ecx
    pushl %edx

    pushl $wr_return_user
    pushl %ebp
    mov %esp, %ebp

    mov $0x04, %eax         // index to sys_call_table -> 4 is write()
    sysenter
    // after this the return value of the syscall will be in eax.
wr_return_user:

    // if eax<0 -> neg eax, save eax to errno and return -1
    // else return eax
    cmpl $0, %eax
    jge wr_fin

    // save -eax to errno here. i dont know how to do that yet.
    neg %eax
    movl %eax, errno
    mov $-1, %eax

wr_fin:
    leave
    add $4, %esp    // quitar wr_return

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
    
    movl $10, %eax

    pushl $time_return_user
    pushl %ebp
    movl %esp, %ebp

    sysenter
time_return_user:

    leave
    add $4, %esp // pop of return address. 

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
    
