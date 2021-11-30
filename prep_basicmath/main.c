#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>
#include "pin/Pin.h"

int A[100000];

void debloat_init(void)
{
    static int (*glibc_strcmp) (const char *a, const char *b) = NULL;
    int func_size;
    char *error;

    printf("debloat_init()\n");

    void *handle = dlopen("libs/libc/libc-2.23.so", RTLD_LAZY);
    if(!handle){
      fputs(dlerror(), stderr);
      exit(1);
    }

    glibc_strcmp = dlsym(handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    func_size = 47;
    memset(glibc_strcmp, 0, func_size);

    printf("debloat_init(): handle %p\n", handle);
    // XXX Do NOT close handle. Because we cannot share the handle between
    // the application code and the thunk library, the only way the zero'd
    // bytes will persist is if the library stays open. 
    // The call to dlopen in the thunk library returns the same handle
    // (just look at the matching pointers), and that's why this all works.
}


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
    debloat_init();

    if(argc < 2 ){
          printf(" Usage: testexe #num \n");
      return 0;
    }
    int n = atoi(argv[1]);
    A[0] = 0; A[1] = 1; A[2] = 2;
    PinInit();
    int fibV = fib(n);
    printf(" Fib %d = %d\n", n, fibV);
    int SumV = Summ(n);
    printf(" Summ %d = %d\n", n, SumV);
    //printf(" Summ B = %d", n, B[n][n]);
    PinFree();
    return 0;
}
