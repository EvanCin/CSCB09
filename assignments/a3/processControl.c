#define _DEFAULT_SOURCE //For unistd.h and usleep()

#include "processControl.h"
#include "signals.h"
#include "computations.h"
#include "display.h"

bool isNumber(const char* str) {
	for(int i = 0; str[i] != '\0'; i++) {
		if(!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

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
///_|> descry: this function gets the total ram, helper function for createProcessesAndPipes
///_|> returning: this function returns the total ram
	struct sysinfo info;
    sysinfo(&info);
 	long totalram = info.totalram;
	return totalram;
}
double getMemoryPerBarGB() {
///_|> descry: this function turns total ram into memory per bar for display, helper function for createProcessesAndPipes
///_|> returning: this function returns ram per bar in GB
	long totalram = getTotalRam();
	int totalRamGB = totalram / 1000000000;
	double memoryPerBarGB = (double) totalRamGB / 12;
	return memoryPerBarGB;
}

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
	int pids[4];
	
	if(displayMemory) {
		pids[0] = fork();
		//This child process calculates memory info and writes it to pipeMemory
		if(pids[0] == 0) {
			signalSetupChild();
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
			signalSetupChild();
			close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCores[0]); close(pipeCores[1]); 
			close(pipeFreq[0]); close(pipeFreq[1]); close(pipeCPU[0]);
			for(int i = 0; i < samples; i++) {
				double cpuUsage = getCpuUsage(tdelay);
				write(pipeCPU[1], &cpuUsage, sizeof(cpuUsage));
			}
			close(pipeCPU[1]);
			exit(0);
		}
	}
	if(displayCores) {
		pids[2] = fork();
		//This child process calculates cores info and writes it to pipeCores
		if(pids[2] == 0) {
			close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCPU[0]); close(pipeCPU[1]); 
			close(pipeFreq[0]); close(pipeFreq[1]); close(pipeCores[0]);
			//retreive cores and max freq info
			int numCores;
			numCores = getNumCores();
			write(pipeCores[1], &numCores, sizeof(int));
			close(pipeCores[1]);
			exit(0);
		}
		pids[3] = fork();
		//This child process calculates max freq info and writes it to pipeFreq
		if(pids[3] == 0) {
			close(pipeMemory[0]); close(pipeMemory[1]); close(pipeCPU[0]); close(pipeCPU[1]); 
			close(pipeCores[0]); close(pipeCores[1]); close(pipeFreq[0]);
			double maxFreq;
			maxFreq = getMaxFreq();
			write(pipeFreq[1], &maxFreq, sizeof(double));
			close(pipeFreq[1]);
			exit(0);
		}
	}
	close(pipeMemory[1]); close(pipeCPU[1]); close(pipeCores[1]); close(pipeFreq[1]);

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
	while(read1 != 0 || read2 != 0) {
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
	}
	if(displayCores) {
		read3 = read(pipeCores[0], &numCores, sizeof(int));
		if(read3 == 0) perror("No core info from pipe\n");
		read4 = read(pipeFreq[0], &maxFreq, sizeof(double));
		if(read4 == 0) perror("No max freq info from pipe\n");
		displayCoreInfo(coreOutputRow, numCores, maxFreq);
	}
	close(pipeMemory[0]); close(pipeCPU[0]); close(pipeCores[0]); close(pipeFreq[0]);
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
	createProcessesAndPipes(displayCore, displayMemory, displayCPU, samples, tdelay, memoryOutputRow, cpuOutputRow, coreOutputRow);
	printf("\x1b[%d;%df", endOutputRow + 14, 1);
}