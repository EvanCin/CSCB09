__README -- Recreating the System-Wide FD Tables__ <br />
Author: Evan Chen <br />
Date: March 7, 2025 <br />

__ABOUT__ <br />
This project is about displaying various information about processes and file descriptors in the file systems, specifically under /proc/pid/fd.

__Approach to the problem__ <br />
1. I investigated /proc/stat and utilized /proc/stat to get the number of processes to know how many processes to iterate over and check if they exist.
2. Each process that is running exists in /proc/pid and through that I was able to find the open file descriptors for each process through /proc/pid/fd.
3. Made use of dirent.h to open and read directories, specifically the file descriptors for each PID stored
at /proc/PID/fd.
4. Referencing https://www.group-ib.com/blog/linux-pro-manipulation/, /proc/PID/fd is a directory
containing symbolic links for each open file descriptor, thus we can use readlink https://pubs.opengroup.org/onlinepubs/7908799/xsh/readlink.html to read the contents of link and get file name.
Utilized https://man7.org/linux/man-pages/man5/proc_pid_fd.5.html to learn about what is returned by readlink.
5. Utilized stat structure and made calls to stat() to get file descriptor info, particularly for getting inode at st_ino in stat struct.

__Implementation__ <br />
1. For implementation, I separated the code into sections that performed getting, checking, updating, and displaying.
   I had the main function check the input arguments and feed them into an update function that updated variables
   which decided which tables to display. Then I fed those variables into a display function that called the display function
   corresponding to the variable. I created a display function for each type of table display and had them go through the pids
   in proc to fetch the relevant information.
2. I have one module called systemWideFDTables.c that contains the code for the whole implementation.
3. __getNumProcesses()__ returns the number of active processes from /proc/stat. It opens the /proc/stat file and reads
   until it gets to the process line and retrieves the process number. <br />
   __displayCompositeTable(int numProcesses, int pid)__ displays the table corresponding to the user arg --composite. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. It uses          readlink() from unistd.h and stat() from sys/stat.h to get inode and filename. <br />
   __displayPerProcessTable(int numProcesses, int pid)__ displays the table corresponding to the user arg --per-process. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. <br />
   __displaySystemWideTable(int numProcesses, int pid)__ displays the table corresponding to the user arg --systemWide. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. It uses          readlink() from unistd.h to get filename. <br />
   __displayVnodesTable(int numProcesses, int pid)__ displays the table corresponding to the user arg --Vnodes. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. It uses          stat() from sys/stat.h to get inode. <br />

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
When compiling with std=c99, readlink gives an error saying implicit declaration, so after looking at https://man7.org/linux/man-pages/man2/readlink.2.html, I found that readlink() requires _POSIX_C_SOURCE >= 200112L and after researching I found https://stackoverflow.com/questions/66862654/why-does-my-compiler-think-my-readlink-is-implicitly-declared-if-i-set-the-sta where I found a POSIX c source which worked for compiling so I included -D_POSIX_C_SOURCE=200809L in my makefile since 2008 >= 2001
