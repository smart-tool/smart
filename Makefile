CC=gcc
CFLAGS=-O3 -march=native -mtune=native -Wall -Wfatal-errors
ALGOS=$(wildcard source/algos/*.c)
BINS=$(patsubst source/algos/%,source/bin/%,$(patsubst %.c,%,$(ALGOS)))
HELPERS=smart show select test textgen compilesm

all: $(BINS) $(HELPERS)

source/bin/%: source/algos/%.c
	$(CC) $(CFLAGS) $< -o $@
./%: source/%.c source/*.h
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
select: source/selectAlgo.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: check clean all
check: all
	./select -which
	-cp source/bin/br cp source/bin/br1
	./select -add br1

clean:
	rm $(BINS) $(HELPERS)
TAGS: $(ALGOS)
	-mv TAGS TAGS~
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
