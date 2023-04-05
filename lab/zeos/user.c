#include "stats.h"
#include <libc.h>

char buff[24];

int pid;

void trigger_page_fault() {
    char *p = 0;
    *p = 0x69;
}

void write_wrapper(char *msg) {
    if (write(1, msg, strlen(msg)) < 0)
        perror();
}

void write_msg_n_num(char *msg, int num) {
    char* buffer = "\0\0\0\0\0\0\0\0\0\0\0\n";
    itoa(num, buffer);
    write_wrapper(msg);
    write_wrapper(buffer);
    write_wrapper("\n");
}

void print_stats(struct stats st) {
    char *buffer = "\0\0\0\0\0\0\0\0\0\n";
    write(1, "user ticks: ", strlen("user ticks: "));
    itoa(st.user_ticks, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);

    write(1, "system ticks: ", strlen("system ticks: "));
    itoa(st.user_ticks, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);

    write(1, "blocked ticks: ", strlen("blocked ticks: "));
    itoa(st.blocked_ticks, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);

    write(1, "ready ticks: ", strlen("ready ticks: "));
    itoa(st.ready_ticks, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);

    write(1, "total transitions (ready->run): ", strlen("total transitions (ready->run): "));
    itoa(st.total_trans, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);

    write(1, "remaining ticks: ", strlen("remaining ticks: "));
    itoa(st.remaining_ticks, buffer);
    write(1, buffer, strlen(buffer));
    write(1, "\n", 1);
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

// Test or sum
    int ret = fork();
    if ( ret== 0 ) {
        write_msg_n_num("Soy el hijo. PID: ", getpid());
        ret = fork();
        if (ret == 0) {
            write_msg_n_num("Soy el hijo. PID: ", getpid());
            ret = fork();
            if (ret == 0) {
                write_msg_n_num("Soy el hijo. PID: ", getpid());
                ret = fork();
                if (ret == 0) {
                    write_msg_n_num("Soy el hijo. PID: ", getpid());
                    ret = fork();
                    if (ret == 0) {
                        write_msg_n_num("Soy el hijo. PID: ", getpid());
                        ret = fork();
                        if (ret == 0) {
                            write_msg_n_num("Soy el hijo. PID: ", getpid());
                            ret = fork();
                            if (ret == 0) {
                                write_msg_n_num("Soy el hijo. PID: ", getpid());
                                ret = fork();
                                if (ret == 0) {
                                    write_msg_n_num("Soy el hijo. PID: ", getpid());
                                    ret = fork();
                                    if (ret == 0) {
                                        write_msg_n_num("Soy el hijo. PID: ", getpid());
                                        ret = fork();
                                    }else if (ret == -1) { perror(); }
                                }else if (ret == -1) { perror(); }
                            }else if (ret == -1) { perror(); }
                        }else if (ret == -1) { perror(); }
                    }else if (ret == -1) { perror(); }
                }else if (ret == -1) { perror(); }
            }else if (ret == -1) { perror(); }
        }else if (ret == -1) { perror(); }
    }else if (ret == -1) { perror(); }
    //
    // exit(-1);

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

    for (int i = 0; i < 500000; ++i)
        itoa(gettime(), buffer);

    // Time 2
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 2: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    // if (pid3 != -1) {
    //     write(1, "exiting!\n", strlen("exiting!\n"));
    //
    //     struct stats st;
    //     get_stats(pid3, &st);
    //     print_stats(st);
    //
    //     exit(-1);
    // }

    for (int i = 0; i < 500000; ++i)
        itoa(gettime(), buffer);

    // Time 3
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 3: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 500000; ++i)
        itoa(gettime(), buffer);

    // Time 4
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 4: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 500000; ++i)
        itoa(gettime(), buffer);

    // Time 5
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 5: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    for (int i = 0; i < 500000; ++i)
        itoa(gettime(), buffer);

    // Time 6
    itoa(getpid(), buffer1);
    itoa(gettime(), buffer);
    write(1, "- PID: ", strlen("- PID: "));
    write(1, buffer1, 10);
    write(1, " || Time 6: ", strlen(" || Time 1: "));
    write(1, buffer, 10);

    write(1, "\n\n", 2);

    write(1, "- PID: ", strlen("- PID: "));
    struct stats st;
    get_stats(getpid(), &st);
    print_stats(st);

    // Trigger a page fault
    // trigger_page_fault();

    // This point shall never be reached, since the page
    // fault exception never returns
    // write(1, "no more gettime()\n", strlen("no more gettime()\n"));
    // write(1, "This point shall never be reached if page fault is activated.", 61);
    while(1);
}
