#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>

#define PATH_MAX 4096

int getNumProcesses() {
/*
descry: this function gets the number of active processes
returning: this function returns an int representing the number of active processes
*/
	int numProcesses = -1;
	char str[20];
	FILE* readFile = fopen("/proc/stat", "r");
	while(numProcesses == -1 && fscanf(readFile, "%19s", str) != EOF) {
		if(strcmp(str, "processes") == 0) {
			fscanf(readFile, "%d", &numProcesses);
		}
	}
	fclose(readFile);
	return numProcesses;
}

void displayCompositeTable(int numProcesses, int pid) {
/*
descry: this function displays the composite table
numProcesses: the number of active processes
pid: the process specified by the user, pid if given, -1 otherwise
returning: this function does not return anything
*/
	printf("         PID     FD     Filename       Inode\n");
	printf("        ======================================\n");
	int i = 1;
	int maxProcess = numProcesses;
	if(pid != -1) {
		i = pid;
		maxProcess = pid+1;
	}
	char path[PATH_MAX];
	while(i < maxProcess) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					char fdPath[PATH_MAX];
					char fileName[PATH_MAX];
					struct stat statData;
					int status;
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					status = stat(fdPath, &statData);
					int length = readlink(fdPath, fileName, sizeof(fileName)-1);
					//Only want filename
					fileName[length] = '\0';
					if(status == 0) {
						printf("         %d  %s      %s      %ld\n", i, entry->d_name, fileName, statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ======================================\n");
}

void displayPerProcessTable(int numProcesses, int pid) {
/*
descry: this function displays the process FD table
numProcesses: the number of active processes
pid: the process specified by the user, pid if given, -1 otherwise
returning: this function does not return anything
*/
	printf("         PID     FD\n");
	printf("        ============\n");
	int i = 1;
	int maxProcess = numProcesses;
	if(pid != -1) {
		i = pid;
		maxProcess = pid+1;
	}
	char path[PATH_MAX];
	while(i < maxProcess) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					printf("         %d  %s\n", i, entry->d_name);
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ============\n");
}

void displaySystemWideTable(int numProcesses, int pid) {
/*
descry: this function displays the system wide table
numProcesses: the number of active processes
pid: the process specified by the user, pid if given, -1 otherwise
returning: this function does not return anything
*/
	printf("         PID     FD     Filename\n");
	printf("        ===============================================\n");

	int i = 1;
	int maxProcess = numProcesses;
	if(pid != -1) {
		i = pid;
		maxProcess = pid+1;
	}
	char path[PATH_MAX];
	while(i < maxProcess) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					char fdPath[PATH_MAX];
					char fileName[PATH_MAX];
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					int length = readlink(fdPath, fileName, sizeof(fileName)-1);
					//Only want filename
					fileName[length] = '\0';
					printf("         %d  %s      %s\n", i, entry->d_name, fileName);
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ===============================================\n");
}

void displayVnodesTable(int numProcesses, int pid) {
/*
descry: this function displays the vnode table
numProcesses: the number of active processes
pid: the process specified by the user, pid if given, -1 otherwise
returning: this function does not return anything
*/
	printf("           FD          Inode\n");
	printf("        ===============================================\n");

	int i = 1;
	int maxProcess = numProcesses;
	if(pid != -1) {
		i = pid;
		maxProcess = pid+1;
	}
	char path[PATH_MAX];
	while(i < maxProcess) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					char fdPath[PATH_MAX];
					struct stat statData;
					int status;
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					status = stat(fdPath, &statData);
					if(status == 0) {
						printf("           %s            %ld\n", entry->d_name, statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ===============================================\n");
}

void displaySummaryTable(int numProcesses) {
/*
descry: this function displays each active process with the number of open file descriptors
numProcesses: the number of active processes
returning: this function does not return anything
*/
	printf("         Summary Table\n");
	printf("         =============\n");
	int i = 1;
	char path[PATH_MAX];
	while(i < numProcesses) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			int fdsOpen = 0;
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					fdsOpen++;
				}
			}
			printf("%d (%d), ", i, fdsOpen);
		}
		closedir(dir);
		i++;
	}
	printf("\n");
}

void displayThresholdTable(int numProcesses, int thresholdVal) {
/*
descry: this function displays each active process with the number of open file descriptors
		only if number of open file descriptors is greater than or equal to thresholdVal
numProcesses: the number of active processes
thresholdVal: the user indicated threshold value, processes with number of fds >= thresholdVal will be printed
returning: this function does not return anything
*/
	printf("## Offending processes -- #FD threshold=%d\n", thresholdVal);
	int i = 1;
	char path[PATH_MAX];
	while(i < numProcesses) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			int fdsOpen = 0;
			struct dirent* entry;
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					fdsOpen++;
				}
			}
			if(fdsOpen >= thresholdVal) {
				printf("%d (%d), ", i, fdsOpen);
			}
		}
		closedir(dir);
		i++;
	}
	printf("\n");
}

bool isNumber(const char* str) {
/*
descry: this function checks if a string is a number
str: a string
returning: this function returns true if str is a number, false otherwise
*/
	for(int i = 0; str[i] != '\0'; i++) {
		if(!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

bool isAccessiblePid(int pid) {
/*
descry: this function checks if the given pid is accessible
pid: a process id
returning: this function returns true if the pid's file descriptor directory is accessible, false otherwise
*/
	char path[PATH_MAX];
	sprintf(path, "/proc/%d/fd", pid);
	DIR* dir = opendir(path);
	//Checks if able to access process info
	if(dir != NULL) {
		closedir(dir);
		return true;
	}
	return false;
}

bool updateArg(char* arg, bool* dComposite, bool* dSystemWide, bool* dVnodes, 
		bool* dProcess, bool* dSummary, bool* dThreshold, int* thresholdVal) {
/*
descry: this function updates the booleans of tables to display based on arg
arg: user input argument
dComposite: true indicates display composite table, false otherwise
dSystemWide: true indicates display system wide table, false otherwise
dVnodes: true indicates display Vnodes table, false otherwise
dProcess: true indicates display process FD table, false otherwise
dSummary: true indicates display summary table, false otherwise
dThreshold: true indicates display threshold table, false otherwise
thresholdVal: the minimum number of fds a process needs to be printed in threshold table
returning: this function returns true if update successful, false is update failed
*/
	if(strcmp(arg, "--per-process") == 0) {
		*dProcess = true;
		return true;
	}
	if(strcmp(arg, "--systemWide") == 0) {
		*dSystemWide = true;
		return true;
	}
	if(strcmp(arg, "--Vnodes") == 0) {
		*dVnodes = true;
		return true;
	}
	if(strcmp(arg, "--composite") == 0) {
		*dComposite = true;
		return true;
	}
	if(strcmp(arg, "--summary") == 0) {
		*dSummary = true;
		return true;
	}
	int strlen = 0;
	while(arg[strlen] != '\0') {
		strlen++;
	}
	if(strlen < 13) {
		return false;
	}
	char firstHalfInput[13];
	strncpy(firstHalfInput, arg, 12);
	if(strcmp(firstHalfInput, "--threshold=") != 0) {
		return false;
	}
	char secondHalfInput[strlen];
	strncpy(secondHalfInput, arg+12, strlen-12);
	if(!isNumber(secondHalfInput)) {
		return false;
	} else {
		*dThreshold = true;
		*thresholdVal = atoi(secondHalfInput);
		return true;
	}
	return false;
}

void displayTables(bool dComposite, bool dSystemWide, bool dVnodes, 
					bool dProcess, bool dSummary, bool dThreshold, int thresholdVal, int pid) {
/*
descry: this function determines which tables are displayed
dComposite: true indicates display composite table, false otherwise
dSystemWide: true indicates display system wide table, false otherwise
dVnodes: true indicates display Vnodes table, false otherwise
dProcess: true indicates display process FD table, false otherwise
dSummary: true indicates display summary table, false otherwise
dThreshold: true indicates display threshold table, false otherwise
thresholdVal: the minimum number of fds a process needs to be printed in threshold table
pid: the process specified by the user, pid if given, -1 otherwise
returning: this function does not return anything
*/
	int numProcesses = getNumProcesses();
	if(dComposite) {
		displayCompositeTable(numProcesses, pid);
	}
	if(dSystemWide) {
		displaySystemWideTable(numProcesses, pid);
	}
	if(dVnodes) {
		displayVnodesTable(numProcesses, pid);
	}
	if(dProcess) {
		displayPerProcessTable(numProcesses, pid);
	}
	if(dSummary) {
		displaySummaryTable(numProcesses);
	}
	if(dThreshold) {
		displayThresholdTable(numProcesses, thresholdVal);
	}
	return;
}

int main(int argc, char** argv) {
	bool displayComposite = false;
	bool displaySystemWide = false;
	bool displayVnodes = false;
	bool displayProcess= false;
	bool displaySummary = false;
	bool displayThreshold = false;
	int thresholdVal = -1;
	int pid = -1;
	int i = 1;

	//Check input cases
	//Case 1: No args, so by default display the composite table
	if(argc <= 1) {
		displayComposite = true;
	} else if(isNumber(argv[i])) {
		//Case 2: First arg is Positional arg pid
		pid = atoi(argv[i]);
		if(!isAccessiblePid(pid)) {
			fprintf(stderr, "FD directory of PID %d is not accessible\n", pid);
			exit(1);
		}
		//pid is the only arg so by default display the composite table
		if(argc == 2) {
			displayComposite = true;
		}
		i++;
	}
	//Case 3: Check rest of args
	bool success;
	while(i < argc) {
		success = updateArg(argv[i], &displayComposite, &displaySystemWide, &displayVnodes,
					&displayProcess, &displaySummary, &displayThreshold, &thresholdVal);
		if(!success) {
			fprintf(stderr, "Invalid argument, input should be of form: ./showFDtables [pid] [--per-process] [--systemWide] [--Vnodes] [--composite] [--summary] [--threshold=X]\n");
			exit(1);
		}
		i++;
	}
	//After getting all arguments display the relevant info
	displayTables(displayComposite, displaySystemWide, displayVnodes, displayProcess,
					displaySummary, displayThreshold, thresholdVal, pid);
	return 0;
}