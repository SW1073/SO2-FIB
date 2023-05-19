#include <io.h>
#include <utils.h>
#include <list.h>
#include <devices.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

const char command_starter = '\[';

int sys_write_console(char *buffer,int size)
{
    int i,          // Pointer to the character being processed
        init_i;     // Pointer to the beginning of the command
    int chars_written = 0; // Number of characters written to the screen

    for (i=0; i<size; i++) {
        if (buffer[i] == command_starter) {
            // ====== A command has started ======
            init_i = i;
            while (i < size && !is_letter(buffer[i])) {
                i++; // Advance the pointer until the end of a command or the end of the buffer
            }

            if (is_letter(buffer[i])) {
                // We have a complete command
                if (execute_command(buffer[i], &buffer[init_i+1], i-init_i-2) < 0) {
                    for (; init_i <= i; ++init_i) {
                        printc(buffer[init_i]);
                        chars_written++;
                    }
                }
            }
            else {
                for (; init_i <= i; ++init_i) {
                    printc(buffer[init_i]);
                    chars_written++;
                }
            }
        }
        else {
            printc(buffer[i]);
            chars_written++;
        }
    }

    return chars_written;
}

int execute_command (char op, char *args, int size) {
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
            // TODO: parse 2 values out of args
            // TODO: set x and y
            break;

        // Change screen attributes
        case 'm':
            // TODO: parse all numbers out of args
            // TODO: set the values as some kind of default somewhere
            // IDEA ^^^: Es poden fer els colors globals i que tots els printc siguin printc_color
            break;
        
        // Command not implemented
        default:
            return -1;
    }
    // Command was executed successfully
    return 0;
}
