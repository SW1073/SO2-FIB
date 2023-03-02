#include "utils.h"
#include <libc.h>

char buff[24];

int pid;

void page_fault() {
    char *p = 0;
    *p = 0x10;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */


    write(1, "\nhola\n", 6);
    char buffer[30];
    for (int i = 0; i < 30; ++i) buffer[i] = '\0';
    
  while(1) {
      itoa(gettime(), buffer);
      write(1, buffer, strlen(buffer));
      write(1, "\n", 1) ;

    for (int i = 0; i < 100000000; ++i);
  }
}
