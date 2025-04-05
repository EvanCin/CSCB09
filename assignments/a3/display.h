#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef __Display_header
#define __Display_header

void displayParameters(int samples, int microsecondsTdelay);
///_|> descry: this function prints the number of samples and prints tdelay as microseconds and as seconds
///_|> samples: the number of samples the processes will take for the graphs
///_|> microsecondsTdelay: microseconds to delay by
///_|> returning: this function does not return anything

void displayMemoryGraph(long totalRam, int samples, int outputRow);
///_|> descry: this function displays the memory graph with no data
///_|> totalRam: the total ram in bytes
///_|> samples: number of samples to take
///_|> outputRow: the initial row for printing
///_|> returning: this function does not return anything

void displayCPUGraph(int samples, int outputRow);
///_|> descry: this function displays the CPU graph with no data
///_|> samples: number of samples to take
///_|> outputRow: the initial row for printing
///_|> returning: this function does not return anything

void printCores(int numCores);
///_|> descry: this function prints numCores cores
///_|> numCores: the number of cores to print
///_|> returning: this function does not return anything

void displayCoreInfo(int outputRow, int numCores, double maxFreq);
///_|> descry: this function prints the number of cores as a diagram starting at row outputRow and prints the maximum frequency
///_|> outputRow: the initial row for printing
///_|> numCores: the number of cores to print
///_|> maxFreq: the maximum frequency
///_|> returning: this function does not return anything

void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow);
///_|> descry: this function prints the memory usage from the current sample to the memory graph
///_|> memoryPerBarGB: the amount of GB each bar '|' represents
///_|> usedRamGB: current ram usage
///_|> currCol: the current column for printing
///_|> outputRow: the initial row for printing
///_|> returning: this function does not return anything

void updateCPUGraph(double cpuUsage, int currCol, int outputRow);
///_|> descry: this function prints the CPU usage from the current sample to the CPU graph
///_|> cpuUsage: current CPU usage
///_|> currCol: the current column for printing
///_|> outputRow: the initial row for printing
///_|> returning: this function does not return anything

#endif