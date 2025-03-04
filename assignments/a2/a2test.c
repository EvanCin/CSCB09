#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	//this part gets all accessible fds of processes
	//in fdinfo are the inode for each fd
	//this way we can get pid, fd, and inode info
	//need to get filename
	int i = 1;
	char path[25];
	while(i < 5000) {
		sprintf(path, "/proc/%d/fdinfo", i); 
		DIR* dir = opendir(path);
		if(dir != NULL) {
			printf("%d ", i);
		}
		i++;
	}

	return 0;
	//struct dirent* entry;
	

	//closedir(dir);

}
