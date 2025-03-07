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

void displayCompositeTable(int numProcesses) {
	printf("         PID     FD      Filename                Inode\n");
	printf("        ===============================================\n");

	//int i = 1;
	int i = 2675;
	int max = 2675+1; //make this 1 greater than the particular pid
	char path[MAX_PATH_LENGTH];
	while(i < max) {
		sprintf(path, "/proc/%d/fd", i);
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			//printf("%d ", i);
			struct dirent* entry;
			//if((entry = readdir(dir)) != NULL) {
			//	printf("process %d with inode %s ",i, entry->d_name);
			//}
			while((entry = readdir(dir)) != NULL) {
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					char fdPath[PATH_MAX];
					char buf[PATH_MAX];
					struct stat statData;
					int status;
					sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
					status = stat(fdPath, &statData);
					
					//printf("%s ", fdPath);
					int length = readlink(fdPath, buf, sizeof(buf)-1);
					buf[length] = '\0';
					//printf("PID: %d, FD: %s, Filename: %s\n", i, entry->d_name, buf);
					if(status == 0) {
						printf("         %d  %s      %s       %ld\n", i, entry->d_name, buf, statData.st_ino);
						//printf("INODE: %ld\n", statData.st_ino);
					}
					//printf("%d\n",entry->d_ino);
				}
			}
		}
		closedir(dir);
		i++;
	}
	printf("        ===============================================\n");
}

void displayPerProcessTable(int numProcesses) {
	printf("         PID     FD\n");
	printf("        ===============================================\n");
	//int i = 1;
	int i = 2675;
	int max = 2675+1; //make this 1 greater than the particular pid
	char path[MAX_PATH_LENGTH];
	while(i < max) {
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

void displaySystemWideTable(int numProcesses) {
	printf("         PID     FD      Filename\n");
	printf("        ===============================================\n");

	//int i = 1;
	int i = 2675;
	int max = 2675+1; //make this 1 greater than the particular pid
	char path[MAX_PATH_LENGTH];
	while(i < max) {
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

void displayVnodesTable(int numProcesses) {
	printf("           FD          Inode\n");
	printf("        ===============================================\n");

	//int i = 1;
	int i = 2675;
	int max = 2675+1; //make this 1 greater than the particular pid
	char path[MAX_PATH_LENGTH];
	while(i < max) {
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

int main() {
	int numProcesses = getNumProcesses();
	printf("NUMPROCESSES %d\n", numProcesses);
	displayCompositeTable(numProcesses);
	displayPerProcessTable(numProcesses);
	displaySystemWideTable(numProcesses);
	displayVnodesTable(numProcesses);
	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
