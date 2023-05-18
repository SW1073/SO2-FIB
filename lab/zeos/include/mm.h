/*
 * mm.h - Capçalera del mòdul de gestió de memòria
 */

#ifndef __MM_H__
#define __MM_H__

#include <types.h>
#include <mm_address.h>
#include <sched.h>

 
#define FREE_FRAME 0
#define USED_FRAME 1
/* Bytemap to mark the free physical pages */
extern Byte phys_mem[TOTAL_PAGES];

extern char* sbrk;

extern page_table_entry dir_pages[NR_TASKS][TOTAL_PAGES];

extern unsigned int pcbs_in_dir[NR_TASKS];

int init_frames( void );
int alloc_frame( void );
void free_user_pages( struct task_struct* task );
void free_frame( unsigned int frame );
void set_user_pages( struct task_struct *task );
int copy_and_allocate_pages(struct task_struct *parent, struct task_struct *child);
int get_free_page(page_table_entry *PT);
void free_user_pages(struct task_struct *task);
void del_ss_extra_pages(page_table_entry *PT);
void abort_copy(struct task_struct *parent, struct task_struct *child);
DWord* get_new_stack(page_table_entry *PT);
int get_DIR_pos(struct task_struct *t);

extern Descriptor  *gdt;

extern TSS         tss; 

void init_mm();
void set_cr3(page_table_entry *dir);

void setGdt();

void setTSS();

void set_ss_pag(page_table_entry *PT, unsigned page,unsigned frame);
void del_ss_pag(page_table_entry *PT, unsigned page);
unsigned int get_frame(page_table_entry *PT, unsigned int page);

// Utils propis
int copy_pages_to_child(struct task_struct *child, struct task_struct *parent);
int get_free_page(page_table_entry *pt);
void del_ss_extra_pages(page_table_entry *pt);

#endif  /* __MM_H__ */
