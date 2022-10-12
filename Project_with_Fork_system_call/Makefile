# set the compiler
CC = gcc
# set the flags
CFLAGS = -Wall -g3
# specify which libraries to use
LDFLAGS =
#set the object files
OBJFILES = main.o child_utils.o file_wrapper.o
# specify the name of the desired output file
TARGET = multiplexer

all: $(TARGET)

#make the files
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

# clean the files from directory
clean:
	rm -f $(TARGET) *.o
