__How the program was written:__
Memory utilization was calculated using <sys/sysinfo.h>, namely using variables totalram and freeram of struct sysinfo.
Used ram is equal to totalram - freeram. 

CPU utilization was calculated by going to /proc/stat and dividing the idle time by the total time of the CPU.
Since the times in /proc/stat are total times since the computer has started, the values of total time and idle time
for the last sample are kept and subtracted from the current sample to get the actual change in time.
https://www.linuxhowtos.org/System/procstat.htm was used to understand the values in /proc/stat

Number of CPUs was calculated by going to /sys/devices/system/cpu and iterating through the cpu0, cpu1, ..., 
located there to find the max cpu number.

Max frequency was calculated by reading the value stored at
/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq and converting from Khz to Ghz.

To add the tdelay between each sample, usleep() was utilized.
There was a warning when compiling with -std=c99 so added #define _DEFAULT_SOURCE
as stated in https://man7.org/linux/man-pages/man3/usleep.3.html.

__Functions:__
1. int getNumCores()

   Returns the number of cores
3. double getMaxFreq()
   Returns the maximum frequency in Ghz
4. double getMemoryUsage(struct sysinfo* info)
   Returns the current amount of memory usage in GB
5. double getCpuUsage(double* prevTotalCpuTime, double* prevIdleTime)
   Returns the current cpu usage as a percentage
6. void displayParameters(int samples, int microsecondsTdelay)
   Displays the chosen (or default) samples and tdelay values
7. void displayMemoryGraph(long totalRam, int samples, int outputRow)
   Displays the memory graph with no graph values
8. void displayCPUGraph(int samples, int outputRow)
   Displays the CPU graph with no graph values
9. void printCores(int numCores)
   Displays numCores core visual representation
10. void displayCoreInfo(int outputRow)
   Displays all core visual representation and the maximum frequency
11. void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow)
   Function is called with every sample and updates the memory graph with the current memory usage
12. void updateCPUGraph(double cpuUsage, int currCol, int outputRow)
   Function is called with every sample and updates the CPU graph with the current CPU usage
13. bool isNumber(const char* str)
   Returns true if str is a number and false otherwise
14. int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input)
   Checks if input is a valid argument ie --memory, --cores, --cpu, --samples=N, --tdelay=T
   Updates the value corresponding to the argument accordingly
   Returns 1 if valid argument and -1 otherwise
15. void displayGraphs(int samples, int tdelay, bool displayMemory, bool displayCPU, int memoryOutputRow, int cpuOutputRow)
   Displays the memory graph with all samples taken tdelay apart if displayMemory is true
   Displays the CPU graph with all samples taken tdelay apart if displayCPU is true
16. void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore)
   Displays the information requested by the user from the command line arguments
17. int main(int argc, char** argv)
   Reads user command line arguments 



__How To Use Program:__
1. gcc myMonitoringTool -std=c99 -Wall -Werror -o myMonitoringTool
2. ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]
Notes
Same arguments can be used. ie ./myMonitoringTool --cpu --cpu
The rightmost values will be used. ie ./myMonitoringTool 20 --samples=30 will run the program with samples = 30
Another example, ./myMonitoringTool --tdelay=2000 --tdelay=1000 will run the program with tdelay = 1000
