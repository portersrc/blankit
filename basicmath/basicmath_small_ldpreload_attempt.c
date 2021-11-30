#include "snipmath.h"
#include <math.h>

#include <link.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


/* The printf's may be removed to isolate just the math calculations */

void debloat_init(void)
{

    //static size_t (*glibc_strlen) (const char *str) = NULL;
    static void * (*glibc_mmap) (void *start, size_t len, int prot, int flags, int fd, off_t offset) = NULL;

    int func_size;
    char *error;

    printf("debloat init called\n");

    void *handle = dlopen("libs/libc/libc-2.23.so", RTLD_LAZY);
    if(!handle){
      fputs(dlerror(), stderr);
      exit(1);
    }

    /*glibc_strlen = dlsym(handle, "strlen");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("debloat_init handle: %p\n", handle);
    printf("debloat_init glibc_strlen: %p\n", glibc_strlen);
    func_size = 42;
    memset(glibc_strlen, 0, func_size);*/

    glibc_mmap = dlsym(handle, "mmap");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    func_size = 249;
    memset(glibc_mmap, 0, func_size);

}

int main(int argc, char *argv[])
{
  double  a1 = 1.0, b1 = -10.5, c1 = 32.0, d1 = -30.0;
  double  a2 = 1.0, b2 = -4.5, c2 = 17.0, d2 = -30.0;
  double  a3 = 1.0, b3 = -3.5, c3 = 22.0, d3 = -31.0;
  double  a4 = 1.0, b4 = -13.7, c4 = 1.0, d4 = -35.0;
  double  x[3];
  double X;
  int     solutions;
  int i;
  unsigned long l = 0x3fed0169L;
  struct int_sqrt q;
  long n = 0;
  
  debloat_init();
  printf("strlen of arg1: %lu\n", strlen(argv[1]));

  /* solve soem cubic functions */
  printf("********* CUBIC FUNCTIONS ***********\n");
  /* should get 3 solutions: 2, 6 & 2.5   */
  SolveCubic(a1, b1, c1, d1, &solutions, x);  
  printf("Solutions:");
  for(i=0;i<solutions;i++)
    printf(" %f",x[i]);
  printf("\n");
  /* should get 1 solution: 2.5           */
  SolveCubic(a2, b2, c2, d2, &solutions, x);  
  printf("Solutions:");
  for(i=0;i<solutions;i++)
    printf(" %f",x[i]);
  printf("\n");
  SolveCubic(a3, b3, c3, d3, &solutions, x);
  printf("Solutions:");
  for(i=0;i<solutions;i++)
    printf(" %f",x[i]);
  printf("\n");
  SolveCubic(a4, b4, c4, d4, &solutions, x);
  printf("Solutions:");
  for(i=0;i<solutions;i++)
    printf(" %f",x[i]);
  printf("\n");
  /* Now solve some random equations */
  for(a1=1;a1<10;a1++) {
    for(b1=10;b1>0;b1--) {
      for(c1=5;c1<15;c1+=0.5) {
	for(d1=-1;d1>-11;d1--) {
	  SolveCubic(a1, b1, c1, d1, &solutions, x);  
	  printf("Solutions:");
	  for(i=0;i<solutions;i++)
	    printf(" %f",x[i]);
	  printf("\n");
	}
      }
    }
  }
  
  printf("********* INTEGER SQR ROOTS ***********\n");
  /* perform some integer square roots */
  for (i = 0; i < 1001; ++i)
    {
      usqrt(i, &q);
			// remainder differs on some machines
     // printf("sqrt(%3d) = %2d, remainder = %2d\n",
     printf("sqrt(%3d) = %2d\n",
	     i, q.sqrt);
    }
  usqrt(l, &q);
  //printf("\nsqrt(%lX) = %X, remainder = %X\n", l, q.sqrt, q.frac);
  printf("\nsqrt(%lX) = %X\n", l, q.sqrt);


  printf("********* ANGLE CONVERSION ***********\n");
  /* convert some rads to degrees */
  for (X = 0.0; X <= 360.0; X += 1.0)
    printf("%3.0f degrees = %.12f radians\n", X, deg2rad(X));
  puts("");
  for (X = 0.0; X <= (2 * PI + 1e-6); X += (PI / 180))
    printf("%.12f radians = %3.0f degrees\n", X, rad2deg(X));
  
  
  return 0;
}
