CC = gcc
CFLAGS = -Wall -Werror -g -std=c99

myprog: file1.o file2.o
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
.PHONY: clean
clean:
	rm *.o