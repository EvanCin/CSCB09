__README -- System Monitoring Tool -- Concurrency and Signals__ <br />
Author: Evan Chen <br />
Date: March 23, 2025 <br />

Rough notes for assignment <br />



__ABOUT__ <br />
This project is about extending A1 System Monitoring Tool to allow cpu, memory, and cores information to be processed concurrently
within different processes. Terminate and stop signals from the keyboard are processed differently to match the use case of the program.

__Approach to the problem__ <br />
1. I split the program up into 3 parts: Main (for processing command line input and calling the display functions), <br />
Computations (for retrieving and calculating the values for various components), <br />
IO (for using pipes to pass values from Computations between parent and child processes, and displaying info)

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
   __displaySummaryTable(int numProcesses)__ displays the table corresponding to the user arg --summary. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. It        
   sums the number of file descriptors open per process and prints it. <br />
   __displayThresholdTable(int numProcesses, int thresholdVal)__ displays the table corresponding to the user arg --threshold=X. It opens
   the directory /proc/pid/fd using functions from dirent.h and goes through all the pids and their relevant file descriptors. It        
   sums the number of file descriptors open per process and prints it if the sum is greater than or equal to thresholdVal. <br />
   __isNumber(const char* str)__ returns true if str is a number and false otherwise. The function loops through each character of str
   and calls on isdigit() from ctype.h to check if character is a digit. <br />
   __isAccessiblePid(int pid)__ returns true if pid is accessible and false otherwise. The function attempts to open the file descriptor
   directory for pid at /proc/pid/fd using functions from dirent.h. <br />
   __updateArg(char* arg, bool* dComposite, bool* dSystemWide, bool* dVnodes, <br />
		bool* dProcess, bool* dSummary, bool* dThreshold, int* thresholdVal)__ <br />
   updates the booleans and values for the corresponding values based on the user argument arg. <br />
   __displayTables(bool dComposite, bool dSystemWide, bool dVnodes, <br />
      bool dProcess, bool dSummary, bool dThreshold, int thresholdVal, int pid)__ <br />
   displays the tables based on boolean values by calling the display function corresponding to the boolean values. <br />
   __main(int argc, char** argv)__ <br />
   checks user arguments, calls updateArg to update boolean values and prints the tables with a call to displayTables. <br />

__Flow Chart Diagram__ <br />
![flow chart diagram](https://github.com/user-attachments/assets/a6032db5-2fbc-4034-8f6b-cd91cdf866bf)

__How To Use Program:__ <br />
1. To compile code: make <br />
2. makefile uses ccflags -std=c99, -Wall, -Werror, -D_POSIX_C_SOURCE=200809L (for readlink, more details in Notes section) <br />
3. To run: ./showFDtables [pid] [--per-process] [--systemWide] [--Vnodes] [--composite] [--summary] [--threshold=X] <br />

__Expected Results__ <br />
./showFDtables displays the composite table for all pids <br />
./showFDtables [pid] displays the composite table for pid <br />
./showFDtables --summary displays the summary table for all pids <br />
./showFDtables [pid] --summary displays the composite table for pid and displays the summary table for all pids <br />
./showFDtables --threshold=X displays the threshold table for all pids with amount of fds >= X <br />
./showFDtables [pid] --threshold=X displays the composite table for pid and displays the threshold table for all pids with amount of fds >= X <br />
./showFDtables --per-process --systemWide --Vnodes --composite displays the per process, system wide, vnodes, and composite tables for all pids <br />
./showFDtables [pid] --per-process --systemWide --Vnodes --composite displays the per process, system wide, vnodes, and composite tables for only pid <br />

__Test Cases__ <br />
Running with invalid/inaccessible pid gives the error message: FD directory of PID [pid] is not accessible <br />
Running with invalid arguments gives error message: Invalid argument, input should be of form: ./showFDtables [pid] [--per-process] [--systemWide] [--Vnodes] [--composite] [--summary] [--threshold=X] <br />

__Notes__ <br />
PID argument is a positional argument and should be the first arg if used <br />
If no arguments are given then the output is the composite table <br />
When compiling with std=c99, readlink gives an error saying implicit declaration, so after looking at https://man7.org/linux/man-pages/man2/readlink.2.html, I found that readlink() requires _POSIX_C_SOURCE >= 200112L and after researching I found https://stackoverflow.com/questions/66862654/why-does-my-compiler-think-my-readlink-is-implicitly-declared-if-i-set-the-sta where I found a POSIX c source which worked for compiling so I included -D_POSIX_C_SOURCE=200809L in my makefile since 2008 >= 2001 <br />
I found the maximum path length through https://stackoverflow.com/questions/9449241/where-is-path-max-defined-in-linux and
defined a constant variable PATH_MAX to be 4096 <br />

__References__ <br />
https://man7.org/linux/man-pages/man5/proc.5.html <br />
https://man7.org/linux/man-pages/man0/sys_types.h.0p.html <br />
https://www.man7.org/linux/man-pages/man0/sys_stat.h.0p.html <br />
https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html <br />
https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html <br />
https://www.group-ib.com/blog/linux-pro-manipulation/ <br />
https://pubs.opengroup.org/onlinepubs/7908799/xsh/readlink.html <br />
https://man7.org/linux/man-pages/man2/readlink.2.html <br />
https://stackoverflow.com/questions/66862654/why-does-my-compiler-think-my-readlink-is-implicitly-declared-if-i-set-the-sta <br />
https://stackoverflow.com/questions/9449241/where-is-path-max-defined-in-linux <br />
