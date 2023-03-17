/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;

// HEAD de la freequeue
extern struct list_head freequeue;
struct list_head freequeue;


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

}

void init_task1(void)
{
}


void init_sched()
{
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

