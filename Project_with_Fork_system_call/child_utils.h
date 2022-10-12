#ifndef CHILD_UTILS_H
#define CHILD_UTILS_H

#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct _child_process {
    int file_descriptor[2];
    pid_t pid;
    int child_number;
} child_process;

child_process* build_children(int num_children);
int create_pipes(int num_children, child_process* pChildren);
void non_terminal_child(child_process child, int start_exec_time);
void terminal_child(child_process child_obj, int start_exec_time);
bool read_pipe(child_process child_obj, int file_desc, fd_set* fdsets, int exec_start_time);




#endif
