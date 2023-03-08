/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <interrupt.h> // para el zeos_ticks.
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return ENOSYS; /*ENOSYS*/
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
    if (buffer == NULL) return EFAULT; 
    if (size <= 0) return EINVAL;

    return sys_write_console(buffer, size);
}

unsigned long sys_gettime() {
    return zeos_ticks;
}

void sys_exit()
{ 
}
