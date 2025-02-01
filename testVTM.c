#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <string.h>

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

int main() {
    printf("HELLO WORLD\n");
    printf("Added sysinfo.h\n");

    struct sysinfo info;
    int success = sysinfo(&info);
    printf("%d\n",success);

    long totalram = info.totalram;
    long freeram = info.freeram;
    printf("total ram: %ld bytes\n", totalram);
    printf("free ram: %ld bytes\n", freeram);
    printf("percentage free ram: %f\n", (double) freeram/totalram);
    printf("percentage ram used: %f\n",(double) (totalram-freeram)/totalram);
    printf("Memory unit size in byte: %d\n", info.mem_unit);
    printf("Number of current processes: %d\n", info.procs);
    printf("Available high memory size: %ld\n", info.freehigh);

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
    return 0;
}


