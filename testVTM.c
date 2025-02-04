#include <stdio.h>
#include <stdlib.h>
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
		printf("Current cpu: %d\n",num_cpu);
		fclose(read_file);
		num_cpu++;
		sprintf(str_num_cpu, "%d", num_cpu);
		file_path[path_length] = '\0';
		strncat(file_path, str_num_cpu, 32);
		read_file = fopen(file_path, "r");
	}
	return num_cpu;
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

double getCpuUsage() {
	FILE* readFile;
	readFile = fopen("/proc/stat", "r");
	char* cpu;
	int user, nice, system, idle, iowait, irq, softirq; 
	fscanf(readFile, "%s %d %d %d %d %d %d %d", cpu, &user, &nice, &system,
												&idle, &iowait, &irq, &softirq);
	fclose(readFile);
	int totalCpuTime = user + nice + system + idle + iowait + irq + softirq;
	double cpuUsagePercentage = (1 - ((double)idle / totalCpuTime)) * 100;
	return cpuUsagePercentage;
}

void displayParameters(int samples, int microsecondsTdelay) {
	double secondsTdelay = (double) microsecondsTdelay / 1000000;
	printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)\n\n", samples, microsecondsTdelay, secondsTdelay);
}

void displayMemoryGraph(long totalRam, int samples) {
	double totalRamGB = (double) totalRam / 1000000000;
	printf("%.f GB ", totalRamGB);
	
	int row = 4;	
	int col = 7;
	for(row; row < 16; row++) {
		printf("\x1b[%d;%df", row, col);
		printf("|");
	}
	col = 2;
	printf("\x1b[%d;%df", row, col);
	printf("0 GB ");
	for(int i = 0; i < samples+1; i++) {
		printf("─");
	}
}

void updateMemoryGraph(double memoryPerBar, double usedRamGB, int currCol) {
	int currRow = (int) (usedRamGB / memoryPerBar);
	printf("\x1b[%d;%df", 8 - currRow, 8 + currCol);
	printf("#");
}
int main(int argc, int** argv) {
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

	int samples = 20;
	int tdelay = 500000;
	//clear screen
	printf("\033[2J");
	//position cursor at top left
	printf("\033[H");
	//No arguments so present all information
	if(argc == 1) {
		displayParameters(samples, tdelay);
	//	displayMemoryGraph();
	}

    struct sysinfo info;
    int success = sysinfo(&info);
	//printf("Memory Usage: %.2f%%\n", getMemoryUsage(&info));
	printf("v Memory  %.2f\n", getMemoryUsage(&info)); 
	long totalram = info.totalram;
	displayMemoryGraph(totalram, samples);
	double memoryPerBar = (double) totalram / 12;
	for(int i = 0; i < 5; i++) {
		double usedRamGB = getMemoryUsage(&info);
		updateMemoryGraph(memoryPerBar, usedRamGB, i); 
		sleep(0.5);
	}
	//printf("%d\n", getNumCpus());
	//printf("CPU Usage: %.2f%%\n", getCpuUsage());
	printf("\n");
    return 0;
}

