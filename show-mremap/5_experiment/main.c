#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>

/*void debloat_init(void)
{
    //FILE *fp;
    printf("debloat_init()\n");

    void *handle = dlopen("/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libpin_normal/libpin.so", RTLD_LAZY);
    if(!handle){
      fputs(dlerror(), stderr);
      exit(1);
    }
    static void (*libPinInit) (void) = NULL;
    char *error;
    libPinInit = dlsym(handle, "PinInit");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    // Experiment 3: zero out the function we want to test
    int func_size = 236;
    char *base_addr = (char *) libPinInit;
    memset(base_addr, 0, func_size);
    //dlclose(handle);




    // Experiment 4: zero out strcmp, as well.
    // dlopen() fails for the libc girish provided.  Following this example
    // works:
    //   https://stackoverflow.com/questions/6841311/using-dlopen-to-get-handle-of-libc-memory-allocation-functions
    // this fails:
    //void *handle_two = dlopen("/home/rudy/debloat/pin_probe_debloat_experiments/libc/libc.so", RTLD_LAZY);
    // this works:
    //void *handle_two = dlopen("libc.so.6", RTLD_LAZY);

    // this works, too. it's the same as the simple version above, but with
    // a hard-copied local version
    void *handle_two = dlopen("/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libc/libc-2.23.so", RTLD_LAZY);
    if(!handle_two){
      fputs(dlerror(), stderr);
      exit(1);
    }
    static int (*glibc_strcmp) (const char *a, const char *b) = NULL;
    glibc_strcmp = dlsym(handle_two, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    // 6031: 0000000000083b03    47 IFUNC   GLOBAL DEFAULT   13 strcmp
    func_size = 47;
    base_addr = (char *) glibc_strcmp;
    FILE *debug_bytes_fp;
    debug_bytes_fp = fopen("debug_bytes_at_sym", "wb");
    size_t nmemb_written = fwrite(base_addr, func_size, 1, debug_bytes_fp);
    assert(nmemb_written == 1);
    fclose(debug_bytes_fp);
    memset(base_addr, 0, func_size);

}*/


void zero_out(void)
{
    FILE *fp;
    void *null_bytes;
    void *backup;

    int offset = 0x610;
    int size   = 12;
    size_t retval;


    backup = malloc(size);

    null_bytes = malloc(size);
    memset(null_bytes, 0, size);

    fp = fopen("mark_writable/libfoo.so", "r+b");

    // read out the function's bytes
    fseek(fp, offset, 0);
    retval = fread(backup, size, 1, fp);
    assert(retval == 1);
    rewind(fp);

    // zero out the function
    fseek(fp, offset, 0);
    fwrite(null_bytes, size, 1, fp);
    rewind(fp);

    // write the function's bytes back in
    fseek(fp, offset, 0);
    fwrite(backup, size, 1, fp);
    rewind(fp);

    fclose(fp);

}



// see helper/offset_and_size.sh
//   foo 0000000000000610 12


int main(int argc, char **argv)
{
    void (*foo_handle)(void);

    printf("main\n");

    zero_out();
    void *handle = dlopen("mark_writable/libfoo.so", RTLD_LAZY);
    foo_handle = dlsym(handle, "foo");
    foo_handle();

    printf("done -- should see 'inside foo' printed above\n");
    return 0;
}
