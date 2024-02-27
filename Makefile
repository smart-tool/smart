CC=gcc
CFLAGS=-O3 -march=native -mtune=native -Wall -Wfatal-errors
ALGOS=$(wildcard source/algos/*.c)
BINS=$(patsubst /algos/,/bin/,$(patsubst %.c,%,$(ALGOS)))
HELPERS=smart show select test textgen compilesm

all: $(BINS) $(HELPERS)

source/bin/%: source/algos/%.c
	$(CC) $(CFLAGS) $< -o $@
%: source/%.c
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
select: source/selectAlgo.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean all
clean:
	rm $(BINS) $(HELPERS)
TAGS: $(ALGOS)
	-mv TAGS TAGS~
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
