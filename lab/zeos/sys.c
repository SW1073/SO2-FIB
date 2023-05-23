/*
 * sys.c - Syscalls implementation
 */
#include "entry.h"
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

    pcbs_in_dir[get_DIR_pos(current_proc)]--;

    // TODO !!! preegunta!!!
    // se deberia liberar el task_struct si aun hay gente en el directorio?   

    if (pcbs_in_dir[get_DIR_pos(current_proc)] == 0) {
        // Free user pages
        free_user_pages(current_proc);
    }

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

int sys_read(char *b, int maxchars) {
    if (maxchars <= 0) return EINVAL;
    if (!access_ok(VERIFY_WRITE, b, maxchars)) return EFAULT;

    struct task_struct *t = current();

    t->circ_buff_chars_to_read = maxchars;
    t->circ_buff_maxchars = maxchars;

    // poner proceso en blocked para que el scheduler no le pille.
    update_process_state_rr(t, &blocked);

    int diff = t->circ_buff_maxchars - t->circ_buff_chars_to_read;
    char buff[TAM_BUF];
    while (t->circ_buff_chars_to_read > 0) {
        sched_next_rr();

        int i = 0;
        char c = circ_buff_read();
        while (c != '\0') {
            buff[i] = c;
            ++i;
            c = circ_buff_read();
        }

        copy_to_user(buff, b + diff, i);

        diff = t->circ_buff_maxchars - t->circ_buff_chars_to_read;
    }

    copy_to_user((void*)"\0", b+diff, 1);

    // update_process_state_rr(current(), &readyqueue);
    // sched_next_rr();
    list_del(&current()->list);
    list_add_tail(&current()->list, &readyqueue);

    return maxchars;
}

void sys_exit_thread(void) {
    struct task_struct* t = current();

    // TODO reemplazar esto con bithack raro para pillar
    // la página del esp. Esto borra todo lo que hay después de
    // las páginas de código en la tabla de páginas.
    // del_ss_extra_pages(get_PT(t));

    union task_union* t_union = (union task_union*)t;
    DWord *base_stack = &(t_union->stack[KERNEL_STACK_SIZE]);

    DWord page = base_stack[-2] >> 12;

    del_ss_pag(get_PT(t), page);

    sys_exit();
}

int sys_create_thread(void (*start_routine)(void* arg), void *parameter) {
    if (list_empty(&freequeue)) {
        return ENOMEM;
    }

    if (!access_ok(VERIFY_READ, start_routine, sizeof(void*))) {
        return EFAULT;
    }

    // if (!access_ok(VERIFY_READ, parameter, sizeof(void*))) {
    //     return EFAULT;
    // }

    struct list_head *free_list_pos = list_first(&freequeue);
    list_del(free_list_pos);

    struct task_struct* pcb = list_head_to_task_struct(free_list_pos);
    union task_union* pcb_union = (union task_union*)pcb;

    copy_data(current(), pcb_union, sizeof(union task_union));

    int dir_pos = get_DIR_pos(pcb);
    pcbs_in_dir[dir_pos]++;

    DWord *new_stack = get_new_stack(get_PT(pcb));
    if (new_stack == NULL) return ENOMEM;

    DWord *base_stack = &(pcb_union->stack[KERNEL_STACK_SIZE]);

    new_stack[(PAGE_SIZE/4)-1] = (DWord)parameter;
    new_stack[(PAGE_SIZE/4)-2] = (DWord)0; // evil floating point bit level hacking

    pcb->kernel_esp = &(pcb_union->stack[KERNEL_STACK_SIZE-19]); // 17-2 por el @ret_from_fork y el ebp.
    pcb_union->stack[KERNEL_STACK_SIZE-19] = 0;
    pcb_union->stack[KERNEL_STACK_SIZE-18] = (DWord)ret_from_fork;

    base_stack[-5] = (DWord)start_routine;
    base_stack[-2] = (DWord)&new_stack[(PAGE_SIZE/4)-2];

    list_add_tail(free_list_pos, &readyqueue);

    return 0;
}

int sys_mutex_init(int *m) {
    if (!access_ok(VERIFY_READ, m, sizeof(int))) {
        return EFAULT;
    }

    int m_sys = 0;
    copy_from_user(m, &m_sys, sizeof(int));

    struct mutex_t *mutex = mutex_get(m_sys);
    if (mutex == NULL) return -1;

    mutex->count = 0;

    return 0;
}

int sys_mutex_lock(int *m) {
    if (!access_ok(VERIFY_WRITE, m, sizeof(int)) || !access_ok(VERIFY_READ, m, sizeof(int))) {
        return EFAULT;
    }

    int m_sys;
    copy_from_user(m, &m_sys, sizeof(int));

    struct mutex_t *mutex = mutex_get(m_sys);
    if (mutex == NULL || mutex->count == -1) return -1;

    if (mutex->count >= 1) {
        update_process_state_rr(current(), &mutex->blocked_queue);
        sched_next_rr();
    }

    mutex->count++;

    return 0;
}

int sys_mutex_unlock(int *m) {
    if (!access_ok(VERIFY_WRITE, m, sizeof(int)) || !access_ok(VERIFY_READ, m, sizeof(int))) {
        return EFAULT;
    }

    int m_sys;
    copy_from_user(m, &m_sys, sizeof(int));

    struct mutex_t *mutex = mutex_get(m_sys);
    if (mutex == NULL || mutex->count == -1) return -1;

    if (mutex->count == 0) return 0; // nose cuando puede pasar esto.

    if (!list_empty(&mutex->blocked_queue)) {
        struct task_struct *t = list_head_to_task_struct(list_first(&mutex->blocked_queue));
        list_del(&t->list);
        list_add(&t->list, &readyqueue);
    }

    mutex->count--;

    return 0;
}

char* sys_dyn_mem(int num_bytes) {
    if (num_bytes <= 0) return NULL;

    page_table_entry *pt = get_PT(current());

    sbrk -= num_bytes;

    if (pt[(int)sbrk>>12].bits.present == 0)  {
        int frame = alloc_frame();
        if (frame == -1) return NULL;

        set_ss_pag(pt, (int)sbrk>>12, frame);
    }

    return sbrk;
}
