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

    // write_wrapper("\[H\[5;34;45mJola\[05;30;47mJola");

    // 20x60
    write_wrapper("\[10;2H\[2;46;8m                                                             ");
    write_wrapper("\[10;3H\[46;8m ");write_wrapper("\[70;3H\[46;8m ");
    write_wrapper("\[10;4H\[46;8m ");write_wrapper("\[70;4H\[46;8m ");
    write_wrapper("\[10;5H\[46;8m ");write_wrapper("\[70;5H\[46;8m ");
    write_wrapper("\[10;6H\[46;8m ");write_wrapper("\[70;6H\[46;8m ");
    write_wrapper("\[10;7H\[46;8m ");write_wrapper("\[70;7H\[46;8m ");
    write_wrapper("\[10;8H\[46;8m ");write_wrapper("\[70;8H\[46;8m ");
    write_wrapper("\[10;9H\[46;8m ");write_wrapper("\[70;9H\[46;8m ");
    write_wrapper("\[10;10H\[46;8m ");write_wrapper("\[70;10H\[46;8m ");
    write_wrapper("\[10;11H\[46;8m ");write_wrapper("\[70;11H\[46;8m ");
    write_wrapper("\[10;12H\[46;8m ");write_wrapper("\[70;12H\[46;8m ");
    write_wrapper("\[10;13H\[46;8m ");write_wrapper("\[70;13H\[46;8m ");
    write_wrapper("\[10;14H\[46;8m ");write_wrapper("\[70;14H\[46;8m ");
    write_wrapper("\[10;15H\[46;8m ");write_wrapper("\[70;15H\[46;8m ");
    write_wrapper("\[10;16H\[46;8m ");write_wrapper("\[70;16H\[46;8m ");
    write_wrapper("\[10;17H\[46;8m ");write_wrapper("\[70;17H\[46;8m ");
    write_wrapper("\[10;18H\[46;8m ");write_wrapper("\[70;18H\[46;8m ");
    write_wrapper("\[10;19H\[46;8m ");write_wrapper("\[70;19H\[46;8m ");
    write_wrapper("\[10;20H\[46;8m ");write_wrapper("\[70;20H\[46;8m ");
    write_wrapper("\[10;21H\[46;8m ");write_wrapper("\[70;21H\[46;8m ");
    write_wrapper("\[10;22H\[2;46;8m                                                             ");

    write_wrapper("\[0;0H\[2;44m Soy dim");
    write_wrapper("\[0;1H\[1;44m Soy light");

    for(;;);
}
