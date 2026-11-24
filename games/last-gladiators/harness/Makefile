CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -I../src -I../../_shared
SRC = ../src/physics.c
SMOKE = smoke_main.c

.PHONY: all run clean

all: smoke

smoke: $(SMOKE) $(SRC) ../src/physics.h
	$(CC) $(CFLAGS) -o smoke $(SMOKE) $(SRC)

run: smoke
	./smoke

clean:
	rm -f smoke
