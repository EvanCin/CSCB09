__README -- System Monitoring Tool -- Concurrency and Signals__ <br />
Author: Evan Chen <br />
Date: April 5, 2025 <br />

__ABOUT__ <br />
This project is about extending A1 System Monitoring Tool to allow cpu, memory, cores, and max frequency information to be processed concurrently within different processes. Terminate and stop signals from the keyboard are processed differently to match the use case of the program.

__Approach to the problem__ <br />
1. I split the program up into 5 parts: Main (for processing command line input and calling the display functions), <br />
Computations (for retrieving and calculating the values for various components), <br />
Display (for displaying the memory and cpu graphs, and cores and max frequency info), <br />
Process Control (for using pipes to pass values between parent and child processes, and displaying computed info) <br />
Signals (for handling various signals) <br />
2. I wrote a function createProcessesAndPipes to create four pipes, one for each process, and then made four fork calls so that I am able to calculate memory, cpu, core, and max freq info concurrently. I have my child processes close irrelevant pipes and the read end of the pipe in use and write the required info to the pipe. After finishing writing all info, I exit my child processes as they are no longer needed. In my parent process, I keep reading from the pipes and update the display as needed until the read ends have closed and all info have been read.
3. To ignore signals coming from ctrl-z, I created a sigaction with handler set to SIG_IGN and specified signal to be intercepted to be SIGTSTP. I called the code for this before any fork calls so that both parent and child processes will ignore SIGTSTP signals. To alter the actions of ctrl-c, I set up different sigactions for SIGINT for parent and child processes. In the SIGINT handler for the child processes, I sent a SIGSTOP to themselves so that they will wait for a signal from the parent process. In the SIGINT handler for the parent process, I ask the user for an input 'r' to continue, or anything else to terminate. If the user inputs 'r', then I send a SIGCONT signal to the process gid which includes the child processes since the parent and child processes have the same process gid. If the user inputs anything else, then I send a SIGTERM signal to the process gid which terminates the parent and child processes.

__Implementation__ <br />
1. For implementation, I separated the code into sections that performed computation, display, process creation, and signal checking.
   I had the main function check the input arguments and feed them into an update function that updated variables
   which decided what information to display. Then I fed those variables into a display function that created the relevant processes for getting the information and had the values fed through pipes to the parent process for printing the corresponding information. I then created functions for the new signal handlers and functions to assign new signal actions (for SIGTSTP and SIGINT).
2. I have 4 modules:
   computations: contains functions for retrieving data from files and performing calculations
   display: contains functions for displaying memory, cpu, cores, and max frequency information
   process control: contains functions for updating what values to display and managing processes and pipes
   signals: contains functions for setting up new signal handlers
4. __int getNumCores()__: This function retrieves the number of cores from files <br />
   __double getMaxFreq()__: This function retrieves the maximum frequency from files <br />
   __double getMemoryUsage(struct sysinfo* info)__: This function retrieves the current system memory usage by using struct sysinfo to get the system information. <br />
   __double getCpuUsage(int tdelay)__: This function retrieves the current cpu usage by taking a sample at time t1 and a sample at time t2 such that there is a delay of tdelay between the two samples. For the samples, we take the total cpu utilization time (T1 and T2) and subtract the idle time to get U1 and U2 respectively. Then we calculate the cpu usage as (U2-U1)/(T2-T1) * 100. <br />
   __void displayParameters(int samples, int microsecondsTdelay)__: This function displays the user specified parameters, samples and microseconds of tdelay <br />
   __void displayMemoryGraph(long totalRam, int samples, int outputRow)__: This function displays the memory graph in its initial state without any points graphed <br />
   __void displayCPUGraph(int samples, int outputRow)__: This function displays the cpu graph in its initial state without any points graphed <br />
   __void printCores(int numCores)__: This function prints numCores cores <br />
   __void displayCoreInfo(int outputRow, int numCores, double maxFreq)__: This function displays the number of cores and the maximum frequency of the system <br />
   __void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow)__: This function updates the memory graph with the new point which is to be graphed at location column currCol and row outputRow <br />
   __void updateCPUGraph(double cpuUsage, int currCol, int outputRow)__: This function updates the cpu graph with the new point which is to be graphed at location column currCol and row outputRow <br />
   __bool isNumber(const char* str)__: This function determines whether str is a number <br />
   __int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input)__: This function determines whether the user input 'input' is a valid input and updates the corresponding boolean accordingly <br />
   __long getTotalRam()__: This function calculates the total ram of the system <br />
   __double getMemoryPerBarGB()__: This function calculates the total memory per bar for the memory graph display <br />
   __void createProcessesAndPipes(bool displayCores, bool displayMemory, bool displayCPU, int samples, int tdelay ,int memoryOutputRow, int cpuOutputRow, int coreOutputRow)__: This function creates a pipe each for the following: memory, cores, and max frequency. Then this function forks to create the four processes and have them each write one of the four pieces of info to the corresponding pipe to the parent process. The parent process keeps reading from the pipes and updating the display until all info is read and the pipes are closed. <br />
   __void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore)__: This function determines at which lines each piece of information is displayed and calls createProcessesAndPipes to display the information. <br />
   __void sigIntHandler(int sig)__: This function is the parent processes' handler for the SIGINT signal. It asks the user to input 'r' to continue displaying the graph info and anything else to exit the program. If user inputs 'r', SIGCONT signal is sent to pgid. If user inputs anything else, SIGTERM signal is sent to pgid. <br />
   __void sigIntHandlerChild(int sig)__: This function is the child process' handler for the SIGINT signal. It sends the child process a SIGSTOP signal to stop its execution until a SIGCONT signal arrives to continue the process, or a SIGTERM signal to arrive to terminate the process. <br />
   __void signalSetup()__: This function sets up the sigactions to ignore SIGTSTP signals and the new sigIntHandler for SIGINT signals <br />
   __void signalSetupChild()__: This function sets up the sigaction for the new sigIntHandlerChild for SIGINT signals <br />

__Flow Chart Diagram__ <br />
![flow chart diagram](https://github.com/user-attachments/assets/5208b1bd-e815-4610-a9d0-db1603c9280e)

__How To Use Program:__ <br />
1. To compile code: make <br />
2. makefile uses ccflags -std=c99, -Wall, -Werror <br />
3. To remove .o files and myMonitoringTool executable use: make clean
4. To run: ./myMonitoringTool [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T] <br />
Notes
Same arguments can be used. ie ./myMonitoringTool --cpu --cpu
The rightmost values will be used. ie ./myMonitoringTool 20 --samples=30 will run the program with samples = 30
Another example, ./myMonitoringTool --tdelay=2000 --tdelay=1000 will run the program with tdelay = 1000

__Expected Results__ <br />
The output should always be in the following order (if specified to be displayed by user): user specified or default tdelay and samples ,memory graph, cpu graph, cores and maximum frequency info, cores display. <br />
If no input, default tdelay = 500000 and default sample = 20, and all info will be displayed. <br />
If there is some input not for specifying samples or tdelay, then only that info will be displayed. <br />
When running the program, if ctrl-z is input by user, it is ignored and the program continues execution. <br />
When running the program, if ctrl-c is input by user, the graphs should stop displaying new values and wait for user input. If user inputs 'r', then continue execution of the program and continue displaying graph values. If user inputs anything other than 'r', then terminate the program.

__Test Cases__ <br />
./myMonitoringTool displays the memory, cpu, and cores and max frequency info, with samples=20, tdelay=500000 <br />
./myMonitoringTool 30 displays the memory, cpu, and cores and max frequency info, with samples=30, tdelay=500000 <br />
./myMonitoringTool 40 1000000 displays the memory, cpu, and cores and max frequency info, with samples=40, tdelay=1000000 <br />
./myMonitoringTool -1 1000000 exits the program and prints a message telling the user the proper syntax because samples>=0 <br />
./myMonitoringTool 30 -1 exits the program and prints a message telling the user the proper syntax because tdelay>=0 <br />
./myMonitoringTool --memory displays the memory graph with samples=20, tdelay=500000 <br />
./myMonitoringTool --cpu --cores displays the cpu graph and cores and max freq info with samples=20, tdelay=500000 <br />
./myMonitoringTool 30 100000 --cpu --memory displays the cpu and memory graphs with samples=30, tdelay=100000 <br />
./myMonitoringTool 30 100000 --cpu 10 exits the program and prints a message telling the user the proper syntax because samples and tdelay have to be the first two input args. <br />
./myMonitoringTool --tdelay=100000 --cpu displays the cpu graph with samples=20, tdelay=100000 <br />
./myMonitoringTool --tdelay = 100000 exits the program and prints a message telling the user the proper syntax because there should no spaces for the tdelay argument (should be --tdelay=100000) <br />
./myMonitoringTool --samples = 30 exits the program and prints a message telling the user the proper syntax because there should no spaces for the samples argument (should be --samples=30) <br />
./myMonitoringTool 30 100000 --tdelay=200000 --memory will display the memory graph with samples=30 and tdelay=200000 because the rightmost values are used <br />
./myMonitoringTool --memory --memory --cpu --memory will display the memory and cpu graphs with samples=20 and tdelay=500000 because repeated arguments are allowed <br />

__Notes__ <br />
Memory utilization was calculated using <sys/sysinfo.h>, namely using variables totalram and freeram of struct sysinfo.<br />
Used ram is equal to totalram - freeram. 

Number of CPUs was calculated by going to /sys/devices/system/cpu and iterating through the cpu0, cpu1, ..., <br />
located there to find the max cpu number.<br />

Max frequency was calculated by reading the value stored at<br />
/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq and converting from Khz to Ghz.<br />

To add the tdelay between each sample, usleep() was utilized.<br />
There was a warning when compiling with -std=c99 so added #define _DEFAULT_SOURCE<br />
as stated in https://man7.org/linux/man-pages/man3/usleep.3.html.<br />

Escape codes were utilized to clear the screen and position the cursor to the right position for printing.<br />

https://man7.org/linux/man-pages/man2/sigaction.2.html set sa_handler to SIG_IGN so that when sigaction() is called with SIGTSTP, the stop signal is ignored. <br />
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-kill-send-signal-process found if pid is equal to 0, then kill sends its signal to all processes with the same process group ID as the sender. In my implementation, only the parent process will be calling the particular signal handling function so by saying kill(0, signal) I am able to send signal to the child processes since child processes have the same pgid which can be observed through calling 'ps xao pid,ppid,pgid,comm'. <br />

__References__ <br />
https://man7.org/linux/man-pages/man3/usleep.3.html <br />
https://man7.org/linux/man-pages/man2/sysinfo.2.html <br />
https://man7.org/linux/man-pages/man2/sigaction.2.html <br />
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-kill-send-signal-process <br />
https://man7.org/linux/man-pages/man5/proc_cpuinfo.5.html <br />
https://docs.kernel.org/filesystems/proc.html <br />
https://man7.org/linux/man-pages/man7/signal.7.html <br />
https://man7.org/linux/man-pages/man2/kill.2.html <br />
https://man7.org/linux/man-pages/man2/sigaction.2.html <br />
https://man7.org/linux/man-pages/man7/pipe.7.html <br />
https://www.man7.org/linux/man-pages/man2/setpgid.2.html <br />
