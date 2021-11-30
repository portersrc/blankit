#include "snipmath.h"
#include <math.h>
#include "../blankit/blankit.h"

/* The printf's may be removed to isolate just the math calculations */

int main(void)
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

  /* solve soem cubic functions */
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("********* CUBIC FUNCTIONS ***********\n");
  /* should get 3 solutions: 2, 6 & 2.5   */
  SolveCubic(a1, b1, c1, d1, &solutions, x);  
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("Solutions:");
  for(i=0;i<solutions;i++){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf(" %f",x[i]);
  }
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("\n");
  /* should get 1 solution: 2.5           */
  SolveCubic(a2, b2, c2, d2, &solutions, x);  
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("Solutions:");
  for(i=0;i<solutions;i++){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf(" %f",x[i]);
  }
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("\n");
  SolveCubic(a3, b3, c3, d3, &solutions, x);
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("Solutions:");
  for(i=0;i<solutions;i++){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf(" %f",x[i]);
  }
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("\n");
  SolveCubic(a4, b4, c4, d4, &solutions, x);
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("Solutions:");
  for(i=0;i<solutions;i++){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf(" %f",x[i]);
  }
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("\n");
  /* Now solve some random equations */
  for(a1=1;a1<10;a1++) {
    for(b1=10;b1>0;b1--) {
      for(c1=5;c1<15;c1+=0.5) {
        for(d1=-1;d1>-11;d1--) {
          SolveCubic(a1, b1, c1, d1, &solutions, x);  
          blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
          printf("Solutions:");
          for(i=0;i<solutions;i++){
            blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
            printf(" %f",x[i]);
          }
          blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
          printf("\n");
        }
      }
    }
  }
  
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("********* INTEGER SQR ROOTS ***********\n");
  /* perform some integer square roots */
  for (i = 0; i < 1001; ++i){
      usqrt(i, &q);
     // remainder differs on some machines
     // printf("sqrt(%3d) = %2d, remainder = %2d\n",
     blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
     printf("sqrt(%3d) = %2d\n", i, q.sqrt);
  }
  usqrt(l, &q);
  //printf("\nsqrt(%lX) = %X, remainder = %X\n", l, q.sqrt, q.frac);
  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("\nsqrt(%lX) = %X\n", l, q.sqrt);


  blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  printf("********* ANGLE CONVERSION ***********\n");
  /* convert some rads to degrees */
  for (X = 0.0; X <= 360.0; X += 1.0){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf("%3.0f degrees = %.12f radians\n", X, deg2rad(X));
  }
  blankit_predict("putchar;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
  puts("");
  for (X = 0.0; X <= (2 * PI + 1e-6); X += (PI / 180)){
    blankit_predict("_IO_printf;_IO_do_write;_IO_file_overflow;_IO_file_write;_IO_file_xsputn;_IO_printf;_IO_vfprintf;__overflow;__printf_fp;putchar;strchrnul;strlen;write");
    printf("%.12f radians = %3.0f degrees\n", X, rad2deg(X));
  }
  
  
  return 0;
}
