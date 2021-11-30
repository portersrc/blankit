FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

all: basicmath_small basicmath_large
CC =  /home/amit/1/pbarua/llvm5/cl_build/bin/clang 
CFLAGS = -g -Wl,--rpath=/home/prithayan/projects/libc/bin -Wl,--dynamic-linker=/home/prithayan/projects/libc/bin/lib/ld-linux-x86-64.so.2 -Xclang -load -Xclang /home/amit/1/pbarua/llvm5/cl_build/lib/LLVMpathPredInstrument.so -mllvm -debug -mllvm -debug-only=pathPredInstrument -O2

basicmath_small: ${FILE1} Makefile
	$(CC) -static $(CFLAGS)  ${FILE1} -o basicmath_small -lm
basicmath_large: ${FILE2} Makefile
	$(CC) -static $(CFLAGS)  ${FILE2} -o basicmath_large -lm

clean:
	rm -rf basicmath_small basicmath_large output*
