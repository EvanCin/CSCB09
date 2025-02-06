#define _DEFAULT_SOURCE //For unistd.h and usleep()
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

int getNumCpus() {
	FILE* read_file;
	int num_cpu = 0;
	char file_path[256] = "/sys/devices/system/cpu/cpu";
	int path_length = strlen(file_path);
	//Convert num_cpu to string
	char str_num_cpu[32];
	sprintf(str_num_cpu, "%d", num_cpu);
	strncat(file_path, str_num_cpu, 32);
	read_file = fopen(file_path, "r");
	while(read_file != NULL) {
		fclose(read_file);
		num_cpu++;
		sprintf(str_num_cpu, "%d", num_cpu);
		file_path[path_length] = '\0';
		strncat(file_path, str_num_cpu, 32);
		read_file = fopen(file_path, "r");
	}
	return num_cpu;
}

double getMaxFreq() {
	FILE* readFile;
	readFile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
	if(readFile == NULL) {
		perror("Could not read /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
		exit(1);
	}
	double maxFreqGhz;
	fscanf(readFile, "%lf", &maxFreqGhz);
	return maxFreqGhz /= 1000000;
}

double getMemoryUsage(struct sysinfo* info) {
	sysinfo(info);
	long totalRam = info->totalram;
	long freeRam = info->freeram;
	long usedRam = totalRam - freeRam;
	double memoryUsageGB = (double) usedRam / 1000000000;
	return memoryUsageGB;
}

double getCpuUsage(double* prevTotalCpuTime, double* prevIdleTime) {
	FILE* readFile;
	readFile = fopen("/proc/stat", "r");
	if(readFile == NULL) {
		perror("Could not open /proc/stat");
		exit(1);
	}
	char cpu[8];
	double user, nice, system, currIdleTime, iowait, irq, softirq; 
	fscanf(readFile, "%s %le %le %le %le %le %le %le", cpu, &user, &nice, &system,
												&currIdleTime, &iowait, &irq, &softirq);
	fclose(readFile);
	double currTotalCpuTime = user + nice + system + currIdleTime + iowait + irq + softirq;
	double updatedTotalCpuTime = currTotalCpuTime - *prevTotalCpuTime;
	double updatedIdleCpuTime = currIdleTime - *prevIdleTime;
	*prevTotalCpuTime = currTotalCpuTime;
	*prevIdleTime = currIdleTime;
	double cpuUsagePercentage = (1 - ((double)updatedIdleCpuTime / updatedTotalCpuTime)) * 100;
	return cpuUsagePercentage;
}

void displayParameters(int samples, int microsecondsTdelay) {
	double secondsTdelay = (double) microsecondsTdelay / 1000000;
	printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)\n\n", samples, microsecondsTdelay, secondsTdelay);
}

void displayMemoryGraph(long totalRam, int samples, int outputRow) {
	int col = 1;
	double totalRamGB = (double) totalRam / 1000000000;
	printf("\x1b[%d;%df", outputRow, col);
	printf("v Memory        GB");
	outputRow++;
	col++;
	printf("\x1b[%d;%df", outputRow, col);
	printf("%.f GB ", totalRamGB);
	col = 8;
	while(outputRow < 16) {
		printf("\x1b[%d;%df", outputRow, col);
		printf("|");
		outputRow++;
	}
	col = 2;
	printf("\x1b[%d;%df", outputRow, col);
	printf(" 0 GB ");
	for(int i = 0; i < samples+1; i++) {
		printf("─");
	}
}

void displayCPUGraph(int samples, int outputRow) {
	int col = 1;
	printf("\x1b[%d;%df", outputRow, col);
	printf("v CPU        %%\n");
	printf("  100%% ");
	outputRow += 1;
	col += 7;
	int rowsToPrint = outputRow+10;
	while(outputRow < rowsToPrint) {
		printf("\x1b[%d;%df", outputRow, col);
		printf("|");
		outputRow++;
	}
	col = 5;
	printf("\x1b[%d;%df", outputRow, col);
	printf("0%% ");
	for(int i = 0; i < samples+1; i++) {
		printf("─");
	}
}

void printCores(int numCores) {
	for(int i = 0; i < numCores; i++) {
		printf("+──+  ");
	}
	printf("\n");
	for(int i = 0; i < numCores; i++) {
		printf("|  |  ");
	}
	printf("\n");
	for(int i = 0; i < numCores; i++) {
		printf("+──+  ");
	}
	printf("\n");
}

void displayCoreInfo(int outputRow) {
	int numCpus = getNumCpus();
	printf("\x1b[%d;%df", outputRow, 1);
	printf("v Number of Cores: %d @ %.2f GHz\n", numCpus, getMaxFreq());
	int rowsToPrint = numCpus / 4;
	int cpusToPrint = numCpus % 4;
	for(int r = 0; r < rowsToPrint; r++) {
		printCores(4);
	}
	printCores(cpusToPrint);
}

void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow) {
	printf("\x1b[%d;%df%.2f", outputRow, 11, usedRamGB);
	outputRow += 13;
	int currRow = ((int) (usedRamGB / memoryPerBarGB));
	//printf("\x1b[%d;%df", 16 - currRow, 9 + currCol);
	printf("\x1b[%d;%df", outputRow - currRow, 9 + currCol);
	printf("#\n");
}

void updateCPUGraph(double cpuUsage, int currCol, int outputRow) {
	if(cpuUsage < 0) {
		cpuUsage = 0;
	}
	printf("\x1b[%d;%df%.2f", outputRow, 8, cpuUsage);
	outputRow += 10;
	// int currRow = (int) (cpuUsage / 10); //Couldnt get ceil to work here for some reason
	int currRow = (int) (cpuUsage / 10);
	if(currRow >= 10) {
		currRow--;
	}else if(currRow < 0) {
		currRow = 0;
	}
	printf("\x1b[%d;%df", outputRow - currRow, 9 + currCol);
	printf(":\n");
}

bool isNumber(const char* str) {
	for(int i = 0; str[i] != '\0'; i++) {
		if(!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

//Returns 1 if valid input and update successful, -1 otherwise
int updateValues(int* samples, int* tdelay, bool* displayMemory, bool* displayCPU, bool* displayCore, char* input) {
	if(strcmp(input, "--memory") == 0) {
		*displayMemory = true;
		return 1;
	} else if(strcmp(input, "--cpu") == 0) {
		*displayCPU = true;
		return 1;
	} else if(strcmp(input, "--cores") == 0) {
		*displayCore = true;
		return 1;
	}
	int strlen = 0;
	int startOfNum = 0;
	while(input[strlen] != '\0') {
		if(input[strlen] == '=') {
			startOfNum = strlen + 1;
		}
		strlen++;
	}
	char secondHalfInput[strlen];
	int inputNum;
	strncpy(secondHalfInput, input+startOfNum, strlen);
	if(!isNumber(secondHalfInput)) {
		return -1;
	} else {
		inputNum = atoi(secondHalfInput);
	}
	char firstHalfInput[strlen];
	strcpy(firstHalfInput, input);
	firstHalfInput[startOfNum-1] = '\0';
	if(strcmp(firstHalfInput, "--samples") == 0) {
		*samples = inputNum;
		return 1;
	} else if(strcmp(firstHalfInput, "--tdelay") == 0) {
		*tdelay = inputNum;
		return 1;
	}
	return -1;
}

void displayGraphs(int samples, int tdelay, bool displayMemory, bool displayCPU, int memoryOutputRow, int cpuOutputRow) {
	double prevTotalCpuTime = 0;
	double prevIdleTime = 0;
	getCpuUsage(&prevTotalCpuTime, &prevIdleTime);
	struct sysinfo info;
    sysinfo(&info);
 	long totalram = info.totalram;
 	int totalRamGB = totalram / 1000000000;
	double memoryPerBarGB = (double) totalRamGB / 12;
	if(displayMemory) {
 		displayMemoryGraph(totalram, samples, memoryOutputRow);
	}
	if(displayCPU) {
 		displayCPUGraph(samples, cpuOutputRow);
	}
 	for(int i = 0; i < samples; i++) {
		if(displayMemory) {
 			double usedRamGB = getMemoryUsage(&info);
 	   		updateMemoryGraph(memoryPerBarGB, usedRamGB, i, memoryOutputRow);
 		}
		if(displayCPU) {
			updateCPUGraph(getCpuUsage(&prevTotalCpuTime, &prevIdleTime), i, cpuOutputRow);
 		}
 		usleep(tdelay);
 	}
}

void displayRequestedInfo(int samples, int tdelay, bool displayMemory, bool displayCPU, bool displayCore) {
	int memoryOutputRow = 3;
	int cpuOutputRow;
	int coreOutputRow;
	int endOutputRow;
	if(!displayMemory && !displayCPU && !displayCore) {
		displayMemory = true;
		displayCPU = true;
		displayCore = true;
	}
	//clear screen
	printf("\033[2J");
	//position cursor at top left
	printf("\033[H");
	displayParameters(samples, tdelay);
	if(displayMemory) {
		cpuOutputRow = 18;
		coreOutputRow = 18;
		endOutputRow = 20;
		if(displayCPU) {
			coreOutputRow = 32;
			endOutputRow = 40;
		}
		if(displayCore) {
			endOutputRow = 40;
		}
	} else if(displayCPU) {
		cpuOutputRow = 3;
		coreOutputRow = 18;
		endOutputRow = 20;
		if(displayCore) {
			endOutputRow = 40;
		}
	} else if(displayCore) {
		coreOutputRow = 3;
		endOutputRow = 20;
	}
	if(displayMemory || displayCPU) {
		displayGraphs(samples, tdelay, displayMemory, displayCPU, memoryOutputRow, cpuOutputRow);
	}
	if(displayCore) {
		displayCoreInfo(coreOutputRow);
	}
	printf("\x1b[%d;%df", endOutputRow, 1);
}

int main(int argc, char** argv) {
	//Default values
	int samples = 20;
	int tdelay = 500000;
	//Variables to check what info to display
	bool displayMemory = false;
	bool displayCPU = false;
	bool displayCore = false;
	int updateVal;	
	if(argc == 1) {
		displayParameters(samples, tdelay);
		displayRequestedInfo(samples, tdelay, displayMemory, displayCPU, displayCore);
		return 0;
	}
	if(argc == 2) {
		if(isNumber(argv[1])) {
			samples = atoi(argv[1]);
		} else {
			updateVal = updateValues(&samples, &tdelay, &displayMemory, &displayCPU, &displayCore, argv[1]);
			if(updateVal == -1) {
				printf("Wrong arguments. Syntax: ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]");
				exit(1);
			}
		}
		displayRequestedInfo(samples, tdelay, displayMemory, displayCPU, displayCore);
		return 0;
	}
	int commandIndex = 1;
	if(isNumber(argv[1])) {
		samples = atoi(argv[1]);
		commandIndex++;
		if(isNumber(argv[2])) {
			tdelay = atoi(argv[2]);
			commandIndex++;
		}
	}
	for(int i = commandIndex; i < argc; i++) {
		updateVal = updateValues(&samples, &tdelay, &displayMemory, &displayCPU, &displayCore, argv[i]);
		if(updateVal == -1) {
			printf("Wrong arguments. Syntax: ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]");
			exit(1);
		}
	}
	displayRequestedInfo(samples, tdelay, displayMemory, displayCPU, displayCore);
    return 0;
}