CC = gcc
CCFLAGS = -std=c99 -Wall -Werror -D_POSIX_C_SOURCE=200809L

all: myMonitoringTool

myMonitoringTool: computations.o display.o processControl.o signals.o main.o
	${CC} ${CCFLAGS} -o myMonitoringTool $^

main.o: main.c
	${CC} ${CCFLAGS} -c main.c

%.o: %.c
	${CC} ${CCFLAGS} -c $<

.PHONY: clean
clean:
	rm *.o myMonitoringTool