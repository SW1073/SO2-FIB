#ifndef DEVICES_H__
#define  DEVICES_H__

extern struct list_head blocked;

int sys_write_console(char *buffer,int size);
int execute_command (char op, char *args, int args_size);
int parse_params(char* args, int args_size);
int atoi_n(char *args, int n);
#endif /* DEVICES_H__*/
