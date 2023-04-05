/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include "stats.h"
#include <list.h>
#include <types.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

#define INIT_QUANTUM 10
#define MAX_CHILDREN 10

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  int quantum;
  // int exit_status;
  // int number_of_children;
  page_table_entry * dir_pages_baseAddr;
  struct list_head list;
  DWord *kernel_esp;
  struct stats st;
  // struct task_struct* children[MAX_CHILDREN];
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union task[NR_TASKS]; /* Vector de tasques */
extern struct list_head freequeue;
extern struct list_head readyqueue;

extern struct task_struct *idle_task;
extern struct task_struct *init_task; // TODO quitar esto, era solo para probar el task_switch

extern int global_quantum;
extern int current_ticks;
extern int pids;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

int get_quantum(struct task_struct *t);
void set_quantum(struct task_struct *t, int new_quantum);

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

void init_children(struct task_struct* t);

// -----------------------
void init_stats(struct task_struct *t);
void stats_user_to_system(struct task_struct *t);
void stats_system_to_user(struct task_struct *t);
void stats_system_to_ready(struct task_struct *t);
void stats_ready_to_system(struct task_struct *t);
// -----------------------

struct task_struct * current();

void task_switch(union task_union*t);
void inner_task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

#endif  /* __SCHED_H__ */
