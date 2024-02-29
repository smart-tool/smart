CC := gcc
uname_m := $(shell $CC -dumpmachine | cut -f1 -d-)
ifneq ($(uname_m),x86_64)
  CFLAGS  := -O3 -Wall
  NON_SSE = source/algos/epsm.c source/algos/ssecp.c source/algos/ssef.c
  ALGOSRC = $(filter-out $(NON_SSE),$(wildcard source/algos/*.c))
else
  CFLAGS  := -O3 -march=native -mtune=native -Wall -Wfatal-errors
  ALGOSRC = $(wildcard source/algos/*.c)
endif
BINS    = $(patsubst source/algos/%,source/bin/%,$(patsubst %.c,%,$(ALGOSRC)))
HELPERS = smart show select test textgen compilesm
TESTS   = bm mp kmp tbm bom so

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
	./select -all block bmh2 bmh4 dfdm sbdm faoso2 blim ssecp

clean:
	rm $(BINS) $(HELPERS)
TAGS: $(ALGOS)
	-rm -f TAGS
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
