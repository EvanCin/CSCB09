#include <stdio.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef __Computations_header
#define __Computations_header

int getNumCores();
///_|> descry: this function retrieves the number of cores from system files
///_|> returning: this function returns the number of cores

double getMaxFreq();
///_|> descry: this function retrieves the maximum frequency from system files
///_|> returning: this function returns the maximum frequency

double getMemoryUsage(struct sysinfo* info);
///_|> descry: this function calls sysinfo() to get current memory status and calculates current memory usage
///_|> info: a pointer to a struct sysinfo
///_|> returning: this function returns the memory usage in GB

double getCpuUsage(int tdelay);
///_|> descry: this function reads from /proc/stat to get CPU times and calculates the current CPU usage
///_|> tdelay: time to delay in microseconds
///_|> returning: this function returns the current CPU usage as a percentage

#endif