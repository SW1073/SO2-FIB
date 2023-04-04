/*
 * sys.c - Syscalls implementation
 */
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

DWord pid_counter = 2;

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

    // Inherit system data: copy the parent's task_union to the child.
    // Determine whether it is necessary to modify the page table of
    // the parent to access the child's system data. 
    // The copy_data function can be used to copy.

    // Obtenemos el task union del hijo y del padre.
    struct list_head *child_list = list_first(&freequeue);
    list_del(child_list);
    union task_union *child = (union task_union*)list_head_to_task_struct(child_list);
    union task_union *parent = (union task_union*)current();

    // Copiamos los contenidos del task struct del padre al hijo
    copy_data(parent, child, sizeof(union task_union));
    
    // Allocate el nuevo directorio
    allocate_DIR(&child->task);

    // Search physical pages to map logical pages for data+stack of the child process    
    // Share the kernel and code pages
    page_table_entry *child_pt = get_PT(&child->task);
    page_table_entry *parent_pt = get_PT(&parent->task);
    copy_pages_to_child(child_pt, parent_pt);

    // Modificar la pila del hijo para que devuelva a ret_from_fork

    child->task.kernel_esp = (DWord)&child->stack[KERNEL_STACK_SIZE-19];
    *((DWord*)child->task.kernel_esp) = 0; // ebp
    *((DWord*)(child->task.kernel_esp+4)) = (DWord)(void*)ret_from_fork; // @ret_from_fork;

    // Asignar el PID al hijo
    child->task.PID = pid_counter++;

    // Devolver ese PID al padre
    list_add_tail(child_list, &readyqueue);
    return child->task.PID;
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
