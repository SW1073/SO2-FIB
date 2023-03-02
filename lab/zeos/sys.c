/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <interrupt.h>

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

void sys_exit()
{  
}

/**
 * write syscall implementation
 */
int sys_write(int fd, char * buffer, int size) {
    int err;
    // check if file descriptor is correct
    if ((err = check_fd(fd, 1)) < 0)
        return err;
    // check if the buffer address is not a null pointer
    if (buffer == NULL)
        return EFAULT; /*EFAULT(-14): Bad address || EINVAL(-22): Invalid argument*/
    // check if the size is valid (>0)
    if (size < 0)
        return EINVAL; /*EINVAL: Invalid argument*/

    /* UNSURE IF NECESSARY
    // copy user data segment to kernel address space, for the buffer to point to the correct data
    // if this operation is not done, the pointer will not be accessing the correct data
    if ((err = copy_from_user(buffer, buffer, size)) < 0)
        return err;
    */

    return sys_write_console(buffer, size); // return number of bytes written
}

/**
 * clock syscall implementation
 */
int sys_gettime() {
    return zeos_ticks;
}
