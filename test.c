#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <string.h>

// void myFunction(int x[]) {
//     printf("%lu", sizeof(x));
// }

int sum_array(int* arr, int size, int processes) {
    int each_process_nums = size/processes;
    pid_t pid;
    int sum = 0;
    for(int i = 0; i < processes; i++) {
        pid = fork();
        if(pid == -1) exit(1);
        if(pid == 0) {
            int start = i * each_process_nums;
            int end = start+each_process_nums;
            if(i+1 == processes) {
                end = size;
            }
            for(int j = start; j < end; j++) {
                sum += arr[j];
            }
            exit(sum);
        }
    }
    int status;
    for(int i = 0; i < processes; i++) {
        wait(&status);
        if(WIFEXITED(status)) {
            sum += WEXITSTATUS(status);
        }
    }
    return sum;
}

typedef struct min_heap {
    int size;       // the number of nodes in this heap; 0 <= size <= capacity
    int capacity;   // the number of nodes that can be stored in this heap
    int* indexMap;  // indexMap[id] is the index of node with ID id in array arr
  } MinHeap;

int main(int argc, char** argv) {
    int x = 1;
    printf("%d", x++);
    

    //Need to find memory usage (plot memory utilization per sample in GB)
    // Can get memory usage using sysinfo totalram and freeram

    //Need to find CPU usage (report how much utilization of the CPU is being done in percentage)
    // CPU usage is not a percentage stored somewhere
    // Need to find some sort of time 

    //Need to find cores (report number of cores and max. frequency)
    // "/proc/cpuinfo" gives the number of cores which is the number of processors
    // we can read the cpuinfo as a file
    // investigate /sys/devices/system/cpu/cpu*/ 

    //  /sys/devices/system/cpu/cpu0/cpufreq/
    //  /proc/stat

    // int who = RUSAGE_SELF;
    // struct rusage usage;
    // int ret = getrusage(who, &usage);

    


    // struct utsname utsn;
    // uname(&utsn);
    // printf("\n%s", utsn.version);
    // for (int i = 1; i <= 5; i++) {
    //     printf("%d\n", i);  // Print the number
    //     usleep(500000);     // Delay for 0.5 seconds (500,000 microseconds)
    // }
    return 0;
}

