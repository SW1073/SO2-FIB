#include <io.h>
#include <utils.h>
#include <list.h>
#include <devices.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

int params[3];

const char command_starter = '\[';
const char command_argument_separator = ';';

Byte bg_color;
Byte fg_color;
Byte blink;

int sys_write_console(char *buffer, int size) {
    int i,          // Pointer to the character being processed
        init_i,     // Pointer to the beginning of the command
        chars_written = 0; // Number of characters written to the screen
    bg_color = DEFAULT_BG_COLOR;
    fg_color = DEFAULT_FG_COLOR;
    blink = DEFAULT_BLINK;

    for (i=0; i<size; i++) {
        if (buffer[i] == command_starter) {
            // A command has started
            init_i = i;
            while (i < size && !is_letter(buffer[i])) {
                i++; // Advance the pointer until the end of a command or the end of the buffer
            }

            if (is_letter(buffer[i])) {
                // We have a complete command
                if (execute_command(buffer[i], &buffer[init_i+1], i-init_i-1) < 0) {
                    for (; init_i <= i; ++init_i) {
                        printc_color(buffer[init_i], fg_color, bg_color, blink);
                        chars_written++;
                    }
                }
            }
            else {
                for (; init_i <= i; ++init_i) {
                    printc_color(buffer[init_i], fg_color, bg_color, blink);
                    chars_written++;
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

int execute_command (char op, char *args, int args_size) {
    int n_params= parse_params(args, args_size);
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

/**
 * Parse parameters and return the number of parameters parsed or -1 if there was an error
 */
int parse_params(char* args, int args_size) {
    int i = 0,
        init_i = i,
        num_params = 0;

    // Necessary case to avoid errors
    if (args_size == 0) return 0;

    while (i < args_size) {
        if (args[i] == command_argument_separator) {
            // We have a parameter
            if (i > init_i && num_params < 3) {
                params[num_params] = atoi_n(&args[init_i], i-init_i);
                num_params++;
            }
            else {
                // Too many parameters
                return -1;
            }
            init_i = i;
        }
        i++;
    }

    if (i > init_i && num_params < 3) {
        params[num_params] = atoi_n(&args[init_i], i-init_i);
        num_params++;
    }
    else return -1;
    
    return num_params;
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
            break;
            // case 1: // Bright
            //     break;
            // case 2: // Dim
            //     break;
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
        case 30: // Black
            fg_color = BLACK;
            break;
        case 31: // Red
            fg_color = RED;
            break;
        case 32: // Green
            fg_color = GREEN;
            break;
        case 33: // Yellow
            fg_color = YELLOW;
            break;
        case 34: // Blue
            fg_color = BLUE;
            break;
        case 35: // Magenta
            fg_color = MAGENTA;
            break;
        case 36: // Cyan
            fg_color = CYAN;
            break;
        case 37: // White
            fg_color = WHITE;
            break;

            // Foreground colors
        case 40: // BLACK
            bg_color = BLACK;
            break;
        case 41: // RED
            bg_color = RED;
            break;
        case 42: // GREEN
            bg_color = GREEN;
            break;
        case 43: // YELLOW
            bg_color = YELLOW;
            break;
        case 44: // BLUE
            bg_color = BLUE;
            break;
        case 45: // MAGENTA
            bg_color = MAGENTA;
            break;
        case 46: // CYAN
            bg_color = CYAN;
            break;
        case 47: // WHITE
            bg_color = WHITE;
            break;
    }

}
