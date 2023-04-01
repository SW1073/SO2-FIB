/*
 * sys.c - Syscalls implementation
 */
#include "list.h"
#include "system.h"
#include "types.h"
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <interrupt.h> // para el zeos_ticks.
#include <errno.h>

#include <interrupt.h>

#define LECTURA 0
#define ESCRIPTURA 1

// no sé si está bien pero de momento se queda así.
char sys_buffer[4096];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return ENOSYS; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
    if (list_empty(&freequeue)) {
        return ENOMEM;
    }

    struct list_head *free_list_pos = list_first(&freequeue);
    list_del(free_list_pos);

    struct task_struct* pcb = list_head_to_task_struct(free_list_pos);
    union task_union* pcb_union = (union task_union*)pcb;

    copy_data(current(), pcb_union, sizeof(union task_union));
    allocate_DIR(pcb); // el copy_data() copia el directorio (task.dir_pages_baseAddr) del padre al hijo, así que hay que darle otro.

    // CTX HW + CTX SW + @ret_handler = 17 posiciones encima de la base del stack.
    // Hay que tener en cuenta que encima de estas 17 posiciones hay el @ret_from_fork y el ebp falso.
    pcb->kernel_esp = &(pcb_union->stack[KERNEL_STACK_SIZE]) - 19; // 17-2 por el @ret_from_fork y el ebp.

    pcb_union->stack[KERNEL_STACK_SIZE-19] = 0;
    pcb_union->stack[KERNEL_STACK_SIZE-18] = (DWord)ret_from_fork;
 
    if (copy_and_allocate_pages(current(), pcb) < 0) {
        list_add_tail(free_list_pos, &freequeue);
        return ENOMEM;
    }

    list_add_tail(free_list_pos, &readyqueue);

    pcb->PID = pids++;

    return pcb->PID;
}

void sys_exit()
{ 
    /*
    - argumento status.

        - libera todos los recursos del proceso.
        - no liberar pcb porque tengo que guardar el status.
        - llamar al scheduler.
            - el planificador entonces hace task_switch y el hijo se queda esperando al wait() del padre para morir.

     */
}

int sys_wait() {
    /*
        - mira los hijos a ver si tiene algun zombie.
        - si tiene algún zombie:
                - pilla el exit status del pcb.
                - pilla el pid del pcb.
                - guarda el exit status en algún lugar (??)
                - libera el pcb.
                - retorna el pid.
     */

    return 0;
}

/**
 * write syscall implementation
 */
int sys_write(int fd, char * buffer, int size) {
    int err;
    // check if file descriptor is correct
    if ((err = check_fd(fd, ESCRIPTURA)) < 0)
        return err;
    // check if the buffer address is not a null pointer
    if (buffer == NULL)
        return EFAULT; /*EFAULT(-14): Bad address || EINVAL(-22): Invalid argument*/
    // check if the size is valid (>0)
    if (size < 0)
        return EINVAL; /*EINVAL: Invalid argument*/

    // copy user data segment to kernel address space, for the buffer to point to the correct data
    // if this operation is not done, the pointer will not be accessing the correct data
    if ((err = copy_from_user(buffer, sys_buffer, size)) < 0)
        return err;

    return sys_write_console(sys_buffer, size); // return number of bytes written
}

/**
 * clock syscall implementation
 */
unsigned long sys_gettime() {
    return zeos_ticks;
}


/*
 - sys_fork()
  - primero comprobacion de que tengo los recursos necesarios:
  - si alguno falla, los de arriba se liberan:

    - asignar un task struct al nuevo proceso.
    - asignar una Page Table (DIR) 
    - asignar memoria fisica.

  - ahora se empieza el trabajo en si:

    - init task_union del hijo -> copiar el del padre al hijo.
    - copiar memoria del padre al hijo (tocho):
        - el contenido de kernel del PCB son iguales entre ellos. No se copian, el TLB referencia el del padre.

    - asignar pid
    - preparar contexto de ejecución del hijo
    - poner task_struct del hijo en readyqueue

    |-----------------------|
    |   mierda varia de     |
    |   la rutina de sistema|
    |-----------------------|
    |       ebp             |
    |-----------------------|
    |   @ret a sys_fork     |
    |   (salir de handler)  |
    |-----------------------|
    |       CTX SW          |
    |-----------------------|
    |       CTX HW          |
    |-----------------------|


    - solo hijo:
        - crear función ret_from_fork() que literal solo retorne 0.

        |-----------------------|
        |           0           |
        |-----------------------|
        |    @ret_from_fork     |
        |-----------------------|
        |   @ret a sys_fork     |
        |   (salir de handler)  |
        |-----------------------|
        |       CTX SW          |
        |-----------------------|
        |       CTX HW          |
        |-----------------------|

        - el task_switch() al final hace:
            pop ebp
            ret
        - al hacer pop ebp, ebp ahora vale 0.
        - al hacer ret va a @ret_from_fork (esp ahora apunta a @ret_sys_fork), y hace lo siguiente:
            push de ebp (ebp aqui vale 0)
            ebp <- esp (ebp ahora vale esp, y apunta al tope del stack, donde esta el 0 que acabamos de pushear)
            eax <- 0
            pop ebp (ebp ahora vale 0, y esp apunta a @ret_sys_fork.
            ret
        - al hacer ret volvemos al handler de sys_fork
 */
