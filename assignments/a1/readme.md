__How the program was written:__ <br />
Memory utilization was calculated using <sys/sysinfo.h>, namely using variables totalram and freeram of struct sysinfo.<br />
Used ram is equal to totalram - freeram. 

CPU utilization was calculated by going to /proc/stat and dividing the idle time by the total time of the CPU.<br />
Since the times in /proc/stat are total times since the computer has started, the values of total time and idle time<br />
for the last sample are kept and subtracted from the current sample to get the actual change in time.<br />
https://www.linuxhowtos.org/System/procstat.htm was used to understand the values in /proc/stat<br />

Number of CPUs was calculated by going to /sys/devices/system/cpu and iterating through the cpu0, cpu1, ..., <br />
located there to find the max cpu number.<br />

Max frequency was calculated by reading the value stored at<br />
/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq and converting from Khz to Ghz.<br />

To add the tdelay between each sample, usleep() was utilized.<br />
There was a warning when compiling with -std=c99 so added #define _DEFAULT_SOURCE<br />
as stated in https://man7.org/linux/man-pages/man3/usleep.3.html.<br />

Escape codes were utilized to clear the screen and position the cursor to the right position for printing.<br />

__Functions:__ <br />
1. int getNumCores()<br />
   Returns the number of cores
2. double getMaxFreq()<br />
   Returns the maximum frequency in Ghz
3. double getMemoryUsage(struct sysinfo* info)<br />
   Returns the current amount of memory usage in GB
4. double getCpuUsage(double* prevTotalCpuTime, double* prevIdleTime)<br />
   Returns the current cpu usage as a percentage
5. void displayParameters(int samples, int microsecondsTdelay)<br />
   Displays the chosen (or default) samples and tdelay values
6. void displayMemoryGraph(long totalRam, int samples, int outputRow)<br />
   Displays the memory graph with no graph values
7. void displayCPUGraph(int samples, int outputRow)<br />
   Displays the CPU graph with no graph values
8. void printCores(int numCores)<br />
   Displays numCores core visual representation
9. void displayCoreInfo(int outputRow)<br />
   Displays all core visual representation and the maximum frequency
10. void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow)<br />
   Function is called with every sample and updates the memory graph with the current memory usage
11. void updateCPUGraph(double cpuUsage, int currCol, int outputRow)<br />
   Function is called with every sample and updates the CPU graph with the current CPU usage
12. bool isNumber(const char* str)<br />
   Returns true if str is a number and false otherwise
13. int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input)<br />
   Checks if input is a valid argument ie --memory, --cores, --cpu, --samples=N, --tdelay=T<br />
   Updates the value corresponding to the argument accordingly<br />
   Returns 1 if valid argument and -1 otherwise<br />
14. void displayGraphs(int samples, int tdelay, bool displayMemory, bool displayCPU, int memoryOutputRow, int cpuOutputRow)<br />
   Displays the memory graph with all samples taken tdelay apart if displayMemory is true<br />
   Displays the CPU graph with all samples taken tdelay apart if displayCPU is true<br />
15. void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore)<br />
   Displays the information requested by the user from the command line arguments
16. int main(int argc, char** argv)<br />
   Reads user command line arguments



__How To Use Program:__ <br />
1. gcc myMonitoringTool.c -std=c99 -Wall -Werror -o myMonitoringTool <br />
2. ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T] <br />
__Notes__ <br />
Same arguments can be used. ie ./myMonitoringTool --cpu --cpu <br />
The rightmost values will be used. ie ./myMonitoringTool 20 --samples=30 will run the program with samples = 30 <br />
Another example, ./myMonitoringTool --tdelay=2000 --tdelay=1000 will run the program with tdelay = 1000 <br />
