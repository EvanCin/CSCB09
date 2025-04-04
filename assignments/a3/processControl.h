
#include <stdbool.h>
#include <unistd.h>


#ifndef __ProcessControl_header
#define __ProcessControl_header

bool isNumber(const char* str);

int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input);

void createProcessesAndPipes(bool displayCores, bool displayMemory, bool displayCPU, int samples, int tdelay ,int memoryOutputRow, int cpuOutputRow, int coreOutputRow);

void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore);

#endif