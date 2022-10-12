#include "file_wrapper.h"

#define MAX_BUFF_SIZE 1256

void local_flush_buff(char* msg, int size) {
	for(int i = 0; i < size; i++) {
		msg[i] = '\0';
	}
}

// function to write the parent timestamp to the output message
void parent_timestamp(int fd, int baseTime) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	int message_time = (int)(tv.tv_sec - baseTime);
	double message_millis = (double)(tv.tv_usec/1000.00);

	char* BUFF = calloc(MAX_BUFF_SIZE, sizeof(char));
	// format the message
	snprintf(BUFF, 32, "%i:%05.3f: Parent - ", message_time,message_millis);
	// write the message to the output file
	write_to_file(fd, BUFF, strlen(BUFF));

    free(BUFF);
}

//reads file line by line
int read_by_line(int fd, char* msg, int size) {
	int value_read = 0;
	// clear the buffer prior to reading
	local_flush_buff(msg, size);

	// read no more than the size of the char array
	for(int i = 0; i < size; i++) {
		char c;
		read(fd, &c, 1);
		// if the read character is not NULL add it to the message
		if('\n' != c) {
			msg[i] = c;
			value_read++;
		}
		else {
			msg[i] = c;
			value_read++;
			break;
		}
	}
	return value_read;
}

//write to file
void write_to_file(int fd, char* msg, int size) {
	for(int i = 0; i < size; i++) {
		if('\0' == msg[i]) {
			break;
		}
		else {
			write(fd, &msg[i], 1);
		}
	}
}
