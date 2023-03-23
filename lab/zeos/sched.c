/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include "types.h"
#include <sched.h>
#include <mm.h>
#include <io.h>

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
extern struct list_head freequeue;
struct list_head freequeue;

// HEAD de la freequeue
extern struct list_head readyqueue;
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

