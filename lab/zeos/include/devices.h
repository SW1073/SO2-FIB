#ifndef DEVICES_H__
#define  DEVICES_H__

extern struct list_head blocked;

int sys_write_console(char *buffer,int size);
int execute_command (char op, int *params, int n_params);
int atoi_n(char *args, int n);
void set_attribute(int a);
#endif /* DEVICES_H__*/
