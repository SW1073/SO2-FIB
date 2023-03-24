/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include "system.h"
#include "types.h"
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
struct task_struct *init_task;


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
    pcb->task.PID = 0;

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
    pcb->task.PID = 1;

    // hacer que el kernel_esp apunte al tope del stack.
    pcb->task.kernel_esp = &(pcb->stack[KERNEL_STACK_SIZE]);
    tss.esp0 = (DWord)pcb->task.kernel_esp;
    init_task = (struct task_struct*)pcb;
    // writeMSR(0x175, 0, tss.esp0);
    writeMSR(0x175, 0, tss.esp0);

    set_cr3(get_DIR(&(pcb->task)));
}

void init_sched()
{
    INIT_LIST_HEAD(&readyqueue);

    INIT_LIST_HEAD(&freequeue);
    for (int i = NR_TASKS-1; i >= 0; --i) {
        list_add(&(task[i].task.list), &freequeue);
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

    new->task.kernel_esp = &(new->stack[KERNEL_STACK_SIZE]);
    tss.esp0 = (unsigned long)new->task.kernel_esp;
    writeMSR(0x175, 0, tss.esp0);

    set_cr3(get_DIR((struct task_struct*)new));
    DWord ebp = get_ebp();
    current()->kernel_esp = (DWord*)ebp;

    set_esp_and_switch(new->task.kernel_esp);
}

/*

void task_switch(union task_union*t) {
    push ebp
    ebp <- esp

    cr3 <- new->task.DIR            // cambia el tlb para poder traducir las direcciones logicas unicas al proceso
    tss.esp0 <- new->stack[1024]    // pone la nueva pila de sistema (1024 para invalidar todo lo que habia antes).

    // esto no se puede hacer en c, hay que llamar a una función en assembly.
    ebp = current()->task.kernel_esp
    // para este man lo mismo que el de arriba.
    esp <- new->task.kernel_esp

    pop ebp
    ret
}

    |                       |
    |       ebp             |
    |-----------------------|
    |   @ret (a lo que      |
    |   llamo el switch)    |
    |-----------------------|
    |   new (argumento      |
    |   de task_switch)     |
    |-----------------------|
    |   mierda varia de     |
    |   la rutina de sistema|
    |-----------------------|
    |       ebp             |
    |-----------------------|
    |   @ret a usuario      |
    |   (salir de handler)  |
    |-----------------------|
    |       CTX SW          |
    |-----------------------|
    |       CTX HW          |
    |_______________________|

 */

