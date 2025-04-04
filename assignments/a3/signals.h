#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#ifndef __Signals_header
#define __Signals_header

void sigIntHandler(int sig);

void sigIntHandlerChild(int sig);

void signalSetup();

#endif