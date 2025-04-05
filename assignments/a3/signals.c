#define _DEFAULT_SOURCE
#include "signals.h"

void sigIntHandler(int sig) {
	printf("\x1b[%d;%df\n", 30, 2);
	printf("  Enter r to resume, anything else to quit\n");
	char userInput[10];
	scanf("%9s", userInput);
	if(strcmp(userInput, "r") == 0) {
		kill(0, SIGCONT);
	} else {
		kill(0, SIGTERM);
	}
}

void sigIntHandlerChild(int sig) {
	kill(getpid(), SIGSTOP);
} 

void signalSetup() {
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
}

void signalSetupChild() {
	struct sigaction newact;
	newact.sa_handler = sigIntHandlerChild;
	newact.sa_flags = 0;
	sigemptyset(&newact.sa_mask);
	sigaction(SIGINT, &newact, NULL);
}