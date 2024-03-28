CC      := gcc
MACHINE := $(shell uname -m)
ARCH    := $(shell $(CC) -dumpmachine | cut -f1 -d-)
# to detect mingw
TARGET  := $(shell $(CC) -dumpmachine | cut -f3 -d-)
BINDIR  := bin
ALGOSINC := $(wildcard source/algos/include/*.h)
SRCINC   := $(wildcard source/*.h)
ifneq ($(ARCH),x86_64)
  CFLAGS  := -O3 -Wall
  NON_SSE = source/algos/epsm.c source/algos/ssecp.c source/algos/ssef.c
  ALGOSRC := $(filter-out $(NON_SSE),$(wildcard source/algos/*.c))
else
  CFLAGS  := -O3 -march=native -mtune=native -Wall -Wfatal-errors
  ifeq ($(SANITIZE),1)
    BINDIR = bin/asan
    CFLAGS += -g -Wextra -fsanitize=address,undefined -DBINDIR=\"$(BINDIR)\"
  endif
  ALGOSRC := $(wildcard source/algos/*.c)
endif
ifneq ($(ASSERT),1)
  ifneq ($(SANITIZE),1)
    CFLAGS += -DNDEBUG
  endif
endif
ifneq ($(ARCH),$(MACHINE))
  BINDIR = bin/$(ARCH)
  CFLAGS += -DBINDIR=\"$(BINDIR)\"
  DRV = qemu-$(ARCH)
else
  # debian nonsense calling linux gnu
  ifneq ($(TARGET),gnu)
    ifneq ($(TARGET),$(shell uname -s | tr A-Z a-z))
      BINDIR = bin/$(TARGET)
      CFLAGS += -DBINDIR=\"$(BINDIR)\"
      ifeq ($(TARGET),mingw32)
        ALGOSRC := $(filter-out source/algos/libc1.c,$(wildcard source/algos/*.c))
        DRV = wine
      endif
    endif
  endif
endif
ALLSRC  = $(ALGOSRC) $(wildcard source/*.c) $(SRCINC) $(ALGOSINC)
BINS    = $(patsubst source/algos/%,$(BINDIR)/%,$(patsubst %.c,%,$(ALGOSRC)))
ifeq ($(SANITIZE),1)
  TESTBIN = test-asan
  SMARTBIN = smart-asan
  SELECTBIN = select-asan
  HELPERS = $(SMARTBIN) $(TESTBIN) $(SELECTBIN) compilesm-asan show textgen
else
  TESTBIN = test
  SMARTBIN = smart
  SELECTBIN = select
  HELPERS = $(SMARTBIN) $(TESTBIN) $(SELECTBIN) compilesm show textgen
endif
TESTS := $(shell shuf -n 6 good.lst)
ifeq ($(TESTS),)
  TESTS = bm mp kmp tbm bom so
endif

all: $(BINS) $(HELPERS)

$(BINDIR)/%: source/algos/%.c $(ALGOSINC)
	@test -d $(BINDIR) || mkdir $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@
./%-asan: source/%.c $(SRCINC) source/algos/include/shmids.h
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
./%: source/%.c $(SRCINC) source/algos/include/shmids.h
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
$(SELECTBIN): source/selectAlgo.c $(SRCINC)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: check clean all lint verify fmt cppcheck clang-tidy
check: all
	-cp source/algorithms.lst source/algorithms.lst.bak
	$(DRV) ./$(SELECTBIN) -all
	$(DRV) ./$(SELECTBIN) -which | grep br
	-cp $(BINDIR)/br $(BINDIR)/br1
	$(DRV) ./$(SELECTBIN) -add br1
	-rm $(BINDIR)/br1
	$(DRV) ./$(SELECTBIN) -none $(TESTS)
	$(DRV) ./$(SMARTBIN) -text rand4:rand32 -plen 2 4
	$(DRV) ./$(SMARTBIN) -simple abab chbjhxsscsjndwkjnjdnwelabakdlkewdkklewlkdewlkdnewknabdewab
	for t in $(TESTS); do echo $$t; $(DRV) ./$(TESTBIN) $$t; done
	for t in $(TESTS); do echo $$t rand2 2; $(DRV) ./$(TESTBIN) $$t rand2 2; done
	$(DRV) ./$(SELECTBIN) -all block bmh2 bmh4 dfdm sbdm faoso2 blim ssecp
	-mv algorithms.lst.bak algorithms.lst
lint: cppcheck clang-tidy sanitizer.log
# for newer cppcheck
#CPPCHECK_ARGS="-j4 --enable=warning,portability --inline-suppr --check-level=exhaustive"
CPPCHECK_ARGS = -j4 --enable=warning,portability --inline-suppr
cppcheck:
	cppcheck $(CPPCHECK_ARGS) source/*.c source/algos/*.c
compile_commands.json: GNUmakefile
	-+$(MAKE) clean
	bear -- $(MAKE)
clang-tidy.log: compile_commands.json $(ALLSRC)
	clang-tidy source/*.c source/algos/*.c | sed -e"s,$$PWD/,," | tee clang-tidy.log
clang-tidy: clang-tidy.log
sanitizer.log: $(ALLSRC)
	-rm -f sanitizer.log 2>/dev/null
	-./sanitizer.sh 2>sanitizer.log

CBMC_CHECKS=--bounds-check --pointer-check --memory-leak-check            \
  --div-by-zero-check --signed-overflow-check --unsigned-overflow-check   \
  --pointer-overflow-check --conversion-check --undefined-shift-check     \
  --float-overflow-check --nan-check --enum-range-check
  # cbmc 5.12.1: --pointer-primitive-check
verify:
	for c in $(ALGOSRC); do \
	  echo cbmc -DCBMC --depth 64 $(CBMC_CHECKS) $$c; \
	  cbmc -DCBMC --depth 64 $(CBMC_CHECKS) $$c; \
	done
fmt:
	clang-format -i `find source -name \*.c -o -name \*.h`
clean:
	-rm -f $(BINS) $(HELPERS) 2>/dev/null
TAGS: $(ALLSRC)
	-rm -f TAGS 2>/dev/null
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
