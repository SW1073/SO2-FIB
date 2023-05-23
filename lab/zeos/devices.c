#include <io.h>
#include <utils.h>
#include <list.h>
#include <devices.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

#define MAX_PARAMS 3

const char command_starter = '\[';
const char command_argument_separator = ';';

Byte bg_color;
Byte fg_color;
Byte blink;
Byte bright;

int sys_write_console(char *buffer, int size) {
    int i,          // Pointer to the character being processed
        init_param, // Pointer to the beginning of the parameter
        init_i,     // Pointer to the beginning of the command
        chars_written = 0, // Number of characters written to the screen
        params[MAX_PARAMS],
        n_params;

    bg_color = DEFAULT_BG_COLOR;
    fg_color = DEFAULT_FG_COLOR;
    blink = DEFAULT_BLINK;
    bright = DEFAULT_BRIGHT;

    for (i=0; i<size; i++) {
        if (buffer[i] == command_starter) {
            // A command has started
            init_i = init_param = ++i;
            n_params = 0;
            while (i < size) {
                if (is_number(buffer[i])) {
                    ++i;
                }
                else if (buffer[i] == command_argument_separator) {
                    if (i > init_param && n_params < MAX_PARAMS) {
                        params[n_params] = atoi_n(&buffer[init_param], i-init_param);
                        n_params++;
                        init_param = i+1;
                    }
                    ++i;
                }
                else if (is_letter(buffer[i])) {
                    // We have a complete command
                    // Parse the last element if needed to
                    if (i > init_param && n_params < MAX_PARAMS) {
                        params[n_params] = atoi_n(&buffer[init_param], i-init_param);
                        n_params++;
                    }
                    // (Try to) execute the command
                    if (execute_command(buffer[i], params, n_params) < 0) {
                        for (; init_i <= i; ++init_i)
                            printc_color(buffer[init_i], fg_color, bg_color, blink);
                        chars_written += i - init_i + 1;
                    }
                    break;
                }
                else {
                    // Invalid character
                    for (; init_i <= i; ++init_i)
                        printc_color(buffer[init_i], fg_color, bg_color, blink);
                    chars_written += i - init_i + 1;
                    break;
                }
            }
        }
        else {
            printc_color(buffer[i], fg_color, bg_color, blink);
            chars_written++;
        }
    }

    return chars_written;
}

int execute_command (char op, int *params, int n_params) {
    switch (op) {
        // Scroll down screen
        case 'D':
            scroll_screen();
            break;

        // Erase character
        case 'K':
            erase_current_char();
            break;
        
        // Change cursor position
        case 'H':
        case 'f':
            if (n_params < 2) {
                if (n_params == 0) {// Return to home cursor position
                    set_cursor(0, 0);
                    break;
                }
                else // Insuficient valid arguments
                    return -1;
            }
            set_cursor(params[0], params[1]);
            break;

        // Change screen attributes
        case 'm':
            if (n_params < 1 || n_params > 3) return -1;
            for (int i = 0; i < n_params; ++i)
                set_attribute(params[i]);
            break;
        
        // Command not implemented
        default:
            return -1;
    }

    // Command was executed successfully
    return 0;
}

int atoi_n(char *args, int n) {
    int i, num = 0;
    for (i = 0; i < n; ++i) {
        if (is_number(args[i]))
            num = num*10 + (args[i] - '0');
    }
    return num;
}

void set_attribute(int a) {
    switch (a) {
        // Attributes
        case 0: // Reset attributes
            fg_color = DEFAULT_FG_COLOR;
            bg_color = DEFAULT_BG_COLOR;
            blink = DEFAULT_BLINK;
            bright = DEFAULT_BRIGHT;
            break;
        case 1: // Bright
            bright = 1;
            break;
        case 2: // Dim
            bright = 0;
            break;
        case 5: // Blink
            blink = !blink;
            break;
        case 7: // Reverse
            fg_color = DEFAULT_BG_COLOR;
            bg_color = DEFAULT_FG_COLOR;
            break;
        case 8: // Hidden
            fg_color = DEFAULT_BG_COLOR;
            break;

        // Background colors
        case 30: // BLACK
            if (bright) fg_color = DARK_GRAY;
            else fg_color = BLACK;
            break;
        case 31: // RED
            if (bright) fg_color = LIGHT_RED;
            else fg_color = RED;
            break;
        case 32: // GREEN
            if (bright) fg_color = LIGHT_GREEN;
            else fg_color = GREEN;
            break;
        case 33: // YELLOW
            if (bright) fg_color = YELLOW;
            else fg_color = BROWN;
            break;
        case 34: // BLUE
            if (bright) fg_color = LIGHT_BLUE;
            else fg_color = BLUE;
            break;
        case 35: // MAGENTA
            if (bright) fg_color = LIGHT_MAGENTA;
            else MAGENTA;
            break;
        case 36: // CYAN
            if (bright) fg_color = LIGHT_CYAN;
            else fg_color = CYAN;
            break;
        case 37: // WHITE
            if (bright) fg_color = WHITE;
            else fg_color = LIGHT_GRAY;
            break;

        // Foreground colors
        case 40: // BLACK
            if (bright) bg_color = DARK_GRAY;
            else bg_color = BLACK;
            break;
        case 41: // RED
            if (bright) bg_color = LIGHT_RED;
            else bg_color = RED;
            break;
        case 42: // GREEN
            if (bright) bg_color = LIGHT_GREEN;
            else bg_color = GREEN;
            break;
        case 43: // YELLOW
            if (bright) bg_color = YELLOW;
            else bg_color = BROWN;
            break;
        case 44: // BLUE
            if (bright) bg_color = LIGHT_BLUE;
            else bg_color = BLUE;
            break;
        case 45: // MAGENTA
            if (bright) bg_color = LIGHT_MAGENTA;
            else bg_color = MAGENTA;
            break;
        case 46: // CYAN
            if (bright == 1) bg_color = LIGHT_CYAN;
            else bg_color = CYAN;
            break;
        case 47: // WHITE
            if (bright) bg_color = LIGHT_GRAY;
            else bg_color = WHITE;
            break;
    }

}
