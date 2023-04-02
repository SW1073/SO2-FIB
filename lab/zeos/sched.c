/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include <sched.h>
#include <mm.h>
#include <io.h>
#include <system.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;

// HEAD de la freequeue
struct list_head freequeue;

// HEAD de la freequeue
struct list_head readyqueue;

// Declaracion del idle_task. 
// Apunta al PCB (o task struct) del proceso idle.
struct task_struct *idle_task;

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

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

    printk("Ejecutado el CPU_IDLE!!!"); // TODO quitar esto

	while(1)
	{
	;
	}
}

void init_idle (void)
{
    // Obtenemos el puntero al inicio del task struct.
    struct task_struct *task_ptr = list_head_to_task_struct(list_first(&freequeue));
        //list_entry(freequeue.next, struct task_struct, PID);

    // No estoy seguro si es necesario, pero como el task_struct ya no esta disponble,
    // no tiene sentido que siga presente dentro de free_queue.
    list_del(freequeue.next);

    // Proceso idle tiene PID 0
    task_ptr->PID = 0;

    // Quantum del proceso
    task_ptr->quantum = INIT_QUANTUM;

    // init dir_pages_baseAaddr. Retorna 1 if OK (siempre)
    allocate_DIR(task_ptr);

    // Castear el pointer del task para obtener el pointer de la union
    union task_union* task_union_ptr = (union task_union*)task_ptr;

    // Store in the stack of the idle process the address of the code that it will
    // execute (address of the cpu_idle function):
    task_union_ptr->stack[KERNEL_STACK_SIZE-1] = (unsigned long)(void*)cpu_idle;

    // Store in the stack the initial value that we want to assign to register ebp
    // when undoing the dynamic link (it can be 0)
    task_union_ptr->stack[KERNEL_STACK_SIZE-2] = 0;

    // inally, we need to keep (in a new field of its task_struct) the position of
    // the stack where we have stored the initial value for the ebp register. This
    // value will be loaded in the esp register when undoing the dynamic link.
    task_union_ptr->task.kernel_esp = (DWord)&task_union_ptr->stack[KERNEL_STACK_SIZE-2];

    // idle_task apunta ahora al pcb que acabamos de inicializar.
    idle_task = task_ptr;
}

void init_task1(void)
{
    // Nos traemos un pcb libre para inicializar el proceso
    struct task_struct *task_ptr = list_head_to_task_struct(list_first(&freequeue));

    // Obtenemos el pointer al task_union tambien.
    union task_union* task_union_ptr = (union task_union*)task_ptr;

    // Quitamos el pcb de la lista de pcb's libres (ya no lo esta)
    list_del(freequeue.next);

    // El proceso inicial tiene PID 1
    task_ptr->PID = 1;

    // El proceso no ha ejecutado ningun tick todavia
    task_ptr->quantum = INIT_QUANTUM;
    
    // init dir_pages_baseAaddr. Retorna 1 if OK (siempre)
    allocate_DIR(task_ptr);

    // Completar la inicializacion del espacio de direcciones de memoria.
    set_user_pages(task_ptr);

    // Update the TSS to make it point to the new_task system stack.
    tss.esp0 = (DWord)&task_union_ptr->stack[KERNEL_STACK_SIZE];

    // kernel_esp debe guardar la posicion dentro de la pila que tenia el esp
    // antes del último cambio de contexto
    task_ptr->kernel_esp = tss.esp0;

    // In case you use sysenter you must modify also the MSR number 0x175.
    // Basicamente, hacemos que la entrada 0x175 del MSR tome valor tss.esp0,
    // en vez del valor INITIAL_ESP que apuntaba antes
    writeMSR(0x175, 0, tss.esp0);

    // Set its page directory as the current page directory in the system,
    // by using the set_cr3 function (see file mm.c).
    set_cr3(task_ptr->dir_pages_baseAddr);
}


void init_sched()
{
    // Init free queue
    INIT_LIST_HEAD( &freequeue );
    // Si insertamos los elementos como se muestra en el codigo (empezando por
    // el indice más alto), los elementos quedan correctamente ordenados, tal que:
    // +-------------------------------------------------------------------------+
    // | freequeue -> task[0] -> task[1] -> ... -> task[NR_TASKS-1] -> freequeue |
    // +-------------------------------------------------------------------------+
    // De lo contratio, el next al que apuntaria freequeue seria task[NR_TASKS-1].
    // Probablemente, este orden sea irrelevante, pero por si las moscas...
    for (int i = NR_TASKS-1; i >= 0; --i)
        list_add(&(task[i].task.list), &freequeue);

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
    tss.esp0 = new->task.kernel_esp;
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
    if (current_ticks_left == 0 && list_empty(&readyqueue)) {
        current_ticks_left = current()->quantum;
        return;
    }

    struct task_struct* next_process = list_empty(&readyqueue) ? idle_task : list_head_to_task_struct(list_first(&readyqueue));
    update_process_state_rr(next_process, NULL);
    current_ticks_left = get_quantum(next_process);
    task_switch((union task_union*)next_process);
}

/**
 * Function to update the current state of a process to a new state. This function deletes the
 * process from its current queue (state) and inserts it into a new queue.
 */
void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
    if (current() != t && t != idle_task)
        list_del(&t->list);
    if (dest != NULL)
        list_add_tail(&t->list, dest);
}

/**
 * Function to decide if it is necessary to change the current process.
 */
int needs_sched_rr() {
    return !current_ticks_left && !list_empty(&readyqueue);
}

/**
 * Function to update the relevant information to take scheduling decisions.
 */
void update_sched_data_rr() {
    --current_ticks_left;
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
