#include <stdio.h>
#include <stdlib.h>

void lie(int* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i]++;
    }
}
int main(int argc, char** argv) {

    if(argc > 1) {
        printf("first argument %s\n", argv[1]);
    }
    if(argc > 2) {
        printf("second argument %s\n", argv[2]);
    }

    int count = atoi(argv[2]);
    for(int i = 0; i < count; i++) {
        printf("HI\n");
    }
    return 0;
}
