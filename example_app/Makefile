.PHONY: libpin

CC      = gcc
#CFLAGS  = -std=c99 -O3 
CFLAGS  = -std=c99 -O0 -g
EXE     = example_app
LIBPATH = -L/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libpin_normal
LIBPATH += -L/home/rudy/debloat/pin_probe_debloat_experiments/blankit
LIBS    = -lpin -ldl -lblankit
SRC     = $(shell find -type f -name *.c -print)
OBJ     = $(patsubst %.c,%.o,$(SRC))

all: libpin libshim $(EXE)
.PHONY: all

libpin:
	make -C pin
	cp pin/libpin.so libs/libpin_normal
.PHONY: libpin

libshim:
	make -C shim
.PHONY: libshim

clean:
	make -C pin clean
	make -C shim clean
	@rm -f *.o  $(EXE)
.PHONY: clean


$(EXE) : $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIBPATH) $(LIBS)

%.o:%.c
	$(CC) -S $(CFLAGS) -c $< -o $@.S # yes, it's a .o.S
	$(CC) $(CFLAGS) -c  -o  $@  $<
