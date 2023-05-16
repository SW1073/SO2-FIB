/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <system.h>
#include <utils.h>
#include <devices.h>

union task_union task[NR_TASKS]
__attribute__((__section__(".data.task")));

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
    return list_entry( l, struct task_struct, list);
}
#endif

// HEAD de la freequeue
struct list_head freequeue;

// HEAD de la freequeue
struct list_head readyqueue;

struct list_head mutex_blocked;

// Declaracion del idle_task. 
// Apunta al PCB (o task struct) del proceso idle.
struct task_struct *idle_task;

int pids;
int current_ticks;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
    return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
    return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
    int pos;

    pos = ((int)t-(int)task)/sizeof(union task_union);

    t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

    pcbs_in_dir[get_DIR_pos(t)]++;

    return 1;
}

void cpu_idle(void)
{
    __asm__ __volatile__("sti": : :"memory");

    while(1)
    {
        ;
    }
}

void init_idle (void)
{
    // pillar primer pcb libre (esto solo se hará una vez al inicializar el sistema
    // así que no hay problema).
    struct list_head *free_list = list_first(&freequeue);

    // borrar este pcb de la freequeue porque obviamente ya no está libre.
    list_del(free_list);

    // list_head_to_task_struct da un task_struct, así que se castea como un task_union para poder modificar
    // el stack después.
    union task_union *pcb = (union task_union*)list_head_to_task_struct(free_list);

    // -----------
    //      - Esto se quita porque ya se le alocata uno en init_mm().
    //      - Si se le pone otro aquí, pcbs_in_dir[0] sería 2.

    // se le pone un page directory al task.
    // allocate_DIR(&(pcb->task));

    // self explanatory.
    pcb->task.PID = pids++;

    // Inicializamos las estructuras de estadísticas.
    init_process_stats(&pcb->task.stats);

    // en el tope del stack de sistema del pcb se pone la dirección de la función que queremos
    // que se resuma al acabar el task_switch
    pcb->stack[KERNEL_STACK_SIZE-1] = (unsigned long)cpu_idle;

    pcb->stack[KERNEL_STACK_SIZE-2] = 0;

    // hacer que el kernel_esp apunte al tope del stack. como hemos "pusheado" dos
    // valores, kernel_esp será el tope menos dos.
    pcb->task.kernel_esp = &(pcb->stack[KERNEL_STACK_SIZE-2]);

    /*

       -   task_struct y stack de sistema en task_union 
       después de los cambios de arriba:

       |---------------|
       |  task_struct  |
       |===============|
       |   ........    |
       |   ........    |
       |===============| <------ kernel_esp
       |   ebp (0)     |
       |===============|
       |   @cpu_idle   |
       |---------------|
       */

    idle_task = (struct task_struct*)pcb;
}

void init_task1(void)
{
    /*
       - asignar pcb
       - asignar DIR

       - cr3 <- DIR de init (para aclarle las traducciones de direcciones)
       - tss.esp0 <- init.kernel_esp (tss.esp0 es lo que indica al kernel dónde está la pila de sistema)

       - preparar trampolín (ya está hecho):
       - añadir un CTX HW a mano donde esp es la pila del usuario en la page table dentro de DIR
       - añadir un CTW SW a mano para añadir lo mismo que añade el SAVE_ALL pero con los regs
       a 0 y los de segmento ya los sé.
       */

    // pillar primer pcb libre (esto solo se hará una vez al inicializar el sistema
    // así que no hay problema).
    struct list_head *free_list = list_first(&freequeue);

    // borrar este pcb de la freequeue porque obviamente ya no está libre.
    list_del(free_list);

    // list_head_to_task_struct da un task_struct, así que se castea como un task_union para poder modificar
    // el stack después.
    union task_union *pcb = (union task_union*)list_head_to_task_struct(free_list);

    // se le pone un page directory al task.
    allocate_DIR(&(pcb->task));

    set_user_pages(&pcb->task);

    // self explanatory.
    pcb->task.PID = pids++;
    pcb->task.quantum = INIT_QUANTUM;

    // Inicializamos las estructuras de estadísticas.
    init_process_stats(&pcb->task.stats);

    // init_children(&pcb->task);

    // hacer que el kernel_esp apunte al tope del stack.
    pcb->task.kernel_esp = &(pcb->stack[KERNEL_STACK_SIZE]);
    tss.esp0 = (DWord)pcb->task.kernel_esp;
    writeMSR(0x175, 0, tss.esp0);

    set_cr3(get_DIR(&(pcb->task)));
}


void init_sched()
{
    pids = 0;

    // Init free queue
    INIT_LIST_HEAD( &freequeue );
    INIT_LIST_HEAD( &blocked );
    // Si insertamos los elementos como se muestra en el codigo, con list_add_tail,
    // los elementos quedan correctamente ordenados, tal que:
    // +-------------------------------------------------------------------------+
    // | freequeue -> task[0] -> task[1] -> ... -> task[NR_TASKS-1] -> freequeue |
    // +-------------------------------------------------------------------------+
    // Probablemente, este orden sea irrelevante, pero por si las moscas...
    for (int i = 0; i < NR_TASKS; ++i)
        list_add_tail(&(task[i].task.list), &freequeue);

    // Init ready queue (initially empty)
    INIT_LIST_HEAD( &readyqueue );
}

struct task_struct* current()
{
    int ret_value;

    __asm__ __volatile__(
            "movl %%esp, %0"
            : "=g" (ret_value)
            );
    return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch(union task_union *new) {
    // Hacemos que el esp0 apunte al esp del kernel 
    // del proceso que vamos a poner a ejecuta (new).
    tss.esp0 = KERNEL_ESP(new);
    // Cambiamos la entrada que toca del MSR, para
    // las correctas entradas a modo sistema.
    writeMSR(0x175, 0, tss.esp0);
    // Flush de TLB para que las traducciones de las @
    // fisicas a logicas tengan que ser recalculadas
    // (de lo contrario, usariamos el espacio de memoria
    // del proceso anterior)
    set_cr3(get_DIR((struct task_struct*)new));
    // Hacemos que el puntero al kernel_esp del proceso actual
    // apunte al nuevo proceso, desde este momento, 
    // +-----------------+
    // | current() = new |
    // +-----------------+
    current()->kernel_esp = get_ebp();
    // Damos valor al registro esp con el puntero a la pila
    // del nuevo proceso, y devolvemos, poniendo a ejecutar
    // la @ de retorno de la nueva pila de sistema.
    set_esp_and_switch(new->task.kernel_esp);
} 

// ==================== RoundRobin ====================
int current_ticks_left = INIT_QUANTUM;

/**
 * Function to select the next process to execute, to extract it from the ready queue and to invoke
 * the context switch process. This function should always be executed after updating the state
 * of the current process (after calling function update_process_state_rr).
 */
void sched_next_rr() {
    // Escoger el proceso a ejecutar a continuacion
    struct task_struct* next_process;
    if (list_empty(&readyqueue))
        next_process = idle_task;
    else
        next_process = list_head_to_task_struct(list_first(&readyqueue));

    // Actualizar las estadisticas de los procesos que van a intercambiar el contexto
    // +-------------------------+
    // | current(): READY -> SYS |
    // +-------------------------+
    stats_sys_to_ready();
    // +-----------------------------+
    // | new_process(): SYS -> READY |
    // +-----------------------------+
    stats_ready_to_sys(next_process);

    // Cambiar de cola el proceso a ejecutar (lo quitamos de cualquier cola)
    update_process_state_rr(next_process, NULL);

    // Seteamos el quantum de la siguiente ejecucion
    current_ticks_left = get_quantum(next_process);
    stats_reset_remaining_ticks(next_process);

    // printk("haciendo task switch\n");

    // Hacemos el task switch en si. Esta funcion no devuelve por aqui
    task_switch((union task_union*)next_process);
}

/**
 * Function to update the current state of a process to a new state. This function deletes the
 * process from its current queue (state) and inserts it into a new queue.
 */
void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
    if (t != current() && t != idle_task)
        list_del(&t->list);
    if (dest != NULL)
        list_add_tail(&t->list, dest);
}

/**
 * Function to decide if it is necessary to change the current process.
 */
int needs_sched_rr() {
    if (!current_ticks_left && !list_empty(&readyqueue)) {
        return 1; // Necesitamos shceduling!!!
    }

    if (!current_ticks_left) {
        current_ticks_left = current()->quantum;
        stats_reset_remaining_ticks(current());
    }

    return 0;
}

/**
 * Function to update the relevant information to take scheduling decisions.
 */
void update_sched_data_rr() {
    --current_ticks_left;
    stats_decrement_remaining_ticks(current());
}

/**
 * Llama todas las funciones necesarias para decidir
 * que se ejecutara a durante el siguiente tick
 */
void schedule() {
    update_sched_data_rr();
    if (needs_sched_rr()) {
        if (current() != idle_task)
            update_process_state_rr(current(), &readyqueue);
        sched_next_rr();
    }
}

/**
 * Get the quantum of the t process.
 */
int get_quantum (struct task_struct *t) {
    return t->quantum;
}

/**
 * Set the quantum of the t process to new_quantum.
 */
void set_quantum (struct task_struct *t, int new_quantum) {
    t->quantum = new_quantum;
}

// =========================================================
// ==================== Stats funtions  ====================
// =========================================================

void init_process_stats(struct stats *st) {
    st->user_ticks = 0;
    st->system_ticks = 0;
    st->blocked_ticks = 0;
    st->ready_ticks = 0;
    st->elapsed_total_ticks = get_ticks();
    st->total_trans = 0;
    st->remaining_ticks = INIT_QUANTUM;
}

void stats_user_to_sys() {
    DWord current_ticks = get_ticks();
    current()->stats.user_ticks += current_ticks - current()->stats.elapsed_total_ticks;
    current()->stats.elapsed_total_ticks = current_ticks;
}

void stats_sys_to_user() {
    DWord current_ticks = get_ticks();
    current()->stats.system_ticks += current_ticks - current()->stats.elapsed_total_ticks;
    current()->stats.elapsed_total_ticks = current_ticks;
}

void stats_sys_to_ready() {
    DWord current_ticks = get_ticks();
    current()->stats.system_ticks += current_ticks - current()->stats.elapsed_total_ticks;
    current()->stats.elapsed_total_ticks = current_ticks;
}

void stats_ready_to_sys(struct task_struct *t) {
    DWord current_ticks = get_ticks();
    t->stats.ready_ticks += current_ticks - t->stats.elapsed_total_ticks;
    t->stats.elapsed_total_ticks = current_ticks;
    t->stats.total_trans += 1;
}

void stats_decrement_remaining_ticks(struct task_struct *t) {
    t->stats.remaining_ticks--;
}

void stats_set_remaining_ticks(struct task_struct *t, DWord n) {
    t->stats.remaining_ticks = n;
}

void stats_reset_remaining_ticks(struct task_struct *t) {
    t->stats.remaining_ticks = t->quantum;
}

