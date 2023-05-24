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
    // char b[4];
    // read(b, 4);

    // write_wrapper(b);
    // write_wrapper("\n");

    mutex_lock(&mutex);
    pid = 10;
    mutex_unlock(&mutex);

    write_wrapper("Func is over\n");
    exit_thread();
}

#define HEIGHT 21
#define WIDTH 80

char level1[HEIGHT * WIDTH / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc,
    0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00,
    0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00,
    0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0,
    0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03,
    0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c,
    0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00,
    0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xff, 0xff, 0xf8, 0x3c, 0x00, 0x00,
    0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f,
    0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03,
    0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

#define FORMAT_BUFFER_SIZE 12
char format_buffer[FORMAT_BUFFER_SIZE] = "\[00;03H\[46m ";

void draw_ij(char c, unsigned char i, unsigned char j, unsigned char color) {
    format_buffer[1] = j / 10 + '0';
    format_buffer[2] = j % 10 + '0';
    format_buffer[4] = i / 10 + '0';
    format_buffer[5] = i % 10 + '0';
    format_buffer[9] = color + '0';
    format_buffer[FORMAT_BUFFER_SIZE - 1] = c;
    write(1, format_buffer, FORMAT_BUFFER_SIZE);
}

void draw_xy(char c, unsigned char x, unsigned char y, unsigned char color) {
    format_buffer[1] = x / 10 + '0';
    format_buffer[2] = x % 10 + '0';
    format_buffer[4] = y / 10 + '0';
    format_buffer[5] = y % 10 + '0';
    format_buffer[9] = color + '0';
    format_buffer[FORMAT_BUFFER_SIZE - 1] = c;
    write(1, format_buffer, FORMAT_BUFFER_SIZE);
}

#define MAP_X_OFFSET 0
#define MAP_Y_OFFSET 3

void draw_map(char *map_bmp) {
    unsigned char mask, jj, color = 0;
    for (int i = 0; i < HEIGHT; i++) {
        jj = 0;
        for (int j = 0; j < WIDTH/8; j++) {
            mask = 0x80;
            while (mask != 0) {
                if (!(map_bmp[i*(WIDTH/8) + j] & mask))
                    // Draw a wall
                    color = 6;
                else
                    // Draw a space
                    color = 0;
                draw_ij(' ', i+MAP_Y_OFFSET, jj+MAP_X_OFFSET, color);
                mask >>= 1;
                jj++;
            }
        }
    }
}

    int __attribute__ ((__section__(".text.main")))
main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */    

    write_wrapper("\[0;1H Score: ");
    write_wrapper("\[9;1H   69");

    draw_map(level1);

    for(;;);
}
