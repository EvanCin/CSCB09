void sigIntHandler(int sig) {
	
	printf("\n\n\nEnter r to resume, anything else to quit\n");
	//kill(getpid(), SIGSTOP);
	char userInput[10];
	scanf("%9s", userInput);
	if(strcmp(userInput, "r") == 0) {
		kill(0, SIGCONT);
	} else {
		kill(getpid(), SIGTERM);
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