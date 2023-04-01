/*
 * libc.c 
 */

#include "include/errno.h"
#include <libc.h>

#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(void) {
    char err[] = "\0\0\0\0\0";
    itoa(errno, err);
    write(1, "\n\nerrno code: ", strlen("\n\nerrno code: "));
    write(1, err, strlen(err));
    write(1, "\n", 1);

    // WIP -> work in progress. More will be added if needed.
    switch(-errno) {
        case EACCES:
            write(1, "Permission denied\n", strlen("Permission denied\n"));
            break;
        case EBADF:
            write(1, "Bad file descriptor\n", strlen("Bad file descriptor\n"));
            break;
        case EFAULT:
            write(1, "Bad address\n", strlen("Bad address\n"));
            break;
        case EINVAL:
            write(1, "Invalid argument\n", strlen("Invalid argument\n"));
            break;
        case ENOSYS:
            write(1, "Function not implemented\n", strlen("Function not implemented\n"));
            break;
        case ENOMEM:
            write(1, "Insufficient memory\n", strlen("Insufficient memory\n"));
            break;
        default:
            write(1, "Unknown error\n", strlen("Unknown error\n"));
            break;
    }
}
