/**
 * wrappers.S: implementacion de los wrappers de las syscalls.
 * De momento solo contiene la syscall write(fd,buffer,size)
 */

#include <asm.h>

ENTRY(write)
    pushl %ebp
    movl %esp, %ebp
    movl 0x08(%ebp), %ebx   // %ebx <- fd
    movl 0x0c(%ebp), %ecx   // %ecx <- &buffer[0]
    movl 0x10(%ebp), %edx   // %edx <- size
    movl $0x4, %eax         // %eax <- write syscall id
    int $0x80               // generate a trap
    popl %ebp
    ret
