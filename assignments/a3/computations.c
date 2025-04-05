#define _DEFAULT_SOURCE //For unistd.h and usleep()
#include "computations.h"

int getNumCores() {
	FILE* readFile;
	int numCores = 0;
	char filePath[256] = "/sys/devices/system/cpu/cpu";
	int pathLength = strlen(filePath);
	char strNumCores[32];
	sprintf(strNumCores, "%d", numCores);
	strncat(filePath, strNumCores, 32);
	readFile = fopen(filePath, "r");
	while(readFile != NULL) {
		fclose(readFile);
		numCores++;
		sprintf(strNumCores, "%d", numCores);
		filePath[pathLength] = '\0';
		strncat(filePath, strNumCores, 32);
		readFile = fopen(filePath, "r");
	}
	return numCores;
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

double getCpuUsage(int tdelay) {
	FILE* readFile;
	readFile = fopen("/proc/stat", "r");
	if(readFile == NULL) {
		perror("Could not open /proc/stat");
		exit(1);
	}
	char cpu[8];
	double user, nice, system, idle, iowait, irq, softirq; 
	//Take first sample
	fscanf(readFile, "%s %le %le %le %le %le %le %le", cpu, &user, &nice, &system,
												&idle, &iowait, &irq, &softirq);
	double T1 = user + nice + system + idle + iowait + irq + softirq;
	double idleTime1 = idle;
	double U1 = T1 - idleTime1;
	usleep(tdelay);
	//Take second sample
	rewind(readFile);
	fscanf(readFile, "%s %le %le %le %le %le %le %le", cpu, &user, &nice, &system,
												&idle, &iowait, &irq, &softirq);
	double T2 = user + nice + system + idle + iowait + irq + softirq;
	double idleTime2 = idle;
	double U2 = T2 - idleTime2;
	double cpuUsagePercentage = (U2 - U1) / (T2 - T1) * 100;
	fclose(readFile);
	return cpuUsagePercentage;
}