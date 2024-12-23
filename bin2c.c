#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#define MAXNAME 255
#define DEFAULT_NAME "binary.h"
#define MAXBUFFER 0x10000

unsigned char buffer[MAXBUFFER];

int process(char *binary_filename, char *output_filename, uint16_t start_address) {
    int size;
    FILE *binary_file;
    if ((binary_file = fopen(binary_filename, "rb"))) {
        size = fread(buffer, 1, sizeof(buffer), binary_file);
        FILE *output_file;
        if ((output_file = fopen(output_filename, "w"))) {
            fprintf(output_file, "#define BINARY_SIZE (%d)\n", size);
            fprintf(output_file, "uint8_t binary[BINARY_SIZE] = {\n");
            int index;
            for(index = 0; index < size; index++) {
                if (index % 16 == 0) {
                    fprintf(output_file, "\n");
                    fprintf(output_file, "/* 0x%04X */ ", start_address+index);
                }
                fprintf(output_file, " 0x%02X", buffer[index]);
                if ((index+1) < size) {
                    fprintf(output_file, ",");
                } else {
                    fprintf(output_file, " };");
                }
            }
            fprintf(output_file, "\n");
            fclose(output_file);
        } else {
            fprintf(stderr, "can't open output file %s", output_filename);
            return 0;
        }
    } else {
            fprintf(stderr, "can't open binary file %s", binary_filename);
            return 0;
    }
    return size;
}

int main(int argc, char *argv[]) {
    int opt;
    struct option long_options[] = {
        { "help", no_argument, NULL, 'h' },
        { "output", optional_argument, NULL, 'o' },
        { NULL, 0 , NULL, 0 }
    };
    int loaded = 0;
    char name[MAXNAME];
    uint16_t start_address = 0x0000;
    strcpy(name, DEFAULT_NAME);
    while ((opt = getopt_long(argc, argv, "ho:a:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                {
                    if(!sscanf(optarg,"%4hx", &start_address)) {
                        fprintf(stderr, "incorrect start address: %s\n", optarg);
                        exit(1);
                    }
                }
                break;
            case 'h':
                printf("Usage: bin2c myfile.bin [-o|--output name.h]\n");
                printf("   will generate name.h declaring an array of bytes in myfile.bin\n");
                return 1;
            case 'o':
                strcpy(name, optarg);
                break;
        }
    }
    if (optind < argc) {
        if ((loaded = process(argv[optind], name, start_address)) == 0) {
            return 1;
        }
    } else {
        fprintf(stderr, "missing binary file\n");
        return 1;
    }
    return 0;
}
