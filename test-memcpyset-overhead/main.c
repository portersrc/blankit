#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define CHUNK_SIZE   (1<<30)
//#define BUF_SIZE   (10)
#define BUF_SIZE_MAX (1<<20)

long start_global, end_global;

static inline
long timestamp_ns(void)
{
    struct timespec tv = {0};
    if(clock_gettime(CLOCK_MONOTONIC, &tv) != 0){
        printf("timestamp_ns: timer error \n");
        return 0;
    }
    return tv.tv_sec * 1000000000 + tv.tv_nsec;
}

//int __attribute__ ((noinline)) foo(int n)
int foo(int n)
{
    int a;
    start_global = timestamp_ns();
    a = 1;
    for(int i = 0; i < n; i++){
        a *= rand();
    }
    end_global = timestamp_ns();
    return a;
}



int main(void)
{
    char *chunk;
    int offset;
    long start, end;

    //start = timestamp_ns();
    //printf("hi\n");
    //end = timestamp_ns();
    //printf("%ld\n", end - start);

    //srand(time(NULL));   // Initialization, should only be called once.
    char *chunk_src = malloc(CHUNK_SIZE);
    char *chunk_dst = malloc(CHUNK_SIZE);

    for(int buf_size = 1; buf_size <= BUF_SIZE_MAX; buf_size<<=1){
        offset = (rand() % CHUNK_SIZE) - buf_size;      // Returns a pseudo-random integer between 0 and RAND_MAX.
        if(offset >= buf_size){
            offset -= buf_size;
        }
        assert(buf_size <= CHUNK_SIZE);


        start = timestamp_ns();
        memcpy(chunk_dst+offset, chunk_src+offset, buf_size);
        end = timestamp_ns();
        end = end - start; // hack

        start = timestamp_ns();
        printf("time to memcpy %d bytes: %ldns\n", buf_size, end);
        end = timestamp_ns();
        printf("  printf time: %ld\n", end - start);
    }

    printf("\n");

    for(int buf_size = 1; buf_size <= BUF_SIZE_MAX; buf_size<<=1){
        offset = (rand() % CHUNK_SIZE) - buf_size;      // Returns a pseudo-random integer between 0 and RAND_MAX.
        if(offset >= buf_size){
            offset -= buf_size;
        }
        assert(buf_size <= CHUNK_SIZE);


        start = timestamp_ns();
        memset(chunk_dst+offset, 0, buf_size);
        end = timestamp_ns();

        printf("time to memset %d bytes: %ldns\n", buf_size, end - start);
    }

    int rv;
    //offset = rand();

    start = timestamp_ns();
    rv = foo(1000);
        /*int a;
        start_global = timestamp_ns();
        a = 1;
        for(int i = 0; i < 1000; i++){
            a *= rand();
        }
        end_global = timestamp_ns();
        rv = a;*/
    end = timestamp_ns();
    printf("inner foo: %ldns\n", end_global - start_global);
    printf("outer foo: %ldns\n", end - start);


    return 0;
}
