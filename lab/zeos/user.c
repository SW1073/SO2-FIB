#include "io.h"
#include <libc.h>

char buff[24];

int pid;

int add(int par1,int par2) {
    return par1 + par2;
}

int addASM(int,int);

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    
    // Test write syscall
    if (write(1, "\nHola que tal\n", 14) > 0) {
        write(1, "W\n", 2);
    }
    // Test both gettime and write syscalls
    // Also test write() scrolling capabilities
    char *buffer = "\0\0\0\0\0\0\0\0\0\n";
    itoa(gettime(), buffer);
    write(1, buffer, 10);

    while(1) {
        for (int i = 0; i < 50000000; ++i);
        itoa(gettime(), buffer);
        write(1, buffer, 10);
    }

    // Trigger a page fault
    char *p = 0;
    *p = 'x';

    // loop forever.
    // This point shall never be reached, since the page
    // fault exception never returns
    write(1, "This point shall never be reached.", 34);
    while(1);
}
