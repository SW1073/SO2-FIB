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

    // save -eax to errno here. 
    neg %eax
    movl %eax, errno
    mov $-1, %eax

wr_fin:
    popl %ebp
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

    popl %ebp
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
    

// Wrapper para la syscall fork()
ENTRY (fork)
    pushl %ebp
    movl %esp, %ebp

    movl $2, %eax // id de la syscall fork() -> 2

    pushl $fork_return_user // @ a la que sysenter debe devolver
    pushl %ebp              // el ebp actual, que debera usarse al volver
    movl %esp, %ebp

    sysenter                // saltamos a la segunda entrada de la sys_call_table
fork_return_user:
    popl %ebp
    add $4, %esp // pop of return address. 

    cmpl $0, %eax
    jge fork_no_err

    // Ha habido un error.
    // Devolvemos -1 y ponemos el
    // codigo de error en errno
    neg %eax
    movl %eax, errno
    mov $-1, %eax

fork_no_err:
    leave
    ret
    
// Wrapper para la syscall exit()
ENTRY (exit)
    pushl %ebp
    movl %esp, %ebp

    movl $1, %eax // id de la syscall exit() -> 1

    pushl $exit_return_user // @ a la que sysenter debe devolver
    pushl %ebp              // el ebp actual, que debera usarse al volver
    movl %esp, %ebp

    sysenter                // saltamos a la primera entrada de la sys_call_table

    // Exit no devuelve error. Es más, nunca devuelve por donde
    // se la ha llamado. Por lo tanto, las siguientes lineas de
    // codigo nunca se ejecutan.

exit_return_user:
    popl %ebp
    add $4, %esp // pop of return address.

    leave
    ret



// Wrapper para la syscall fork()
ENTRY (getpid)
    pushl %ebp
    movl %esp, %ebp

    movl $20, %eax // id de la syscall getpid() -> 20

    pushl $getpid_return_user // @ a la que sysenter debe devolver
    pushl %ebp              // el ebp actual, que debera usarse al volver
    movl %esp, %ebp

    sysenter                // saltamos a la vigesima entrada de la sys_call_table
getpid_return_user:
    popl %ebp
    add $4, %esp // pop of return address. 

    cmpl $0, %eax
    jge getpid_no_err

    // Ha habido un error.
    // Devolvemos -1 y ponemos el
    // codigo de error en errno
    neg %eax
    movl %eax, errno
    mov $-1, %eax

getpid_no_err:
    leave
    ret

// get stats
ENTRY (get_stats)
    pushl %ebp
    mov %esp, %ebp

    // Store the values of ecx and edx in the user stack
    // (El registro ebx lo guardamos por convencion de 
    // parametros entre funciones)
    pushl %ebx
    pushl %edx
    pushl %ecx

    // Parameter passing through registers
    mov 8(%ebp), %ebx       // first param : pid -> ebx
    mov 12(%ebp), %ecx      // second param: *st -> ecx
    
    // Put the identifier of the system call in the EAX register
    mov $35, %eax           // 35 is get_stats()

    // Store the return address to user mode in the user stack
    pushl $get_stats_return_user

    // Create a fake dynamic link
    pushl %ebp
    mov %esp, %ebp

    // Enter the system
    sysenter

    // After this the return value of the syscall will be in eax.
get_stats_return_user:

    // if eax<0 -> neg eax, save eax to errno and return -1
    // else return eax
    cmpl $0, %eax
    jge get_stats_fin

    // save -eax to errno
    neg %eax
    movl %eax, errno
    mov $-1, %eax

get_stats_fin:
    // Pop our ebp
    popl %ebp
    
    // Skip the return address we pushed
    add $4, %esp    // quitar @return

    // Pop both ecx and edx, in inverse order to the pushing one
    popl %ecx
    popl %edx
    popl %ebx

    // Pop the caller ebp and restore the esp 
    leave

    // Return to the caller
    ret
