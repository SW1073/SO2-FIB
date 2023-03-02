/**
 * wrappers.S: implementacion de los wrappers de las syscalls.
 * De momento solo contiene la syscall write(fd,buffer,size)
 */

#include <asm.h>

ENTRY(write)
    # Boilerplate
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx    

    # Push variables to the correct registers
    movl 0x08(%ebp), %ebx   // %ebx <- fd
    movl 0x0c(%ebp), %ecx   // %ecx <- &buffer[0]
    movl 0x10(%ebp), %edx   // %edx <- size

    movl $0x4, %eax         // %eax <- write syscall id
    int $0x80               // generate a trap

    cmpl $0, %eax
    jge fi_write
    neg %eax
    movl %eax, errno
    movl $-1, %eax
fi_write:
    popl %edx
    popl %ecx
    popl %ebx
    movl %ebp, %esp
    popl %ebp
    ret

ENTRY(gettime)
    # Boilerplate
    pushl %ebp
    movl %esp, %ebp

    movl $0x0a, %eax         // %eax <- gettime syscall id
    int $0x80               // generate a trap

    cmpl $0, %eax
    jge fi_gettime
    neg %eax
    movl %eax, errno
    movl $-1, %eax
fi_gettime:
    movl %ebp, %esp
    popl %ebp
    ret
