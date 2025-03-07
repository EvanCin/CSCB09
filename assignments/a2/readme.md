__README -- Recreating the System-Wide FD Tables__ <br />
Author: Evan Chen
Date: March 7, 2025

__ABOUT__ <br />
This project is about displaying various information about processes and file descriptors in the file systems, specifically under /proc/pid/fd.

__Approach to the problem__ <br />
1. I investigated /proc/stat and utilized /proc/stat to get the number of processes to know how many processes to iterate over and check if they exist.
2. Each process that is running exists in /proc/pid and through that I was able to find the open file descriptors for each process through /proc/pid/fd
3. Made use of dirent.h to open and read directories, specifically the file descriptors for each PID stored
at /proc/PID/fd.
4. Referencing https://www.group-ib.com/blog/linux-pro-manipulation/, /proc/PID/fd is a directory
containing symbolic links for each open file descriptor, thus we can use readlink https://pubs.opengroup.org/onlinepubs/7908799/xsh/readlink.html to read the contents of link and get file name.
Utilized https://man7.org/linux/man-pages/man5/proc_pid_fd.5.html to learn about what is returned by readlink.
5. 



__How the program was written:__ <br />
Utilized /proc/stat to get the number of processes to know how many processes to store in array.

 

PATH_MAX is utilized for specifying maximum length of path
Linux has a limit of 1024 fd per process https://docs.oracle.com/cd/E19476-01/821-0505/file-descriptor-requirements.html#:~:text=Linux%20systems%20limit%20the%20number,worker%20threads%20will%20be%20blocked.

__Functions:__ <br />




__How To Use Program:__ <br />
1. gcc myMonitoringTool.c -std=c99 -Wall -Werror -o myMonitoringTool <br />
2. ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T] <br />
__Notes__ <br />
PID argument is a positional argument and should be the first arg if used
If no arguments are given then the output is the composite table

