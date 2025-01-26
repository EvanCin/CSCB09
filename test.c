#include <stdlib.h>
#include <stdio.h>


#include <sys/resource.h>
#include <sys/utsname.h>

int main() {
    //Need to find memory usage (plot memory utilization per sample in GB)

    //Need to find CPU usage (report how much utilization of the CPU is being done in percentage)

    //Need to find cores (report number of cores and max. frequency)

    int who = RUSAGE_SELF;
    struct rusage usage;
    int ret = getrusage(who, &usage);

    


    struct utsname utsn;
    uname(&utsn);
    printf("\n%s", utsn.version);
}

