#include <libc.h>

char buff[24];

int pid;

void trigger_page_fault() {
    char *p = 0;
    *p = 0x69;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */    

    // Test write syscall
    if (write(1, "\nHola que tal\n", 14) > 0) {
        write(1, "W\n", 2);
    }

    char *buffer = "\0\0\0\0\0\0\0\0\0\n";

    // Test or sum
    int ret = fork();
    if ( ret== 0 ) {
        itoa(getpid(), buffer);
        write(1, buffer, strlen(buffer));
        write(1, "SOY EL HIJO\n", 12);
    }
    else {
        itoa(getpid(), buffer);
        write(1, buffer, strlen(buffer));
        write(1, "SOY EL PAPA\n", 12);
    }
    
    struct stats st;
    get_stats(1, &st); // aaaaaaaa break here

    perror();

    // Test both gettime and write syscalls
    // Also test write() scrolling capabilities
    // Time 1
    itoa(gettime(), buffer);
    write(1, "Time 1: ", 7);
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer); 

    // Time 2
    itoa(gettime(), buffer);
    write(1, "Time 2: ", 7);
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer); 

    // Time 3
    itoa(gettime(), buffer);
    write(1, "Time 3: ", 7);
    write(1, buffer, 10);

    // Trigger a page fault
    // trigger_page_fault();

    // This point shall never be reached, since the page
    // fault exception never returns
    write(1, "This point shall never be reached if page fault is activated.", 61);
    while(1);
}
