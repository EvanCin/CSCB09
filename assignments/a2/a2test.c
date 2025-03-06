#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
	int numProcesses = -1;
	char str[20];
	FILE* readFile = fopen("/proc/stat", "r");
	while(numProcesses == -1 && fscanf(readFile, "%19s", str) != EOF) {
		if(strcmp(str, "processes") == 0) {
			fscanf(readFile, "%d", &numProcesses);
		}
	}
	fclose(readFile);
	printf("NUMPROCESSES %d\n", numProcesses);
	//this part gets all accessible fds of processes
	//in fdinfo are the inode for each fd
	//this way we can get pid, fd, and inode info
	//need to get filename
	int i = 1;
	char path[25];
	char temp[25];
	//5000 is hardcoded, need to check if file /proc/%d exists
	//to find end of processes
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
					printf("PID: %d, FD: %s, Filename: %s\n", i, entry->d_name, buf);
					if(status == 0) {
						printf("INODE: %ld\n", statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}
	
	
	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
