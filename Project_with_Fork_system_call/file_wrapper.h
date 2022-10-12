#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void local_flush_buff(char* msg, int size);
void parent_timestamp(int fd, int baseTime);
int read_by_line(int fd, char* msg, int size);
void write_to_file(int fd, char* msg, int size);

#endif
