#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH_LENGTH 256

typedef struct fileInfo {
	int fd;
	char* fileName;
	int inode;
	FileInfo* next;
} FileInfo;

typedef struct pidInfo {
	FileInfo* root;
	int numFDs;
} PidInfo;

//Remember to free memory
FileInfo* insertFileInfo(FileInfo* root, int fd, char* fileName, int inode) {
	FileInfo* newFileInfo = malloc(sizeof(FileInfo));
	newFileInfo->fd = fd;
	strcpy(newFileInfo->fileName, fileName);
	newFileInfo->inode = inode;
	if(root == NULL) {
		return newFileInfo;
	}
	root->next = newFileInfo;
	return newFileInfo;
}

void processData(PidInfo* arr, int numProcesses) {
	int i = 1;
	char procPath[MAX_PATH_LENGTH];
	while(i < numProcesses) {
		sprintf(procPath, "/proc/%d/fd", i);
		DIR* dir = opendir(procPath);
		//Checks if able to access process info
		if(dir == NULL) {
			i++;
			continue;
		}
		PidInfo pidInfo;
		FileInfo* currFileInfo;
		struct dirent* entry;
		while((entry = readdir(dir)) != NULL) {
			if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				
				char fdPath[MAX_PATH_LENGTH];
				char buf[MAX_PATH_LENGTH];
				struct stat statData;
				int status;
				sprintf(fdPath, "/proc/%d/fd/%s", i, entry->d_name);
				status = lstat(fdPath, &statData);
				int length = readlink(fdPath, buf, sizeof(buf)-1);
				//Putting '\0' at length so only file name is received
				buf[length] = '\0';

				

				//printf("PID: %d, FD: %s, Filename: %s\n", i, entry->d_name, buf);
				if(status == 0) {
					//printf("INODE: %ld\n", statData.st_ino);
					if(pidInfo.root == NULL) {
						pidInfo.root = insertFileInfo(NULL, atoi(entry->d_name), buf, statData.st_ino);
						currFileInfo = pidInfo.root;
					} else {
						currFileInfo = insertFileInfo(currFileInfo, atoi(entry->d_name), buf, statData.st_ino);
					}
				}
			}
		}
		closedir(dir);
		i++;
	}
}


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
	
	//5000 is hardcoded, need to check if file /proc/%d exists
	//to find end of processes
	
	
	
	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
