#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    FILE *fp;
    void *null_bytes;

    if(argc != 4){
        printf("\n");
        printf("Zero out part of a file.\n");
        printf("\n");
        printf("  Usage: \n");
        printf("    ./zero_out_file_part <file> <offset> <size>\n");
        printf("\n");
    }

    char *lib = argv[1];
    int offset = (int) strtol(argv[2], NULL, 16);
    int size   = (int) strtol(argv[3], NULL, 16);

    null_bytes = malloc(size);
    memset(null_bytes, 0, size);

    fp = fopen(lib, "r+b");
    fseek(fp, offset, 0);
    fwrite(null_bytes, size, 1, fp);
    fclose(fp);

    return 0;
}
