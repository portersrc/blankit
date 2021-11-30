
CC=gcc
CFLAGS=-g
CFLAGS+=-c -Wall -fpic #-fpermissive
LIBS = Pin.o

all: mark_writable $(LIBS)

clean:
	@rm -f *.o *.so mark_writable


%.o:%.c
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -shared -fpic -o libpin.so $@

Pin.o:Pin.c

mark_writable:
	gcc mark_writable.c -o mark_writable
