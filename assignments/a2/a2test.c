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

	int i = 1;
	char path[MAX_PATH_LENGTH];
	while(i < numProcesses) {
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
					status = lstat(fdPath, &statData);
					
					//printf("%s ", fdPath);
					int length = readlink(fdPath, buf, sizeof(buf)-1);
					buf[length] = '\0';
					//printf("PID: %d, FD: %s, Filename: %s\n", i, entry->d_name, buf);
					if(status == 0) {
						printf("         %d  %s      %s       %ld\n", i, entry->d_name, buf, statData.st_ino);
						//printf("INODE: %ld\n", statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}

}



int main() {
	int numProcesses = getNumProcesses();
	printf("NUMPROCESSES %d\n", numProcesses);
	displayCompositeTable(numProcesses);
	
	
	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
