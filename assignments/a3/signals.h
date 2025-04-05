#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#ifndef __Signals_header
#define __Signals_header

void sigIntHandler(int sig);
///_|> descry: this function is the parent process's SIGINT handler.
///_|> Asks user for input and if input is 'r' sends SIGCONT to parent process's pgid,
///_|> otherwise sends SIGTERM to parent process's pgid
///_|> sig: the signal that invoked this handler
///_|> returning: this function does not return anything

void sigIntHandlerChild(int sig);
///_|> descry: this function is the child process's SIGINT handler.
///_|> Sends SIGSTOP to the current (child) process
///_|> sig: the signal that invoked this handler
///_|> returning: this function does not return anything

void signalSetup();
///_|> descry: this function changes signal actions to ignore SIGTSTP and to execute
///_|> sigIntHandler function when SIGINT occurs
///_|> returning: this function does not return anything

void signalSetupChild();
///_|> descry: this function changes signal actions for child process's to execute
///_|> sigIntHandlerChild function when SIGINT occurs
///_|> returning: this function does not return anything

#endif
