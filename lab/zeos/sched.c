/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include "system.h"
#include "types.h"
#include "utils.h"
#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

// #if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
// #endif

extern struct list_head blocked;

struct list_head freequeue;
struct list_head readyqueue;

struct task_struct *idle_task;
struct task_struct *init_task; // TODO quitar esto, era solo para probar el task_switch

int global_quantum;
int pids;
int current_ticks = 0;

int get_quantum(struct task_struct *t) {
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
    t->quantum = new_quantum;
}

//------------- stats ----------------
void stats_user_to_system(struct task_struct *t) {
    unsigned long ticks = get_ticks();
    t->st.user_ticks += ticks - t->st.elapsed_total_ticks;
    t->st.elapsed_total_ticks = ticks;
}

void stats_system_to_user(struct task_struct *t) {
    unsigned long ticks = get_ticks();
    t->st.system_ticks += ticks - t->st.elapsed_total_ticks;
    t->st.elapsed_total_ticks = ticks;
}

void stats_system_to_ready(struct task_struct *t) {
    unsigned long ticks = get_ticks();
    t->st.system_ticks += ticks - t->st.elapsed_total_ticks;
    t->st.elapsed_total_ticks = ticks;
}

void stats_ready_to_system(struct task_struct *t) {
    unsigned long ticks = get_ticks();
    t->st.ready_ticks += ticks - t->st.elapsed_total_ticks;
    t->st.elapsed_total_ticks = ticks;
}
//------------------------------------

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

	while(1)
	{
	;
	}
}

void init_stats(struct task_struct *t) {
    t->st.user_ticks = 0;
    t->st.system_ticks = 0;
    t->st.ready_ticks = 0;
    t->st.blocked_ticks = 0;
    t->st.elapsed_total_ticks = get_ticks();
    t->st.total_trans = 0;
    t->st.remaining_ticks = 0;
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

    // se le pone un page directory al task.
    allocate_DIR(&(pcb->task));
 
    // self explanatory.
    pcb->task.PID = pids++;
    pcb->task.quantum = INIT_QUANTUM;
    init_stats((struct task_struct*)pcb);
    // pcb->task.exit_status = 0;

    // init_children(&pcb->task);

    // en el tope del stack de sistema del pcb se pone la dirección de la función que queremos
    // que se resuma al acabar el task_switch
    pcb->stack[KERNEL_STACK_SIZE-1] = (unsigned long)cpu_idle;

    // aún no entiendo muy bien por qué hay que hacer esto pero lo pone el pdf.
    // tiene algo que ver con el valor del ebp al deshacer el dynamic link o algo así.
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
    global_quantum = pcb->task.quantum;
    init_stats((struct task_struct*)pcb);

    // init_children(&pcb->task);

    // hacer que el kernel_esp apunte al tope del stack.
    pcb->task.kernel_esp = &(pcb->stack[KERNEL_STACK_SIZE]);
    tss.esp0 = (DWord)pcb->task.kernel_esp;
    init_task = (struct task_struct*)pcb;
    writeMSR(0x175, 0, tss.esp0);

    set_cr3(get_DIR(&(pcb->task)));
}


void init_sched()
{
    pids = 0;

    INIT_LIST_HEAD(&readyqueue);

    INIT_LIST_HEAD(&freequeue);
    for (int i = 0; i < NR_TASKS; i++) {
        task[i].task.PID = -1;
        list_add_tail(&(task[i].task.list), &freequeue);
    }

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

    tss.esp0 = KERNEL_ESP(new);
    writeMSR(0x175, 0, tss.esp0);

    set_cr3(get_DIR((struct task_struct*)new));
    DWord ebp = get_ebp();
    current()->kernel_esp = (DWord*)ebp;

    set_esp_and_switch(new->task.kernel_esp);
}


void update_sched_data_rr() {
    global_quantum--;
}

int needs_sched_rr() {
    if (global_quantum <= 0 && !list_empty(&readyqueue)) return 1;

    if (global_quantum == 0) global_quantum = current()->quantum;

    return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
    if (t != current() && t != idle_task) list_del(&t->list);
    if (dest != NULL) list_add_tail(&t->list, dest);
}

void sched_next_rr() {
    struct task_struct *next_task;

    if (list_empty(&readyqueue)) 
        next_task = idle_task;
    else
        next_task = list_head_to_task_struct(list_first(&readyqueue));

    update_process_state_rr(next_task, NULL);

    task_switch((union task_union*)next_task);
}

