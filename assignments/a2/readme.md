__How the program was written:__ <br />
Utilized /proc/stat to get the number of processes to know how many processes to store in array.

Made use of dirent.h to open and read directories, specifically the file descriptors for each PID stored
at /proc/PID/fd. Referencing https://www.group-ib.com/blog/linux-pro-manipulation/, /proc/PID/fd is a directory
containing symbolic links for each open file descriptor, thus we can use readlink https://pubs.opengroup.org/onlinepubs/7908799/xsh/readlink.html to read the contents of link and get file name.
Utilized https://man7.org/linux/man-pages/man5/proc_pid_fd.5.html to learn about what is returned by readlink.

__Functions:__ <br />




__How To Use Program:__ <br />
1. gcc myMonitoringTool.c -std=c99 -Wall -Werror -o myMonitoringTool <br />
2. ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T] <br />
__Notes__ <br />


