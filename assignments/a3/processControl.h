#include <stdbool.h>
#include <unistd.h>

#ifndef __ProcessControl_header
#define __ProcessControl_header

bool isNumber(const char* str);
///_|> descry: this function determines whether or not str is a number
///_|> str: the string being checked
///_|> returning: this function returns true if str is a number, false otherwise

int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input);
///_|> descry: this function checks if input is a valid command line argument and updates the corresponding value
///_|> samples: pointer to the samples value
///_|> tdelay: pointer to the tdelay value
///_|> displayMemory: if true memory graph will be displayed
///_|> displayCPU: if true cpu graph will be displayed
///_|> displayCore: if true core info will be displayed
///_|> input: input command line argument
///_|> returning: this function returns 1 if input is valid argument, -1 otherwise

void createProcessesAndPipes(bool displayCores, bool displayMemory, bool displayCPU, int samples, int tdelay ,int memoryOutputRow, int cpuOutputRow, int coreOutputRow);
///_|> descry: this function creates processes to calculate memory usage, cpu usage, cores info, and max frequency info.
///_|> The calculations are passed through pipes to the parent process where the info is displayed.
///_|> displayCores: if true core info will be displayed
///_|> displayMemory: if true memory info will be displayed
///_|> displayCPU: if true cpu info will be displayed
///_|> samples: number of samples to take
///_|> tdelay: microseconds to delay by
///_|> memoryOutputRow: initial memory graph output row
///_|> cpuOutputRow: initial cpu graph output row
///_|> coreOutputRow: core display output row
///_|> returning: this function does not return anything

void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore);
///_|> descry: this function prints the information corresponding to the command line arguments
///_|> samples: number of samples to take
///_|> tdelay: microseconds to delay by
///_|> displayMemory: if true memory info will be displayed
///_|> displayCPU: if true cpu info will be displayed
///_|> displayCore: if true core info will be displayed
///_|> returning: this function does not return anything

#endif