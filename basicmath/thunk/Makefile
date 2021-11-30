all:
	gcc -Wall -O2 -fpic -shared -o thunk.so thunk.c -ldl -D_GNU_SOURCE
clean:
	rm -f *.so
