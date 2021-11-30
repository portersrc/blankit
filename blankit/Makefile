
CC         := gcc
CFLAGS     := -Wall -fpic -O3 #-fpermissive
CFLAGS_DBG := -Wall -fpic -g -O0 -DBLANKIT_DBG
LIB = libblankit.so

SRC    :=   $(shell find -type f -name \*.c -print)
OBJ    :=   $(patsubst %.c,%.o,$(SRC))

all: $(LIB)
.PHONY: all

debug: CFLAGS := $(CFLAGS_DBG)
debug: all

clean:
	@rm -f *.o *.so
.PHONY: clean

$(LIB): $(OBJ)
	$(CC) -shared -fpic -o $@ $^

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

