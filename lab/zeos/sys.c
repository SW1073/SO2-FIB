/*
 * sys.c - Syscalls implementation
 */
#include <errno.h>
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

int sys_write(int fd, char *buffer, int size) {
    int checkfd = check_fd(fd, ESCRIPTURA);
    if (checkfd != 0) return checkfd;

//  TODO fix this shit => aka do errno.h
    if (buffer == NULL) return -40; 
    if (size <= 0) return -69;

    return sys_write_console(buffer, size);
}

unsigned long sys_gettime() {
    return zeos_ticks;
}

void sys_exit()
{  
}
