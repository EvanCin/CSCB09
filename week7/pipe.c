#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int reading_fds[argc-1];
    char buf;
    for(int i = 0; i < argc-1; i++) {
        int pipefd[2];
        if(pipe(pipefd) == -1) {
            perror("pipe");
            exit(1);
        }
        
        int result = fork();
        if(result < 0) {
            perror("fork");
            exit(1);
        } else if(result == 0) {
            close(pipefd[0]); //close reading

            for(int j = 0; j < i; j++) {
                close(reading_fds[j]); //close reading of all other pipes
            }

            int len = strlen(argv[i+1]);
            if(write(pipefd[1], &len, sizeof(len)) == -1) { //write to pipe
                perror("write");
                exit(1);
            }
            close(pipefd[1]); //finished writing
            exit(0);
        } else {
            close(pipefd[1]);
            reading_fds[i] = pipefd[0];
        }
    }
    int sum = 0;

    for(int i = 0; i < argc-1; i++) {
        wait(NULL);
    }
    for(int i = 0; i < argc-1; i++) {
        int len;
        if(read(reading_fds[i], &len, sizeof(len)) == -1) {
            perror("read");
            exit(1);
        }
        sum += len;
        printf("current length: %d\n", len);
    }

    printf("The length of all the args is %d\n", sum);
    return 0;
}