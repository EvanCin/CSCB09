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
#include <sys/wait.h>


int pids[4];

void sigIntHandler(int sig) {
	
	printf("\n\n\nEnter r to resume, anything else to quit\n");
	//kill(getpid(), SIGSTOP);
	char userInput[10];
	scanf("%9s", userInput);
	if(strcmp(userInput, "r") == 0) {
		for(int i = 0; i < 2; i++) {
			kill(pids[i], SIGCONT);
		}
		kill(getpid(), SIGCONT);
	} else {
		kill(getpid(), SIGTERM);
	}
}

void sigIntHandlerChild(int sig) {
	kill(getpid(), SIGSTOP);
	
} 


/*Returns the number of cores*/
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

/*Returns the maximum frequency*/
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

/*
This function calls sysinfo() to get current memory status and
calculates current memory usage
Input: 
	info: a pointer to a struct sysinfo
Output: 
	Returns the memory usage in GB
*/
double getMemoryUsage(struct sysinfo* info) {
	sysinfo(info);
	long totalRam = info->totalram;
	long freeRam = info->freeram;
	long usedRam = totalRam - freeRam;
	double memoryUsageGB = (double) usedRam / 1000000000;
	return memoryUsageGB;
}

/*
This function reads from /proc/stat to get CPU times and
calculates the current CPU usage
Input:
	prevTotalCpuTime: a pointer to the cpu time of the last sample
	prevIdleTime: a pointer to the idle time of the last sample
Output:
	Returns the current CPU usage as a percentage
*/
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

/*Prints the number of samples and prints tdelay as microseconds and as seconds*/
void displayParameters(int samples, int microsecondsTdelay) {
	double secondsTdelay = (double) microsecondsTdelay / 1000000;
	printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)\n\n", samples, microsecondsTdelay, secondsTdelay);
}

/*
This function displays the memory graph with no data
Input:
	totalRam: the total ram in bytes
	samples: number of samples to take
	outputRow: the initial row for printing
*/
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
		printf("\u2500");
	}
}

/*
This function displays the CPU graph with no data
Input:
	samples: number of samples to take
	outputRow: the initial row for printing
*/
void displayCPUGraph(int samples, int outputRow) {
	int col = 1;
	printf("\x1b[%d;%df", outputRow, col);
	printf("v CPU        %%\n");
	printf("  100%% \n");
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
		printf("\u2500");
	}
}

/*Prints numCores cores*/
void printCores(int numCores) {
	for(int i = 0; i < numCores; i++) {
		printf("+\u2500\u2500+  ");
	}
	printf("\n");
	for(int i = 0; i < numCores; i++) {
		printf("|  |  ");
	}
	printf("\n");
	for(int i = 0; i < numCores; i++) {
		printf("+\u2500\u2500+  ");
	}
	printf("\n");
}

/*
This function prints the number of cores as a diagram
starting at row outputRow and prints the maximum frequency
*/
void displayCoreInfo(int outputRow, int numCores, double maxFreq) {
	printf("\x1b[%d;%df", outputRow, 1);
	printf("v Number of Cores: %d @ %.2f GHz\n", numCores, maxFreq);
	int rowsToPrint = numCores / 4;
	int coresToPrint = numCores % 4;
	for(int r = 0; r < rowsToPrint; r++) {
		printCores(4);
	}
	printCores(coresToPrint);
}

/*
This function prints the memory usage from the current sample to the memory graph
Input:
	memoryPerBarGB: the amount of GB each bar '|' represents
	usedRamGB: current ram usage
	currCol: the current column for printing
	outputRow: the initial row for printing
*/
void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow) {
	printf("\x1b[%d;%df%.2f", outputRow, 11, usedRamGB);
	outputRow += 13;
	int currRow = ((int) (usedRamGB / memoryPerBarGB));
	printf("\x1b[%d;%df", outputRow - currRow, 9 + currCol);
	printf("#\n");
}

/*
This function prints the CPU usage from the current sample to the CPU graph
Input:
	cpuUsage: current CPU usage
	currCol: the current column for printing
	outputRow: the initial row for printing
*/
void updateCPUGraph(double cpuUsage, int currCol, int outputRow) {
	if(cpuUsage < 0) {
		cpuUsage = 0;
	}
	printf("\x1b[%d;%df%.2f", outputRow, 8, cpuUsage);
	outputRow += 10;
	int currRow = (int) (cpuUsage / 10);
	if(currRow >= 10) {
		currRow--;
	}else if(currRow < 0) {
		currRow = 0;
	}
	printf("\x1b[%d;%df", outputRow - currRow, 9 + currCol);
	printf(":\n");
}

/*Returns true if str is a number, false otherwise*/
bool isNumber(const char* str) {
	for(int i = 0; str[i] != '\0'; i++) {
		if(!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

/*
This function checks if input is a valid command line argument and
updates the corresponding value
Input:
	samples: pointer to the samples value
	tdelay: pointer to the tdelay value
	displayMemory: if true memory graph will be displayed
	displayCPU: if true cpu graph will be displayed
	displayCore: if true core info will be displayed
	input: input command line argument
Output:
	1 if input is valid argument, -1 otherwise
*/
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

long getTotalRam() {
	struct sysinfo info;
    sysinfo(&info);
 	long totalram = info.totalram;
	return totalram;
}
double getMemoryPerBarGB() {
	long totalram = getTotalRam();
	int totalRamGB = totalram / 1000000000;
	double memoryPerBarGB = (double) totalRamGB / 12;
	return memoryPerBarGB;
}

/*Creates processes and runs concurrently*/
void createProcessesAndPipes(bool displayCores, bool displayMemory, bool displayCPU, int samples, int tdelay ,int memoryOutputRow, int cpuOutputRow, int coreOutputRow) {
	if(displayMemory) {
		displayMemoryGraph(getTotalRam(), samples, memoryOutputRow);
   }
   if(displayCPU) {
		displayCPUGraph(samples, cpuOutputRow);
   }
	//Pipe for each output type
	int pipeMemory[2];
	int pipeCPU[2];
	int pipeCores[2];
	int pipeFreq[2];
	pipe(pipeMemory);
	pipe(pipeCPU);
	pipe(pipeCores);
	pipe(pipeFreq);
	//Create processes if needed
	
	if(displayMemory) {
		pids[0] = fork();
		//This child process calculates memory info and writes it to pipeMemory
		if(pids[0] == 0) {
			struct sigaction newact2;
		newact2.sa_handler = sigIntHandlerChild;
		newact2.sa_flags = 0;
		sigemptyset(&newact2.sa_mask);
		sigaction(SIGINT, &newact2, NULL);
		
			close(pipeCPU[0]); close(pipeCPU[1]); close(pipeCores[0]); close(pipeCores[1]);
			close(pipeFreq[0]); close(pipeFreq[1]); close(pipeMemory[0]);
			struct sysinfo info;
    		sysinfo(&info);
			for(int i = 0; i < samples; i++) {
				double usedRamGB = getMemoryUsage(&info);
				write(pipeMemory[1], &usedRamGB, sizeof(usedRamGB));
				usleep(tdelay);
			}
			close(pipeMemory[1]);
			exit(0);
		}
	}
	if(displayCPU) {
		pids[1] = fork();
		//This child process calculates cpu usage and writes it to pipeCPU
		if(pids[1] == 0) {
		struct sigaction newact2;
		newact2.sa_handler = sigIntHandlerChild;
		newact2.sa_flags = 0;
		sigemptyset(&newact2.sa_mask);
		sigaction(SIGINT, &newact2, NULL);
		
			close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCores[0]); close(pipeCores[1]); 
			close(pipeFreq[0]); close(pipeFreq[1]); close(pipeCPU[0]);
			double prevTotalCpuTime = 0;
			double prevIdleTime = 0;
			for(int i = 0; i < samples; i++) {
				double cpuUsage = getCpuUsage(tdelay);
				write(pipeCPU[1], &cpuUsage, sizeof(cpuUsage));
			}
			close(pipeCPU[1]);
			exit(0);
		}
	}
	//if(displayCores) {
	//	pids[2] = fork();
	//	//This child process calculates cores and max freq info and writes it to pipeCores
	//	if(pids[2] == 0) {
	//		close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCPU[0]); close(pipeCPU[1]); 
	//		close(pipeFreq[0]); close(pipeFreq[1]); close(pipeCores[0]);
	//		//retreive cores and max freq info
	//		int numCores;
	//		numCores = getNumCores();
	//		write(pipeCores[1], &numCores, sizeof(int));
	//		close(pipeCores[1]);
	//		exit(0);
	//	}
	//	pids[3] = fork();
	//	if(pids[3] == 0) {
	//		close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCPU[0]); close(pipeCPU[1]); 
	//		close(pipeCores[0]); close(pipeCores[1]); close(pipeFreq[0]);
	//		double maxFreq;
	//		maxFreq = getMaxFreq();
	//		write(pipeCores[1], &maxFreq, sizeof(double));
	//		close(pipeFreq[1]);
	//		exit(0);
	//	}
	//}
	close(pipeMemory[1]); close(pipeCPU[1]); close(pipeCores[1]); close(pipeFreq[1]);

	//FOR DEBUGGING
	//printf("PARENT PID: %d, CHILD1 PID: %d, CHILD2 PID: %d, CHILD3 PID: %d\n", getpid(), pids[0], pids[1], pids[2]);
	//

	double cpuUsage;
	double memoryUsage;
	int numCores;
	double maxFreq;

	int i = 0;
	int samplesReadCPU = 0;
	int samplesReadMemory = 0;
	int read1, read2, read3, read4;
	read1 = read(pipeMemory[0], &memoryUsage, sizeof(memoryUsage));
	read2 = read(pipeCPU[0], &cpuUsage, sizeof(cpuUsage));
	while(read1 > 0 || read2 > 0) {
		if(read1 != 0) {
			if(samplesReadMemory != samples) {
				updateMemoryGraph(getMemoryPerBarGB(), memoryUsage, i, memoryOutputRow);
				samplesReadMemory++;
			}
			read1 = read(pipeMemory[0], &memoryUsage, sizeof(memoryUsage));
		}
		if(read2 != 0) {
			if(samplesReadCPU != samples) {
				updateCPUGraph(cpuUsage, i, cpuOutputRow);
				samplesReadCPU++;
			}
			read2 = read(pipeCPU[0], &cpuUsage, sizeof(cpuUsage));
		}
		i++;
		// printf("READ1: %d, READ2: %d\n", read1, read2);
	}
	

	//if(displayCores) {
	//	read3 = read(pipeCores[0], &numCores, sizeof(int));
	//	if(read3 == 0) perror("No core info from pipe\n");
	//	read4 = read(pipeFreq[0], &maxFreq, sizeof(double));
	//	if(read4 == 0) perror("No max freq info from pipe\n");
	//	displayCoreInfo(coreOutputRow, numCores, maxFreq);
	//}

	//waitpid(pids[0], NULL, 0); waitpid(pids[1], NULL, 0); waitpid(pids[2], NULL, 0);
	close(pipeMemory[0]); close(pipeCPU[0]); close(pipeCores[0]); close(pipeFreq[0]);
}

/*
This function prints the information corresponding to the command line arguments
Input:
	samples: the amount of samples to take
	tdelay: the delay between each sample
	displayMemory: if true displays the memory graph
	displayCPU: if true displays the CPU graph
	displayCore: if true displays the core info
*/
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
	// if(displayMemory || displayCPU) {
	// 	//displayGraphs(samples, tdelay, displayMemory, displayCPU, memoryOutputRow, cpuOutputRow);
		
	// }
	createProcessesAndPipes(displayCore, displayMemory, displayCPU, samples, tdelay, memoryOutputRow, cpuOutputRow, coreOutputRow);
	//if(displayCore) {
	//	displayCoreInfo(coreOutputRow);
	//	return;
	//}
	printf("\x1b[%d;%df", endOutputRow + 10, 1);
	//printf("\x1b[%d;%dfH%d", endOutputRow + 50, 1, endOutputRow);
}



int main(int argc, char** argv) {

	//Set up the signal handlers before creating processes
	struct sigaction newact;
	newact.sa_handler = SIG_IGN;
	newact.sa_flags = 0;
	sigemptyset(&newact.sa_mask);
	sigaction(SIGTSTP, &newact, NULL);
	
	struct sigaction newact2;
	newact2.sa_handler = sigIntHandler;
	newact2.sa_flags = 0;
	sigemptyset(&newact2.sa_mask);
	sigaction(SIGINT, &newact2, NULL);
	
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

