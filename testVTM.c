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
	long totalram = info->totalram;
	long freeram = info->freeram;
	
    printf("total ram: %ld bytes\n", totalram);
    printf("free ram: %ld bytes\n", freeram);

	double memoryUsagePercentage = (1 - ((double)freeram / totalram)) * 100;
	return memoryUsagePercentage;
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

int main() {
    printf("HELLO WORLD\n");
    printf("Added sysinfo.h\n");

    //printf("%d\n",success);

    //long totalram = info.totalram;
    //long freeram = info.freeram;
    //printf("percentage ram used: %f\n",(double) (totalram-freeram)/totalram);
    //printf("Memory unit size in byte: %d\n", info.mem_unit);
    //printf("Number of current processes: %d\n", info.procs);
    //printf("Available high memory size: %ld\n", info.freehigh);

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
	printf("%d\n", getNumCpus());
    struct sysinfo info;
    int success = sysinfo(&info);
	printf("Memory Usage: %.2f%%\n", getMemoryUsage(&info));
	printf("CPU Usage: %.2f%%\n", getCpuUsage());
    return 0;
}

