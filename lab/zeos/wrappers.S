/**
 * wrappers.S: implementacion de los wrappers de las syscalls.
 */

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
    // popl %ebp
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

    // popl %ebp
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

ENTRY (getpid)
    pushl %ebp
    movl %esp, %ebp

    movl $20, %eax

    pushl $pid_return_user
    pushl %ebp
    movl %esp, %ebp

    sysenter
pid_return_user:
    // popl %ebp
    leave
    add $4, %esp

    cmpl $0, %eax
    jge pid_no_err

    neg %eax
    movl %eax, errno
    movl $-1, %eax

pid_no_err:
    leave
    ret

ENTRY (fork)
    pushl %ebp
    movl %esp, %ebp

    movl $2, %eax

    pushl $fork_return_user
    pushl %ebp
    movl %esp, %ebp

    sysenter
fork_return_user:
    leave
    // popl %ebp
    add $4, %esp

    cmpl $0, %eax
    jge fork_no_err

    neg %eax
    movl %eax, errno
    movl $-1, %eax

fork_no_err:
    leave
    ret

ENTRY (exit)
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    movl 8(%ebp), %ebx

    movl $1, %eax

    pushl $exit_return_user
    pushl %ebp
    movl %esp, %ebp

    sysenter
exit_return_user:
    leave
    // popl %ebp
    addl $4, %esp

    popl %ebx

    cmpl $0, %eax
    jge fork_no_err

    neg %eax
    movl %eax, errno
    movl $-1, %eax

exit_no_err:
    leave
    ret

ENTRY (get_stats)
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %ecx

    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx

    movl $35, %eax

    pushl $st_return_user
    pushl %ebp
    movl %esp, %ebp

    sysenter
st_return_user:
    leave
    // popl %ebp
    addl $4, %esp

    popl %ebx
    popl %ecx

    cmpl $0, %eax
    jge st_no_err

    neg %eax
    movl %eax, errno
    movl $-1, %eax

st_no_err:
    leave
    ret
