// Reference:
// http://www.linuxjournal.com/article/7795
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>

// XXX: Including string.h causes compilationn to fail, presumably because
// we're redefining strcmp below. Without this include, we have implicit
// declaration warnings within shim.c for memcpy. In this example, everything
// still works, but this could be another hiccup in a real example.
//#include <string.h>

void *dlopen_handle;

int strcmp(const char *s1, const char *s2)
{
    static int (*glibc_strcmp) (const char *a, const char *b) = NULL;
    char *error;

    printf("shim strcmp()\n");

    void *handle = dlopen("/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libc/libc-2.23.so", RTLD_LAZY);
    if(!handle){
      fputs(dlerror(), stderr);
      exit(1);
    }

    if(!glibc_strcmp){
        printf("shim strcmp(): setting glibc_strcmp\n");
        glibc_strcmp = dlsym(handle, "strcmp");
        if((error = dlerror()) != NULL){
            fprintf(stderr, "%s\n", error);
            exit(1);
        }
    }
#if 0
    // example: always return 0, meaning they are equal
    return 0;
#endif

    // experiment 4
    FILE *fp;
    char *lib = "libs/libc/libc-2.23.so";
    long lib_size;
    size_t nmemb_read;

    fp = fopen(lib, "rb");
    fseek(fp, 0, SEEK_END);
    lib_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  //same as rewind(f);

    char *lib_buf = malloc(lib_size + 1);
    nmemb_read = fread(lib_buf, lib_size, 1, fp);
    assert(nmemb_read == 1);
    fclose(fp);

    // 6031: 0000000000083b03    47 IFUNC   GLOBAL DEFAULT   13 strcmp
    int func_size = 47;
    int func_offset = 0x83b03;
    printf("shim strcmp(): copying function back into place\n");
    /*FILE *debug_bytes_fp = fopen("debug_bytes_to_copy_back", "wb");
    size_t nmemb_written = fwrite(lib_buf+func_offset, func_size, 1, debug_bytes_fp);
    assert(nmemb_written == 1);
    fclose(debug_bytes_fp);
    memcpy(glibc_strcmp, lib_buf+func_offset, func_size);*/
    char *func_buf = malloc(func_size);
    FILE *debug_bytes_fp = fopen("debug_bytes_at_sym", "rb");
    nmemb_read = fread(func_buf, func_size, 1, debug_bytes_fp);
    assert(nmemb_read == 1);
    fclose(debug_bytes_fp);
    memcpy(glibc_strcmp, func_buf, func_size);

    printf("shim strcmp(): call real strcmp\n");
    return glibc_strcmp(s1, s2);
}


void PinInit_aux()
{
    static void (*libPinInit_aux) (void) = NULL;
    char *error;
    printf("shim PinInit_aux()\n");

    if(!libPinInit_aux){
        libPinInit_aux = dlsym(dlopen_handle, "PinInit_aux");
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            exit(1);
        }
    }

    printf("shim PinInit_aux(): calling real PinInit_aux at address %p\n", libPinInit_aux);
    libPinInit_aux();
}

void PinInit()
{
    static void (*libPinInit) (void) = NULL;
    char *error;

    printf("shim PinInit()\n");


    if(!libPinInit){
        printf("shim PinInit(): opening a handle to libpin\n");
        dlopen_handle = dlopen("/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libpin_normal/libpin.so", RTLD_LAZY);
        if (!dlopen_handle) {
            fputs(dlerror(), stderr);
            exit(1);
        }
        libPinInit = dlsym(dlopen_handle, "PinInit");
        if((error = dlerror()) != NULL){
            fprintf(stderr, "%s\n", error);
            exit(1);
        }
    }


    // Experiment 1: Just call PinInit. This works.
    //printf("calling PinInit at address %p\n", libPinInit);
    //libPinInit();

    // Experiment 2: Overwite the 0th byte. Then revert it. Then call PinInit.
    // This tests that we can write to that memory. If we can't, it should fail
    // even before we try to call the function.
    // This works, so long as we prep the shared library to have writable text.
    /*printf("writing a 0 byte at PinInit address %p\n", libPinInit);
    char zeroeth_byte = *((char *)libPinInit);
    char *base_addr = (char *) libPinInit;
    base_addr[0] = 0;
    printf("done writing 0. reverting...\n");
    base_addr[0] = zeroeth_byte;
    printf("done reverting. calling...\n");
    libPinInit();*/

    // Experiment 3: copy the function back into place and jump to it.
    // hard code for now...
    FILE *fp;
    char *lib = "pin/libpin.so";
    long lib_size;
    size_t nmemb_read;

    //int text_section_offset = 0xe50;
    //int text_section_size   = 0x668;
    /*void *text_section = malloc(text_section_size);
    fp = fopen(lib, "rb");
    fseek(fp, text_section_offset, 0);
    nmemb_read = fread(text_section, text_section_size, 1, fp);
    assert(nmemb_read == 1);
    fclose(fp);*/

    fp = fopen(lib, "rb");
    fseek(fp, 0, SEEK_END);
    lib_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  //same as rewind(f);

    char *lib_buf = malloc(lib_size + 1);
    nmemb_read = fread(lib_buf, lib_size, 1, fp);
    assert(nmemb_read == 1);
    fclose(fp);


    int func_size = 236;
    int func_offset = 0x1134;
    memcpy(libPinInit, lib_buf+func_offset, func_size);
    printf("shim PinInit(): call real PinInit\n");
    libPinInit();

    return;
}


