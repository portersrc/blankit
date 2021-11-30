FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

#all: libthunk basicmath_small basicmath_large
all: libthunk basicmath_small

basicmath_small: ${FILE1} Makefile
	#gcc -static -O3 ${FILE1} -o basicmath_small -lm -ldl
	$(CC) -S -O0 -g -c $< -o $@.S
	gcc -O0 -g ${FILE1} -o basicmath_small -L../blankit -lm -ldl -lblankit
basicmath_large: ${FILE2} Makefile
	#gcc -static -O3 ${FILE2} -o basicmath_large -lm
	gcc -O3 ${FILE2} -o basicmath_large -lm

libthunk:
	make -C thunk
.PHONY: libthunk

clean:
	make -C thunk clean
	rm -rf basicmath_small basicmath_large output*
