CC      = gcc
#CFLAGS  = -std=c99 -O3 
CFLAGS  = -std=c99 -O0 -g
EXE     = prep_basicmath
LIBPATH = -Llibs/libpin
LIBS    = -lpin -ldl
SRC     = $(shell find -type f -name *.c -print)
OBJ     = $(patsubst %.c,%.o,$(SRC))

all: libpin libthunk $(EXE)
.PHONY: all

libpin:
	make -C pin
	cp pin/libpin.so libs/libpin
	cp ../libc/libc-2.23.so libs/libc
.PHONY: libpin

libthunk:
	make -C thunk
.PHONY: libthunk

clean:
	make -C pin clean
	make -C thunk clean
	@rm -f *.o  $(EXE) debug_bytes_* *.o.S libs/libpin/*.so libs/libc/*.so
.PHONY: clean


$(EXE) : $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIBPATH) $(LIBS)

%.o:%.c
	$(CC) -S $(CFLAGS) -c $< -o $@.S # yes, it's a .o.S
	$(CC) $(CFLAGS) -c  -o  $@  $< -g
