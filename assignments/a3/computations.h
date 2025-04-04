
#include <stdio.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef __Computations_header
#define __Computations_header

int getNumCores();

double getMaxFreq();

double getMemoryUsage(struct sysinfo* info);

double getCpuUsage(int tdelay);


#endif