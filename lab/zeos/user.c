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

    int pid = fork();

    if (pid == 0) {
        if (fork() == 0) {
            write(1, "child of child\n", strlen("child of child\n"));
        }
    }

    // Test both gettime and write syscalls
    // Also test write() scrolling capabilities
    char *buffer = "\0\0\0\0\0\0\0\0\0\n";
    char *buffer1 = "\0\0\0\0\0\0\0\0\0\0";
    // itoa(getpid(), buffer);
    // write(1, "- PID: ", 7);
    // write(1, buffer, 10);

    // Time 1
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 1: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer);

    // Time 2
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 2: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer);

    // Time 3
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 3: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer);

    // Time 4
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 4: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer);

    // Time 5
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 5: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 5000000; ++i)
        itoa(gettime(), buffer);

    // Time 6
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 6: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    // Trigger a page fault
    // trigger_page_fault();

    // This point shall never be reached, since the page
    // fault exception never returns
    write(1, "no more gettime()\n", strlen("no more gettime()\n"));
    write(1, "This point shall never be reached if page fault is activated.", 61);
    while(1);
}
