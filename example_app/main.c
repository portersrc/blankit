#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>
#include "pin/Pin.h"
#include "../blankit/blankit.h"

int A[100000];

// XXX: Not able to define dlopen_handle handle across the app and the
// shim. It's a linkage problem. The app doesn't know about the shim until
// it's preloaded. The shim can get built without a definition for
// dlopen-handle, but then it fails at runtime when it's preloaded. It can't
// find it. So, for now, we'll dl-open every time, and in the real thing,
// we can figure out what to do.

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


int fib(int n)
{
    Pin(0.0);
    for(int i = 3; i <= n; i++){
        A[i] = A[i-1] + A[i-2] + A[1] + A[2];
    }
    UnPin();

    // Make a useless call to a glibc function to show that we can use shims
    // on glibc. Note: If the strcmp is too simple, (e.g. "hello" vs. "world"),
    // gcc optimizes away the call, and so the shim won't be called either.
    char *mystr = malloc(64);
    sprintf(mystr, "hello%d", A[3]);
    if(strcmp(mystr, "world")){
        printf("%s and world are not the same\n", mystr);
    }else{
        printf("impossible: %s and world are the same\n", mystr);
    }

    return A[n];
}

//int B[1000][1000];
int Summ(int n)
{
    Pin(0.0);
    for(int i = 0; i <= n; i++){
        for(int j = 0; j <= n; j++){
            A[i] = A[i] + A[j];
            //B[i][i] = B[i][i] + B[i][j];
        }    
    }
    UnPin();
    return A[n];
}

int main(int argc, char **argv)
{
    if(argc < 2 ){
          printf(" Usage: testexe #num \n");
      return 0;
    }
    int n = atoi(argv[1]);
    A[0] = 0; A[1] = 1; A[2] = 2;
    blankit_predict("PinInit;PinInit_aux;");
    PinInit();
    int fibV = fib(n);
    printf(" Fib %d = %d\n", n, fibV);
    int SumV = Summ(n);
    printf(" Summ %d = %d\n", n, SumV);
    //printf(" Summ B = %d", n, B[n][n]);
    PinFree();

    printf("returning from main from example app\n");
    return 0;
}
