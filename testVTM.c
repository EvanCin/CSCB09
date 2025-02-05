#define _DEFAULT_SOURCE //For unistd.h and usleep()
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

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
    //printf("total ram: %ld bytes\n", totalram);
    //printf("free ram: %ld bytes\n", freeram);

	double memoryUsageGB = (double) usedRam / 1000000000;
	return memoryUsageGB;
}

double getCpuUsage(float* prevTotalCpuTime, float* prevIdleTime) {
	//**** Update this function according to piazza post ****
	FILE* readFile;
	readFile = fopen("/proc/stat", "r");
	if(readFile == NULL) {
		perror("Could not open /proc/stat");
		exit(1);
	}
	char cpu[8];
	float user, nice, system, currIdleTime, iowait, irq, softirq; 
	fscanf(readFile, "%s %e %e %e %e %e %e %e", cpu, &user, &nice, &system,
												&currIdleTime, &iowait, &irq, &softirq);
	fclose(readFile);
	float currTotalCpuTime = user + nice + system + currIdleTime + iowait + irq + softirq;

	float updatedTotalCpuTime = currTotalCpuTime - *prevTotalCpuTime;
	float updatedIdleCpuTime = currIdleTime - *prevIdleTime;
	*prevTotalCpuTime = currTotalCpuTime;
	*prevIdleTime = currIdleTime;

	double cpuUsagePercentage = (1 - ((double)updatedIdleCpuTime / updatedTotalCpuTime)) * 100;
	// printf("IDLE: %f\n", idle);
	// printf("TOTAL CPUTIME: %f\n", totalCpuTime);
	return cpuUsagePercentage;
}

void displayParameters(int samples, int microsecondsTdelay) {
	double secondsTdelay = (double) microsecondsTdelay / 1000000;
	printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)\n\n", samples, microsecondsTdelay, secondsTdelay);
}

void displayMemoryGraph(long totalRam, int samples) {
	double totalRamGB = (double) totalRam / 1000000000;
	printf(" %.f GB ", totalRamGB);
	
	int row = 4;	
	int col = 8;
	while(row < 16) {
		printf("\x1b[%d;%df", row, col);
		printf("|");
		row++;
	}
	col = 2;
	printf("\x1b[%d;%df", row, col);
	printf("0 GB  ");
	for(int i = 0; i < samples+1; i++) {
		printf("─");
	}
}

void displayCPUGraph(int samples) {
	int row = 18;
	int col = 1;
	printf("\x1b[%d;%df", row, col);
	printf("v CPU    %%\n");
	printf("  100%% ");
	row += 1;
	col += 7;
	while(row < 29) {
		printf("\x1b[%d;%df", row, col);
		printf("|");
		row++;
	}
	col = 5;
	printf("\x1b[%d;%df", row, col);
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

void displayCoreInfo() {
	int numCpus = getNumCpus();
	printf("\x1b[%d;%df", 32, 1);
	printf("v Number of Cores: %d @ %.2f GHz\n", numCpus, getMaxFreq());
	int rowsToPrint = numCpus / 4;
	int cpusToPrint = numCpus % 4;
	for(int r = 0; r < rowsToPrint; r++) {
		printCores(4);
		printf("\n");
	}
	printCores(cpusToPrint);
}

void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol) {
	printf("\x1b[%d;%df%.2f", 3, 11, usedRamGB);
	int currRow = ((int) (usedRamGB / memoryPerBarGB));
	printf("\x1b[%d;%df", 16 - currRow, 9 + currCol);
	printf("#\n");
}

void updateCPUGraph(double cpuUsage, int currCol) {
	if(cpuUsage < 0) {
		cpuUsage = 0;
	}
	printf("\x1b[%d;%df%.2f", 18, 8, cpuUsage);
	// int currRow = (int) (cpuUsage / 10); //Couldnt get ceil to work here for some reason
	int currRow = (int) (cpuUsage / 10);
	if(currRow == 10) {
		currRow--;
	}else if(currRow < 0) {
		currRow = 0;
	}
	printf("\x1b[%d;%df", 28 - currRow, 9 + currCol);
	printf(":\n");
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

	
	return -1;
}

bool isNumber(char* str) {

}

int main(int argc, char** argv) {
	float prevTotalCpuTime = 0;
	float prevIdleTime = 0;
    //FILE* read_file;
    //char line[256];
    //read_file = fopen("/proc/cpuinfo", "r");
    //if(read_file == NULL) {
    //     perror("fopen");
    //     exit(1);
    //}
    //printf("File read successful!\n");
    //while(fgets(line, 256, read_file) != NULL) {
    //        printf("%s\n", line);
    //}

	//Default values
	int samples = 20;
	int tdelay = 500000;
	//Variables to check what info to display
	bool displayMemory = false;
	bool displayCPU = false;
	bool displayCore = false;

	//clear screen
	printf("\033[2J");
	//position cursor at top left
	printf("\033[H");
	//No arguments so present all information
	if(argc == 1) {
		displayParameters(samples, tdelay);
		//<-- Display all info and return-->
	}
	if(argc > 6) {
		printf("Too many arguments\n");
		exit(1);
	}
	for(int i = 2; i < argc; i++) {
		//Check wether if theres positional arguments
		//Need a function to check if argv[1] and/or argv[2] is a number
	}

//     struct sysinfo info;
//     int success = sysinfo(&info);
// 	//printf("Memory Usage: %.2f%%\n", getMemoryUsage(&info));
// 	printf("v Memory  %.2f GB\n", getMemoryUsage(&info)); 
// 	long totalram = info.totalram;
// 	int totalRamGB = totalram / 1000000000;
// 	displayMemoryGraph(totalram, samples);
// 	displayCPUGraph(samples);
// 	double memoryPerBarGB = (double) totalRamGB / 12;
// 	for(int i = 0; i < samples; i++) {
// 		double usedRamGB = getMemoryUsage(&info);
// 	   	updateMemoryGraph(memoryPerBarGB, usedRamGB, i);
// 		updateCPUGraph(getCpuUsage(&prevTotalCpuTime, &prevIdleTime), i);
// 		//printf("%.10f ", getCpuUsage(&prevTotalCpuTime, &prevIdleTime));
// 		usleep(2000000);
// 	}
// //	printf("%d\n", getNumCpus());
// 	displayCoreInfo();
// 	// printf("\x1b[%d;%df", 40, 1);
// 	printf("\033[%dB", 2);
    return 0;
}
