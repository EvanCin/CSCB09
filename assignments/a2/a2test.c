#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
	char buf[PATH_MAX];
	//5000 is hardcoded, need to check if file /proc/%d exists
	//to find end of processes
	while(i < numProcesses) {
		sprintf(path, "/proc/%d/fd", i);
		sprintf(temp, "/proc/%d/fd/0", i); 
		DIR* dir = opendir(path);
		//Checks if able to access process info
		if(dir != NULL) {
			//printf("%d ", i);
			struct dirent* entry;
			if((entry = readdir(dir)) != NULL) {
				printf("process %d with inode %s ",i, entry->d_name);
			}
			readlink(temp, buf, sizeof(buf)-1);
			printf("%s ", buf);
		}
		closedir(dir);
		i++;
	}
	
	
	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
