#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef __Display_header
#define __Display_header

void displayParameters(int samples, int microsecondsTdelay);

void displayMemoryGraph(long totalRam, int samples, int outputRow);

void displayCPUGraph(int samples, int outputRow);

void printCores(int numCores);

void displayCoreInfo(int outputRow, int numCores, double maxFreq);

void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow);

void updateCPUGraph(double cpuUsage, int currCol, int outputRow);

#endif