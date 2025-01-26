#include <stdio.h>
#include <stdlib.h>
#define HEADER_SIZE 44

int main(int argc, char* argv[]) {
    char* input_name, *output_name;
    FILE* input_file, *output_file;
    double volume_multiplier;
    short sample;
    short header[HEADER_SIZE];
    int error;
    
    if(argc != 4) {
        fprintf(stderr, "Usage: %s input_file output_file volume_multiplier\n Example: %s input.dat output.dat 1.5\n", argv[0], argv[0]);
        return 1;
    }
    input_name = argv[1];
    output_name = argv[2];
    volume_multiplier = atof(argv[3]);

    input_file = fopen(input_name, "rb");
    if(input_file == NULL) {
        fprintf(stderr, "Could not open input_file\n");
        return 1;
    }
    output_file = fopen(output_name, "wb");
    if(output_file == NULL) {
        fprintf(stderr, "Could not open input_file\n");
        return 1;
    }

    fread(header, HEADER_SIZE, 1, input_file);
    error = fwrite(header, HEADER_SIZE, 1, output_file);
    if(error != 1) {
        fprintf(stderr, "Could not write audio header\n");
        return 1;
    }

    while(fread(&sample, sizeof(short), 1, input_file) == 1) {
        sample *= volume_multiplier;
        error = fwrite(&sample, sizeof(short), 1, output_file);
        if(error != 1) {
            fprintf(stderr, "Could not write a sample\n");
            return 1;
        }
    }
    error = fclose(input_file);
    if(error != 0) {
        fprintf(stderr, "Could not close input_file\n");
        return 1;
    }
    error = fclose(output_file);
    if(error != 0) {
        fprintf(stderr, "Could not close output_file\n");
        return 1;
    }
    return 0;
}