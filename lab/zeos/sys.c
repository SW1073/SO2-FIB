/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
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
    // int PID=-1;
    // creates the child process
    // Get a free task_struct for the process. 
    // If there is no space for a new process, an error will be returned

    if (list_empty(&freequeue)) {
        return ENOMEM;
    }

    struct list_head *free_list_pos = list_first(&freequeue);
    list_del(free_list_pos);

    struct task_struct* pcb = list_head_to_task_struct(free_list_pos);
    union task_union* pcb_union = (union task_union*)pcb;

    copy_data(current(), pcb_union, sizeof(union task_union));
    allocate_DIR(pcb); // el copy_data() copia el directorio (task.dir_pages_baseAddr) del padre al hijo, así que hay que darle otro.
    init_process_stats(&pcb->stats);

    // CTX HW + CTX SW + @ret_handler = 17 posiciones encima de la base del stack.
    // Hay que tener en cuenta que encima de estas 17 posiciones hay el @ret_from_fork y el ebp falso.
    pcb->kernel_esp = &(pcb_union->stack[KERNEL_STACK_SIZE-19]); // 17-2 por el @ret_from_fork y el ebp.

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

/**
 * exit syscall implementation
 */
void sys_exit() {
    struct task_struct* current_proc = current();
    // page_table_entry* current_proc_pt = get_PT(current_proc);

    // Free user pages
    free_user_pages(current_proc);
    // Free pcb
    list_add_tail(&current_proc->list, &freequeue);

    // Let rr decide next proc to execute
    // This function will not return
    sched_next_rr();
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
    // Invalid buffer argument address
    if (!access_ok(VERIFY_READ, buffer, size))
        return EFAULT;

    // copy user data segment to kernel address space, for the buffer to point to the correct data
    // if this operation is not done, the pointer will not be accessing the correct data
    if ((err = copy_from_user(buffer, sys_buffer, size)) < 0)
        return err;

    return sys_write_console(sys_buffer, size); // return number of bytes written
}

/**
 * gettime syscall implementation
 */
unsigned long sys_gettime() {
    return zeos_ticks;
}


/**
 * get_stats syscall implementation
 */
int sys_get_stats(int pid, struct stats *st) {

    if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats)))
        return EFAULT; // Invalid st argument address

    if (pid < 0)
        return EINVAL; // Invalid PID argument

    DWord i;
    for (i = 0; i < NR_TASKS; ++i) {
        if (task[i].task.PID == pid) {
            copy_to_user(&task[i].task.stats, st, sizeof(struct stats));
            return 0;
        }
    }
    return ESRCH; // No such process
}

int read(char *b, int maxchars) {
    if (maxchars <= 0) return EINVAL;

    update_process_state_rr(current(), &blocked);

    char c = 0;
    // TODO bloquear el proceso hasta esto que termine
    for (int i = 0; i < maxchars; ++i) {
        while (c == '\0') c = circ_buff_get_last();

        sys_buffer[i] = c;
    }

    update_process_state_rr(current(), &freequeue);

    return 0;
}
