#include "computations.h"
#include "display.h"
#include "processControl.h"
#include "signals.h"

int main(int argc, char** argv) {
	signalSetup();
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
				printf("Wrong arguments. Syntax: ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]\n");
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
			printf("Wrong arguments. Syntax: ./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]\n");
			exit(1);
		}
	}
	displayRequestedInfo(samples, tdelay, displayMemory, displayCPU, displayCore);
    return 0;
}