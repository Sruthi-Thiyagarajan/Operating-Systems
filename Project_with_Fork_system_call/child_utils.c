#include <stdio.h>
#include "child_utils.h"
#include "file_wrapper.h"

#define WRITE_END 1
#define READ_END 0
#define MAX_BUFF_SIZE 1256

child_process* build_children(int num_children) {
    child_process* pcp = (child_process*) calloc(num_children, sizeof(child_process));
    for(int i = 0; i < num_children; ++i) {
        pcp[i].child_number = i;
    }
    return pcp;

}

int create_pipes(int num_children, child_process* pChildren) {
    int pipe_status = 0;
    for (int i = 0; i < num_children; ++i) {
        pipe_status = pipe(pChildren[i].file_descriptor);
        if (pipe_status == -1) {
            return pipe_status;
        }
    }
    return pipe_status;
}

void non_terminal_child(child_process child_obj, int start_exec_time) {
    int fd = child_obj.file_descriptor[WRITE_END];
    int child = child_obj.child_number;

    int end_time = start_exec_time + 30;
    int n_messages = 1;
    int message_time=0;
    double message_millis=0.0;
    struct timeval tv;
    char* BUFF = calloc(MAX_BUFF_SIZE, sizeof(char));
    gettimeofday(&tv, NULL);

    //close term fds
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    printf("end time: %d\n", end_time);
    srand(time(NULL));
    //30 second loop

    while (end_time >= (int)tv.tv_sec) {
        //format message
        if (message_time >= 30) break;
	gettimeofday(&tv, NULL);
        message_time = (int) (tv.tv_sec - start_exec_time);
        message_millis = (double) (tv.tv_usec / 1000.00);

        snprintf(BUFF, MAX_BUFF_SIZE, "%i:%05.3f: Child %i message %i\n", message_time, message_millis, child, n_messages);
        write(fd, BUFF, strlen(BUFF));
        ++n_messages;

        //sleep for 0,2 s
        sleep(rand() % 3);
        //free(BUFF);

        //gettimeofday(&tv, NULL);
        //message_time = (int) (tv.tv_sec - start_exec_time);
        //message_millis = (double) (tv.tv_usec / 1000.00);
    }

    //exit message
    char exit_message[] = "EXIT_COND";
    write(fd, exit_message, strlen(exit_message)+1);
    close(fd);
    free(BUFF);
}

void terminal_child(child_process child_obj, int start_exec_time) {
    int fd = child_obj.file_descriptor[WRITE_END];
    int child = child_obj.child_number;

    int end_time = start_exec_time + 30;
    int n_messages = 1;
    char* BUFF = calloc(MAX_BUFF_SIZE, sizeof(char));
    char* user_input = calloc(MAX_BUFF_SIZE, sizeof(char));
    int message_time=0;
    double message_millis=0.0;
    struct timeval tv;
    char user_prompt[] = "Enter input: \n";
    bool new_prompt_needed = true;
    bool time_limit_hit = false;
    //pipe read
    fd_set fdsets;
    struct timeval timeout;
	// set the timeout value
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;

    gettimeofday(&tv, NULL);

    while(end_time >= (int)tv.tv_sec && !time_limit_hit) {
        gettimeofday(&tv, NULL);
        //setup terminal env
        FD_ZERO(&fdsets);
		FD_SET(STDIN_FILENO, &fdsets);
        //write user prompt
        if (new_prompt_needed) {
            write(STDOUT_FILENO, user_prompt, strlen(user_prompt));
            //monitor for input
            //error case
            new_prompt_needed = false;
        }
        int retval = select(12, &fdsets, NULL, NULL, &timeout);
        if(retval == -1) {
            printf("Select error");
        }
        //input case
        else if (retval) {
            //avoid non stdin inputs
            if (FD_ISSET(STDIN_FILENO, &fdsets)) {
                gettimeofday(&tv, NULL);
                local_flush_buff(user_input, MAX_BUFF_SIZE);
                read(STDOUT_FILENO, user_input, MAX_BUFF_SIZE);
                //format the message
                message_time = (int) (tv.tv_sec - start_exec_time);
                message_millis = (double) (tv.tv_usec / 1000.00);
                snprintf(BUFF, MAX_BUFF_SIZE, "%i:%05.3f: Child %i: %d message from terminal: %s", message_time, message_millis, child, n_messages, user_input);
                //write message
                write(fd, BUFF, strlen(BUFF));
                ++n_messages;
                gettimeofday(&tv, NULL);
                //we need a new prompt now
                new_prompt_needed = true;
            }
        }
        else {
            printf("\nTerminal timeout after 30 seconds\n");
            time_limit_hit = true;
        }
        gettimeofday(&tv, NULL);
    }

    //exit message
    char exit_message[] = "EXIT_COND";
    write(fd, exit_message, strlen(exit_message)+1);
    close(fd);
    free(BUFF);
    free(user_input);
}

bool read_pipe(child_process child_obj, int file_desc, fd_set* fdsets, int exec_start_time) {
    char read_msg[MAX_BUFF_SIZE];

    // first child's pipe
	if(FD_ISSET(child_obj.file_descriptor[READ_END], fdsets)) {
	    int val = read_by_line(child_obj.file_descriptor[READ_END], read_msg, MAX_BUFF_SIZE);
	    // check to see if the pipe has been closed by the child
		if(0 == strcmp("EXIT_COND", read_msg)) {
            //close the pipe
			return false;
	    }
		// otherwise write the data to the file
		else {
			if('\0' != read_msg[0]) {
				parent_timestamp(file_desc, exec_start_time);
				write_to_file(file_desc, read_msg, val);
			}
		}
	}
    return true;
}
