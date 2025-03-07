#define _DEFAULT_SOURCE //For readlink
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
	printf("         PID     FD      Filename                Inode\n");
	printf("        ===============================================\n");
	int i = 1;
	int maxProcess = numProcesses; //make this 1 greater than the particular pid
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
					char buf[PATH_MAX];
					struct stat statData;
					int status;
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					status = stat(fdPath, &statData);
					int length = readlink(fdPath, buf, sizeof(buf)-1);
					buf[length] = '\0';
					if(status == 0) {
						printf("         %d  %s      %s       %ld\n", i, entry->d_name, buf, statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ===============================================\n");
}

void displayPerProcessTable(int numProcesses, int pid) {
	printf("         PID     FD\n");
	printf("        ===============================================\n");
	int i = 1;
	int maxProcess = numProcesses; //make this 1 greater than the particular pid
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
	printf("        ===============================================\n");
}

void displaySystemWideTable(int numProcesses, int pid) {
	printf("         PID     FD      Filename\n");
	printf("        ===============================================\n");

	int i = 1;
	int maxProcess = numProcesses; //make this 1 greater than the particular pid
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
					char buf[PATH_MAX];
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					int length = readlink(fdPath, buf, sizeof(buf)-1);
					buf[length] = '\0';
					printf("         %d  %s      %s\n", i, entry->d_name, buf);
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ===============================================\n");
}

void displayVnodesTable(int numProcesses, int pid) {
	printf("           FD          Inode\n");
	printf("        ===============================================\n");

	int i = 1;
	int maxProcess = numProcesses; //make this 1 greater than the particular pid
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
						printf("        %s        %ld\n", entry->d_name, statData.st_ino);
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

/*Returns true if str is a number, false otherwise*/
bool isNumber(const char* str) {
	for(int i = 0; str[i] != '\0'; i++) {
		if(!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

//Updates the booleans of tables to display based on arg, returns -1 if arg is invalid
bool updateArg(char* arg, bool* dComposite, bool* dSystemWide, bool* dVnodes, 
		bool* dProcess, bool* dSummary, bool* dThreshold, int* thresholdVal) {
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
	strncpy(secondHalfInput, arg+13, strlen-13);
	if(!isNumber(secondHalfInput)) {
		return false;
	} else {
		*thresholdVal = atoi(secondHalfInput);
		return true;
	}
	return false;
}

void displayTables(bool dComposite, bool dSystemWide, bool dVnodes, 
					bool dProcess, bool dSummary, bool dThreshold, int thresholdVal, int pid) {
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
	// displayCompositeTable(numProcesses, -1);
	// displayPerProcessTable(numProcesses, -1);
	// displaySystemWideTable(numProcesses, -1);
	// displayVnodesTable(numProcesses, -1);
	
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
	//Case 1: No args, display only the composite table
	if(argc <= 1) {
		displayComposite = true;
	}
	//Case 2: First arg is Positional arg pid
	if(isNumber(argv[i])) {
		pid = atoi(argv[i]);
		i++;
	}
	//Case 3: Check rest of args
	bool success;
	for(i; i < argc; i++) {
		success = updateArg(argv[i], &displayComposite, &displaySystemWide, &displayVnodes,
					&displayProcess, &displaySummary, &displayThreshold, &thresholdVal);
		if(!success) {
			fprintf(stderr, "Invalid argument, input should be of form: \
				 ./showFDtables [pid] [--per-process] [--systemWide] [--Vnodes] [--composite] [--summary] [--threshold=X]");
			exit(1);
		}
	}
	//After getting all arguments display the relevant info
	displayTables(displayComposite, displaySystemWide, displayVnodes, displayProcess,
					displaySummary, displayThreshold, thresholdVal, pid);
	
	
	return 0;
}
