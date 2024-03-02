CC      := gcc
MACHINE := $(shell uname -m)
ARCH    := $(shell ${CC} -dumpmachine | cut -f1 -d-)
BINDIR  = bin
ifneq ($(ARCH),x86_64)
  CFLAGS  := -O3 -Wall
  NON_SSE = source/algos/epsm.c source/algos/ssecp.c source/algos/ssef.c
  ALGOSRC = $(filter-out $(NON_SSE),$(wildcard source/algos/*.c))
else
  CFLAGS  := -O3 -march=native -mtune=native -Wall -Wfatal-errors
  ifeq ($(SANITIZE),1)
    BINDIR = bin/asan
    CFLAGS += -g -Wextra -fsanitize=address,undefined -DBINDIR=\"$(BINDIR)\"
  endif
  ALGOSRC = $(wildcard source/algos/*.c)
endif
ifneq ($(ARCH),$(MACHINE))
  BINDIR = bin/$(ARCH)
  CFLAGS += -DBINDIR=\"$(BINDIR)\"
  DRV = qemu-$(ARCH)
else
  DRV =
endif
BINS    = $(patsubst source/algos/%,$(BINDIR)/%,$(patsubst %.c,%,$(ALGOSRC)))
HELPERS = smart show select test textgen compilesm
TESTS := $(shell shuf -n 6 good.lst)
ifeq ($(TESTS),)
  TESTS = bm mp kmp tbm bom so
endif

all: $(BINS) $(HELPERS)

$(BINDIR)/%: source/algos/%.c source/algos/include/*.h
	@test -d $(BINDIR) || mkdir $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@
./%: source/%.c source/*.h
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
select: source/selectAlgo.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: check clean all lint fmt cppcheck clang-tidy
check: all
	-cp source/algorithms.lst source/algorithms.lst.bak
	$(DRV) ./select -all
	$(DRV) ./select -which | grep br
	-cp $(BINDIR)/br $(BINDIR)/br1
	$(DRV) ./select -add br1
	-rm $(BINDIR)/br1
	$(DRV) ./select -none $(TESTS)
	$(DRV) ./smart -text rand32 -plen 2 4
	$(DRV) ./smart -simple abab chbjhxsscsjndwkjnjdnwelabakdlkewdkklewlkdewlkdnewknabdewab
	for t in $(TESTS); do echo $$t; ./test $$t; done
	$(DRV) ./select -all block bmh2 bmh4 dfdm sbdm faoso2 blim ssecp
	-mv source/algorithms.lst.bak source/algorithms.lst
sanitizer.log: $(ALGOSRC) $(wildcard source/*.c) $(wildcard source/*.h)
	-rm -f sanitizer.log 2>/dev/null
	./sanitizer.sh 2>sanitizer.log
lint: cppcheck clang-tidy
cppcheck:
	cppcheck -j4 -D__linux__ .
compile_commands.json: $(ALGOSRC) $(wildcard source/*.c)
	-+$(MAKE) clean
	bear -- $(MAKE)
clang-tidy: compile_commands.json
	clang-tidy source/*.c source/algos/*.c | tee clang-tidy.log
fmt:
	clang-format -i `find source -name \*.c -o -name \*.h`
clean:
	-rm -f $(BINS) $(HELPERS) 2>/dev/null
TAGS: $(ALGOS)
	-rm -f TAGS 2>/dev/null
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
