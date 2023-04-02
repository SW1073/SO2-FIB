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

// - el task_switch() al final hace:
//     pop ebp
//     ret
// - al hacer pop ebp, ebp ahora vale 0.
// - al hacer ret va a @ret_from_fork (esp ahora apunta a @ret_sys_fork), y hace lo siguiente:
//     push de ebp (ebp aqui vale 0)
//     ebp <- esp (ebp ahora vale esp, y apunta al tope del stack, donde esta el 0 que acabamos de pushear)
//     eax <- 0
//     pop ebp (ebp ahora vale 0, y esp apunta a @ret_sys_fork.
//     ret
// - al hacer ret volvemos al handler de sys_fork

ENTRY (ret_from_fork)
    pushl %ebp
    movl %esp, %ebp

    movl $0, %eax

    // popl %ebp
    leave
    ret

