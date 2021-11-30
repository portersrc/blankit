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


void * mmap (void *start, size_t len, int prot, int flags, int fd, off_t offset)
{
    static void * (*glibc_mmap) (void *start, size_t len, int prot, int flags, int fd, off_t offset) = NULL;
    static void * (*ref_glibc_mmap) (void *start, size_t len, int prot, int flags, int fd, off_t offset) = NULL;
    int func_size = 249;
    char *error;

    printf("thunk mmap()\n");

    if(thunk_dlopen_handle == NULL){
        init_thunk();
    }

    printf("thunk mmap(): setting func ptr\n");
    glibc_mmap = dlsym(thunk_dlopen_handle, "mmap");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("thunk mmap(): setting ref func ptr\n");
    ref_glibc_mmap = dlsym(ref_handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    printf("thunk mmap(): copying function back into place\n");
    //memcpy(glibc_mmap, ref_glibc_mmap, func_size);

    printf("thunk mmap(): calling real version\n");
    //return glibc_mmap(start, len, prot, flags, fd, offset);
    return ref_glibc_mmap(start, len, prot, flags, fd, offset);
}

/*int printf (const char *fmt, ...)
{
    static int (*glibc_printf) (const char *fmt, ...) = NULL;
    static int (*ref_glibc_printf) (const char *fmt, ...) = NULL;
    int func_size = 161;
    char *error;

    printf("thunk printf()\n");

    if(thunk_dlopen_handle == NULL){
        init_thunk();
    }

    printf("thunk printf(): setting func ptr\n");
    glibc_printf = dlsym(thunk_dlopen_handle, "printf");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("thunk printf(): setting ref func ptr\n");
    ref_glibc_printf = dlsym(ref_handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    printf("thunk printf(): copying function back into place\n");
    memcpy(glibc_printf, ref_glibc_printf, func_size);

    printf("thunk printf(): calling real version\n");

    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}*/

/*size_t strlen (const char *str)
{
    static size_t (*glibc_strlen) (const char *str) = NULL;
    static size_t (*ref_glibc_strlen) (const char *str) = NULL;
    int func_size = 42;
    char *error;

    printf("thunk strlen()\n");

    if(thunk_dlopen_handle == NULL){
        init_thunk();
    }

    printf("thunk strlen(): setting func ptr\n");
    glibc_strlen = dlsym(thunk_dlopen_handle, "strlen");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("thunk strlen(): setting ref func ptr\n");
    ref_glibc_strlen = dlsym(ref_handle, "strcmp");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("glibc_strlen: %p\n", glibc_strlen);
    printf("ref_glibc_strlen: %p\n", ref_glibc_strlen);

    printf("thunk strlen(): copying function back into place\n");
    //memcpy(glibc_strlen, ref_glibc_strlen, func_size);

    printf("thunk strlen(): calling real version\n");
    //return glibc_strlen(str);
    return ref_glibc_strlen(str);
}*/
