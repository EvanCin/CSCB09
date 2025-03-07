#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH_LENGTH 256

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
	char path[MAX_PATH_LENGTH];
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
	char path[MAX_PATH_LENGTH];
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
	char path[MAX_PATH_LENGTH];
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
	char path[MAX_PATH_LENGTH];
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
	char secondHalfInput[strlen];
	int inputNum;
	strncpy(secondHalfInput, input+startOfNum, strlen);
	if(!isNumber(secondHalfInput)) {
		return -1;
	} else {
		inputNum = atoi(secondHalfInput);
	}
	return false;
}

int main(int argc, char** argv) {
	int numProcesses = getNumProcesses();
	printf("NUMPROCESSES %d\n", numProcesses);
	displayCompositeTable(numProcesses, -1);
	displayPerProcessTable(numProcesses, -1);
	displaySystemWideTable(numProcesses, -1);
	displayVnodesTable(numProcesses, -1);
	
	bool displayComposite;
	bool displaySystemWide;
	bool displayVnodes;
	bool displayProcess;
	bool displaySummary;
	bool displayThreshold;
	int thresholdVal;
	
	
	return 0;
}
