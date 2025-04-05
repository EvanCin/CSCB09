#include "display.h"

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
		printf("\u2500");
	}
}

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

void updateMemoryGraph(double memoryPerBarGB, double usedRamGB, int currCol, int outputRow) {
	printf("\x1b[%d;%df%.2f", outputRow, 11, usedRamGB);
	outputRow += 13;
	int currRow = ((int) (usedRamGB / memoryPerBarGB));
	printf("\x1b[%d;%df", outputRow - currRow, 9 + currCol);
	printf("#\n");
}

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