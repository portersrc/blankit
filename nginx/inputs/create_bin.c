#include <stdio.h>
#include <stdlib.h>

void usage_and_exit(char *progname)
{
    printf("\nUsage:\n");
    printf("\t%s <output_filename> <size_of_file>\n\n", progname);
    printf("--where <size_of_file> is in bytes and must fit within an int\n\n\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char *output_filename;
    int size_of_file;
    int i;

    if(argc != 3){
        usage_and_exit(argv[0]);
    }

    output_filename = argv[1];
    size_of_file    = atoi(argv[2]);

    fp = fopen(output_filename, "wb");
    for(i = 0; i < size_of_file; i++){
        fprintf(fp, "%c", rand());
    }
    fclose(fp);

    return 0;
}
