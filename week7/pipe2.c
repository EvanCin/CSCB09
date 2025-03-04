#include <unistd.h>

int main() {
    int fd[2];
    char buf[3];
    char* msg = "hi";

    if(pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }
    pid_t pid = fork();
    if(pid == 0) {
        close(fd[0]);
        write(fd[1], msg, 3);
        close(fd[1]);
        exit(0);
    } else {
        close(fd[1]);
        while(read(fd[0], buf, 3) > 0) {
            write(STDOUT_FILENO, buf, 3);
        }

        close(fd[0]);
    }
    return 0;
}