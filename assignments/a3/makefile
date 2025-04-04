CC = gcc
CCFLAGS = -std=c99 -Wall -Werror -D_POSIX_C_SOURCE=200809L


all: myMonitoringTool

myMonitoringTool: computations.o display.o processControl.o signals.o test.o
	${CC} ${CCFLAGS} -o myMonitoringTool $^

signals.o: signals.c signals.h
	${CC} -std=c99 -Wall -Werror -c signals.c

display.o: display.c display.h
	${CC} ${CCFLAGS} -c display.c

computations.o: computations.c computations.h
	${CC} ${CCFLAGS} -c computations.c

processControl.o: processControl.c processControl.h
	${CC} ${CCFLAGS} -c processControl.c

# %.o: %.c
# 	${CC} ${CCFLAGS} -c $<

.PHONY: clean
clean:
	rm *.o myMonitoringTool