#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <link.h>
#include <dlfcn.h>

void *thunk_dlopen_handle = NULL;
void *ref_handle = NULL;

void init_thunk(void)
{
    printf("thunk init_thunk()\n");
    thunk_dlopen_handle = dlopen("libs/libc/libc-2.23.so", RTLD_LAZY);
    if(!thunk_dlopen_handle){
        fputs(dlerror(), stderr);
        exit(1);
    }
    printf("thunk init_thunk(): thunk_dlopen_handle %p\n", thunk_dlopen_handle);

    ref_handle = dlopen("ref/libc-2.23.so", RTLD_LAZY);
    if(!ref_handle){
        fputs(dlerror(), stderr);
        exit(1);
    }
}

int strcmp(const char *s1, const char *s2)
{
    static int (*glibc_strcmp) (const char *a, const char *b) = NULL;
    static int (*ref_glibc_strcmp) (const char *a, const char *b) = NULL;
    // 6031: 0000000000083b03    47 IFUNC   GLOBAL DEFAULT   13 strcmp
    int func_size = 47;
    char *error;

    printf("thunk strcmp()\n");

    if(thunk_dlopen_handle == NULL){
        init_thunk();
    }

    printf("thunk strcmp(): setting glibc_strcmp\n");
    glibc_strcmp = dlsym(thunk_dlopen_handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("thunk strcmp(): setting ref_glibc_strcmp\n");
    ref_glibc_strcmp = dlsym(ref_handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    printf("thunk strcmp(): copying function back into place\n");
    memcpy(glibc_strcmp, ref_glibc_strcmp, func_size);

    printf("thunk strcmp(): call real strcmp\n");
    return glibc_strcmp(s1, s2);
}
