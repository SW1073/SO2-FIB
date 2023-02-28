#include "utils.h"
#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    write(1, "\nhola\n", 6);
    char buffer[30];
    for (int i = 0; i < 30; ++i) buffer[i] = '\0';
    
  while(1) {
      itoa(getticks(), buffer);

      for (int i = 0; i < 30; ++i) {
          if (buffer[i] != '\0') continue;

          buffer[i] = '\n';
          break;
      }

      for (int i = 0; i < 100000000; ++i);
      // for (int i = 0; i < 100000000; ++i);
      write(1, buffer, strlen(buffer));
  }
}
