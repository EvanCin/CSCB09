#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

int main() {
    printf("HELLO WORLD\n");
    printf("Added sysinfo.h\n");

    struct sysinfo info;
    int success = sysinfo(&info);
    printf("%d\n",success);

    long totalram = info.totalram;
    long freeram = info.freeram;
    printf("total ram: %ld bytes\n", totalram);
    printf("free ram: %ld bytes\n", freeram);
    printf("percentage free ram: %f\n", (double) freeram/totalram);
    printf("percentage ram used: %f\n",(double) (totalram-freeram)/totalram);
    printf("Memory unit size in byte: %d\n", info.mem_unit);
    printf("Number of current processes: %d\n", info.procs);
    printf("Available high memory size: %ld\n", info.freehigh);
}













