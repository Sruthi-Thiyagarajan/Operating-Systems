#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include "child_utils.h"
#include "file_wrapper.h"

#define N_CHILDREN 5
#define N_FILE_DESCRIPTORS 2 //Read and write
#define MAX_BUFF_SIZE 1256
#define WRITE_END 1
#define READ_END 0

int main(int argc, char* argv[]) {
    int pipe_status = 0;
    int exec_start_time;
    struct timeval tv;

    //build children and create pipes for them
    child_process* pChildren = build_children(N_CHILDREN);
    pipe_status = create_pipes(N_CHILDREN, pChildren);
    if(pipe_status == -1) {
        printf("FATAL ERROR: PIPE CREATION FAILED\n");
        return 1;
    }

    //Start the fork process to create subprocesses
    //get base time of program execution
    gettimeofday(&tv, NULL);
    exec_start_time = tv.tv_sec; 
    for(int i = 0; i < N_CHILDREN; ++i) {

        pChildren[i].pid = fork();
        //parent case
        if (pChildren[i].pid  > 0) {
            close(pChildren[i].file_descriptor[WRITE_END]);
        }
        //Child case
        else if (pChildren[i].pid  ==  0) {
            close(pChildren[i].file_descriptor[READ_END]);
            if (i == 4) {
                terminal_child(pChildren[i], exec_start_time);
            }
            else {
                non_terminal_child(pChildren[i], exec_start_time);
            }
            //Child is done work
            exit(0);
        }
        //Fatal error, fork failed
        else {
            printf("fork returned: %d\n", pChildren[i].pid);
            printf("FATAL ERROR: FORK FAILURE\n");
            return 1;
        }
    }

    // read from all of the pipes and write to a file
	fd_set fdsets;
	struct timeval timeout;
    // file open
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int file_desc = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, mode);
	if(file_desc < 0) {
		printf("FATAL ERROR: Could not open output.txt");
		return 1;
	}


	// loop as long as a pipe is open
    bool open_pipes[N_CHILDREN];
    for (int i = 0; i < N_CHILDREN; ++i) {
        open_pipes[i] = true;
    }
    bool pipe_open = true;
	while(pipe_open) {
		// set the timeout value
		timeout.tv_sec = 2;
		timeout.tv_usec = 500000;
		// ensure that fds is zeroed
		FD_ZERO(&fdsets);
		// set all of the file descriptors
		for(int i = 0; i < N_CHILDREN; i++) {
			FD_SET(pChildren[i].file_descriptor[READ_END], &fdsets);
		}

		int retval = select(12, &fdsets, NULL, NULL, &timeout);
		if(-1 == retval) {
			perror("select()");
		}
		else if(retval) {
            for(int i = 0; i < N_CHILDREN; ++i) {
                if (open_pipes[i]) {
                    open_pipes[i] = read_pipe(pChildren[i], file_desc, &fdsets, exec_start_time);
                }
            }
        }
        pipe_open = false;
        for(int i = 0; i <N_CHILDREN; ++i) {
            if (open_pipes[i] == true) {
                pipe_open = true;
            }
        }
    }

    //close pipes
	for(int i = 0; i < N_CHILDREN; i++) {
		close(pChildren[i].file_descriptor[READ_END]);
	}

	// close file descriptor
	close(file_desc);

	return 0;




}
