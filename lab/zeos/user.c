#include <libc.h>

char buff[24];

int pid = 0;
int mutex = 0;

/* ============== HEADERS ============= */
void trigger_page_fault();
void write_wrapper(char *msg);
void write_msg_n_num(char *msg, int num);
void print_stats(struct stats st);
/* ==================================== */

/* ========== IMPEMENTATIONS ========== */
void trigger_page_fault() {
    write_wrapper("Imma trigger a page fault");
    char *p = 0;
    *p = 0x69;
}

/**
 * Wrapper para simplificar las llamadas 
 * a write y su control de errores
 */
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
    char *separator = "========================================\n";
    write_wrapper(separator);
    write_msg_n_num("User ticks: ", st.user_ticks);
    write_msg_n_num("System ticks: ", st.system_ticks);
    write_msg_n_num("Blocked ticks: ", st.blocked_ticks);
    write_msg_n_num("Ready ticks: ", st.ready_ticks);
    write_msg_n_num("Elapsed total ticks: ", st.elapsed_total_ticks);
    write_msg_n_num("Total transitions: ", st.total_trans);
    write_msg_n_num("Remaining ticks: ", st.remaining_ticks);
    write_wrapper(separator);
}
/* ==================================== */

void func(int i) {
    // while (i < 100000000) {
    //     ++i;
    // }

    mutex_lock(&mutex);
    pid = 10;
    mutex_unlock(&mutex);

    write_wrapper("Func is over\n");
    exit_thread();
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

    write_wrapper("a");
    // Test scroll down
    write_wrapper("\[20;5;HHolaquetalestotendriaqueestaratomarporculo\[K\[K\[K");

    while(1);

    mutex_init(&mutex);

    int i = 0;
    create_thread((void*)func, &i);

    mutex_lock(&mutex);
    pid = 5;
    mutex_unlock(&mutex);

    // char b[4];
    // read(b, 4);
    //
    // write_wrapper(b);
    // write_wrapper("\n");
    //
    write_wrapper("is over\n");

    while(1);

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

    struct stats st1, st2, st3;

    while (gettime() < 1003);

    // Time 1
    write_msg_n_num("Time 1: ", gettime());
    write_msg_n_num("PID: ", getpid());
    get_stats(getpid(), &st1);
    print_stats(st1);

    while (gettime() < 2000);

    // Time 2
    write_msg_n_num("Time 2: ", gettime());
    write_msg_n_num("PID: ", getpid());
    get_stats(getpid(), &st2);
    print_stats(st2);

    while (gettime() < 3000);

    // Time 3
    write_msg_n_num("Time 3: ", gettime());
    write_msg_n_num("PID: ", getpid());
    get_stats(getpid(), &st3);
    print_stats(st3);

    // Trigger a page fault
    // trigger_page_fault();

    // This point shall never be reached, since the page
    // fault exception never returns
    write_wrapper("This point shall never be reached if page fault is activated.");
    for(;;);
}
