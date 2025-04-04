

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