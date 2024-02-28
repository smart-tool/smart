CC=gcc
CFLAGS=-O3 -march=native -mtune=native -Wall -Wfatal-errors
ALGOS=$(wildcard source/algos/*.c)
BINS=$(patsubst source/algos/%,source/bin/%,$(patsubst %.c,%,$(ALGOS)))
HELPERS=smart show select test textgen compilesm
TESTS=bm mp kmp tbm bom so
uname_p := $(shell uname -p)
ifneq ($(uname_p),x86_64)
  NON_SSE=epsm ssecp ssef
  BINS=$(filter-out $(NON_SSE),$(patsubst source/algos/%,source/bin/%,$(patsubst %.c,%,$(ALGOS))))
endif

all: $(BINS) $(HELPERS)

source/bin/%: source/algos/%.c
	$(CC) $(CFLAGS) $< -o $@
./%: source/%.c source/*.h
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
select: source/selectAlgo.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: check clean all
check: all
	./select -which | grep br
	-cp source/bin/br source/bin/br1
	./select -add br1
	-rm source/bin/br1
	./select -none $(TESTS)
	./smart -text rand32
	for t in $(TESTS); do echo $$t; ./test $$t; done
	./select -all block bmh2 bmh4 ssef dfdm sbdm faoso2 blim ssecp

clean:
	rm $(BINS) $(HELPERS)
TAGS: $(ALGOS)
	-mv TAGS TAGS~
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
