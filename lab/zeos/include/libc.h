/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size);

unsigned long gettime();

void itoa(int a, char *b);

int strlen(char *a);

void perror(void);

int getpid();

int fork();

void exit();

int get_stats(int pid, struct stats *st);

int read(char *b, int maxchars);

int create_thread(void (*start_routine)(void* arg), void *parameter);

void exit_thread(void); 

int mutex_init(int *m);

int mutex_lock(int *m);

int mutex_unlock(int *m);

#endif  /* __LIBC_H__ */
