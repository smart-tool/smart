CC      := gcc
MACHINE := $(shell uname -m)
# sdcc isn't a GCC-compatible driver: it has no -dumpmachine (and would
# print its --help text into ARCH/TARGET/TARGET_OS if asked), so detect
# it by name and skip GNU-triple detection entirely for it.
ifneq ($(findstring sdcc,$(CC)),)
  ARCH      := stm8
  TARGET    := sdcc
  TARGET_OS := none
else
  ARCH    := $(shell $(CC) -dumpmachine | cut -f1 -d-)
  # to detect mingw
  TARGET  := $(shell $(CC) -dumpmachine | cut -f3 -d-)
  # GNU triples encode "no OS" as the 2nd component (e.g. arm-none-eabi);
  # together with ARCH==avr (avr-gcc's -dumpmachine is a bare "avr" with
  # no OS component at all) this identifies freestanding/bare-metal
  # targets.
  TARGET_OS := $(shell $(CC) -dumpmachine | cut -f2 -d-)
endif
TIMEOUT_1m := timeout --preserve-status 1m
TIMEOUT_4m := timeout 4m
TIMEOUT_30s := timeout 30s
ifeq (, $(shell command -v timeout))
  TIMEOUT_1m =
  TIMEOUT_4m =
  TIMEOUT_30s =
endif
BINDIR   := bin
ALGOSINC := $(wildcard source/algos/include/*.h)
SRCINC   := $(wildcard source/*.h)
ifeq ($(ASAN),1)
  SANITIZE=1
endif
ifneq ($(filter aarch64 arm64,$(ARCH)),)
  CFLAGS  := -O3 -Wall
  NON_SSE = source/algos/ssef.c source/algos/simdkr.c
  ALGOSRC := $(filter-out $(NON_SSE),$(wildcard source/algos/*.c))
else
ifneq ($(ARCH),x86_64)
  CFLAGS  := -O3 -Wall
  NON_SSE = source/algos/epsm.c source/algos/ssecp.c source/algos/ssef.c source/algos/simdkr.c
  ALGOSRC := $(filter-out $(NON_SSE),$(wildcard source/algos/*.c))
else
  CFLAGS  := -O3 -march=native -mtune=native -Wall -Wfatal-errors
  ifeq ($(SANITIZE),1)
    BINDIR = bin/asan
    CFLAGS += -O1 -g -Wextra -fsanitize=address,undefined -fno-sanitize-recover=all -DBINDIR=\"$(BINDIR)\"
  endif
  ifeq ($(DEBUG),1)
    CFLAGS += -O0 -DDEBUG
  endif
  ifeq ($(FUZZ),1)
    CC = afl-clang-lto
    BINDIR = bin/fuzz
    CFLAGS = -Og -g -Isource/algos -fsanitize=address,undefined -fno-sanitize-recover=all -march=native -mtune=native -DFUZZ -DBINDIR=\"$(BINDIR)\"
  endif
  ALGOSRC := $(wildcard source/algos/*.c)
endif
endif
ifeq ($(ARCH),avr)
  FREESTANDING := 1
endif
ifeq ($(TARGET_OS),none)
  FREESTANDING := 1
endif
ifeq ($(FREESTANDING),1)
  # bare-metal targets have no OS and no full libc: disable the compiler's
  # hosted-environment assumptions, matching the __STDC_HOSTED__==0 checks
  # used throughout source/algos/include/*.h to gate POSIX-only code.
  CFLAGS += -ffreestanding
  ifeq ($(ARCH),avr)
    # avr-gcc targets a bare-metal 8-bit MCU (avr-libc): default to a
    # common Arduino-class device; override via MCU=<device> for others.
    MCU ?= atmega328p
    CFLAGS += -mmcu=$(MCU)
    # algorithms whose preprocessing tables (typically SIGMA*SIGMA or
    # DSIGMA sized) exceed avr-gcc's 32767-byte single-object limit, or
    # whose combined locals overflow SRAM on any AVR device: cannot be
    # built for this target.
    NON_AVR = source/algos/bfs.c source/algos/blim.c source/algos/bmh4.c \
      source/algos/bom2.c source/algos/bql.c source/algos/br.c \
      source/algos/bram3.c source/algos/bram5.c source/algos/bram7.c \
      source/algos/bsdm4.c source/algos/bsdm6.c source/algos/bsdm7.c \
      source/algos/bsdm8.c source/algos/colussi.c source/algos/dfdm.c \
      source/algos/ebom.c source/algos/fbom.c source/algos/fdm.c \
      source/algos/ffs.c source/algos/gg.c source/algos/ildm1.c \
      source/algos/ildm2.c source/algos/jom.c source/algos/kbndm.c \
      source/algos/ksa.c source/algos/ldm.c source/algos/ms.c \
      source/algos/qf72.c source/algos/rcolussi.c source/algos/rf.c \
      source/algos/sbdm.c source/algos/sebom.c source/algos/sfbom.c \
      source/algos/skip4.c source/algos/skip7.c source/algos/skip8.c \
      source/algos/trf.c source/algos/tsw.c source/algos/tvsbs.c \
      source/algos/tvsbs-w2.c source/algos/tvsbs-w4.c source/algos/tvsbs-w6.c \
      source/algos/tvsbs-w8.c source/algos/ww.c source/algos/zt.c
    ALGOSRC := $(filter-out $(NON_AVR),$(ALGOSRC))
  endif
  ifeq ($(ARCH),arm)
    # bare-metal ARM (arm-none-eabi, newlib, no OS): default to a common
    # Cortex-M0 (ARMv6-M) device; override via MCPU=<cpu> for others, e.g.
    # MCPU=cortex-m3 (ARMv7-M). nano.specs/nosys.specs pull in newlib-nano
    # and stub syscalls (_write/_sbrk/...) so printf/malloc link without an
    # OS underneath; every algorithm except the SSE-only simdkr (already
    # excluded above) builds fine since ARM has no AVR-style object-size
    # limit or default SRAM-region enforcement at link time.
    MCPU ?= cortex-m0
    CFLAGS += -mcpu=$(MCPU) -mthumb --specs=nano.specs --specs=nosys.specs
  endif
  ifeq ($(ARCH),stm8)
    # SDCC isn't GCC: -Wall isn't a warning switch here, it's -W(a)ll,
    # i.e. "pass the raw flag 'll' through to the assembler" (SDCC
    # overloads -W<p|a|l><opts> for the preprocessor/assembler/linker),
    # which breaks the assembler invocation; -O3/-ffreestanding are just
    # silently ignored. Reset CFLAGS instead of inheriting them. -mstm8
    # selects the STM8 backend (SDCC has no -mmcu/-mcpu style device
    # selection); __STDC_HOSTED__ must be forced to 0 directly since
    # -ffreestanding is a no-op here.
    CFLAGS := -mstm8 -D__STDC_HOSTED__=0
    # SDCC's minimal libc lacks memmem() (libc1), rejects variable-length
    # arrays (lhc1..8), and crashes on __builtin_ffs (svm0, a GCC-only
    # builtin). The rest are the same class of problem as NON_AVR above:
    # SIGMA*SIGMA/DSIGMA-sized tables that don't fit an 8-bit MCU, except
    # SDCC reports it as either a hard "Cannot allocate variable"/"array
    # is negative" error, or (worse) a pathological multi-minute hang in
    # its register allocator instead of AVR's immediate object-size error.
    NON_STM8 = source/algos/ag.c source/algos/askip.c source/algos/bfs.c \
      source/algos/blim.c source/algos/bm.c source/algos/bmh4.c \
      source/algos/bom2.c source/algos/bql.c source/algos/br.c \
      source/algos/bram3.c source/algos/bram5.c source/algos/bram7.c \
      source/algos/bsdm2.c source/algos/bsdm3.c source/algos/bsdm4.c \
      source/algos/bsdm5.c source/algos/bsdm6.c source/algos/bsdm7.c \
      source/algos/bsdm8.c source/algos/colussi.c source/algos/dbww.c \
      source/algos/dbww2.c source/algos/dfdm.c source/algos/ebom.c \
      source/algos/fbom.c source/algos/fdm.c source/algos/ffs.c \
      source/algos/ft3.c source/algos/gg.c source/algos/ildm1.c \
      source/algos/ildm2.c source/algos/jom.c source/algos/kbndm.c \
      source/algos/kmp.c source/algos/kmpskip.c source/algos/ksa.c \
      source/algos/ldm.c source/algos/lhc1.c source/algos/lhc2.c \
      source/algos/lhc3.c source/algos/lhc4.c source/algos/lhc5.c \
      source/algos/lhc6.c source/algos/lhc7.c source/algos/lhc8.c \
      source/algos/libc1.c source/algos/mp.c source/algos/qf53.c \
      source/algos/qf72.c source/algos/rcolussi.c source/algos/rf.c \
      source/algos/sbdm.c source/algos/sbndmq8.c source/algos/sebom.c \
      source/algos/sfbom.c source/algos/skip4.c source/algos/skip7.c \
      source/algos/skip8.c source/algos/svm0.c source/algos/tbm.c \
      source/algos/trf.c source/algos/tsa.c source/algos/tsa-q2.c \
      source/algos/tso5.c source/algos/tsw.c source/algos/tvsbs.c \
      source/algos/tvsbs-w2.c source/algos/ww.c source/algos/zt.c
    ALGOSRC := $(filter-out $(NON_STM8),$(ALGOSRC))
  endif
  endif

ifneq ($(ASSERT),1)
  ifneq ($(SANITIZE),1)
    ifneq ($(FUZZ),1)
      CFLAGS += -DNDEBUG
    endif
  endif
endif
ifeq ($(NOSHM),1)
  CFLAGS += -DNOSHM
endif
ifneq ($(ARCH),$(MACHINE))
  ifeq ($(BINDIR),bin)
    BINDIR = bin/$(ARCH)
    CFLAGS += -DBINDIR=\"$(BINDIR)\"
  endif
  DRV = qemu-$(ARCH)
else
  # debian nonsense calling linux gnu
  ifneq ($(TARGET),gnu)
    OS := $(shell uname -s | tr A-Z a-z)
    ifeq ($(OS),darwin)
      ifeq ($(shell echo $(TARGET)|cut -c1-6),darwin)
	TARGET=darwin
      endif
    endif
    ifneq ($(TARGET),$(OS))
      ifeq ($(BINDIR),bin)
        BINDIR = bin/$(TARGET)
        CFLAGS += -DBINDIR=\"$(BINDIR)\"
      endif
      ifeq ($(TARGET),mingw32)
        ALGOSRC := $(filter-out source/algos/libc1.c,$(wildcard source/algos/*.c))
        DRV = wine
      else
	DRV = qemu-$(ARCH)
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
  HELPERS = $(SMARTBIN) $(TESTBIN) $(SELECTBIN) algocfg compilesm-asan show textgen
else
ifeq ($(FUZZ),1)
  TESTBIN = test-fuzz
  SMARTBIN = smart-fuzz
  SELECTBIN = select
  HELPERS = $(SMARTBIN) $(TESTBIN) $(SELECTBIN) algocfg compilesm-fuzz show textgen
else
  TESTBIN = test
  SMARTBIN = smart
  SELECTBIN = select
  HELPERS = $(SMARTBIN) $(TESTBIN) $(SELECTBIN) algocfg compilesm show textgen 
endif
endif
TESTS := $(shell shuf -n 10 good.lst)
ifeq ($(TESTS),)
  TESTS = hor mp kmp musl1 tbm so ssm qf33 twfr3 fndm
endif
COMPILE = $(CC) -c $(CFLAGS)

ifeq ($(FREESTANDING),1)
# smart/test/select/algocfg/compilesm/show/textgen are host orchestration
# tools (dirent.h, fork/exec, wall-clock timing): they cannot be built or
# run on a freestanding target, so only the algorithm binaries and not the
# good.lst/asan.lst helpers (which require running algocfg) apply.
all: $(BINS)
else
all: $(HELPERS) $(BINS) good.lst asan.lst
endif
.DEFAULT_GOAL := all

DATA_RAND = data/rand2/rand2.txt data/rand4/rand4.txt data/rand8/rand8.txt \
  data/rand16/rand16.txt data/rand32/rand32.txt data/rand64/rand64.txt \
  data/rand128/rand128.txt data/rand256/rand256.txt

# textgen writes all 8 files in a single run; the stamp avoids re-running
# it once per missing file and lets this work under plain POSIX make.
$(DATA_RAND): data/.textgen-stamp
data/.textgen-stamp: textgen
	$(DRV) ./textgen
	@touch $@

ifneq ($(FREESTANDING),1)
$(BINDIR)/hpbm: CFLAGS += -fopenmp
endif

$(BINDIR)/%: source/algos/%.c $(ALGOSINC) GNUmakefile
	@test -d $(BINDIR) || mkdir $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@
./%-fuzz: source/%.c $(SRCINC) source/algos/include/shmids.h GNUmakefile
	$(CC) $(CFLAGS) -DFUZZ $< -std=gnu99 -o $@ -lm
./%-asan: source/%.c $(SRCINC) source/algos/include/shmids.h GNUmakefile
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
./%: source/%.c $(SRCINC) source/algos/include/shmids.h GNUmakefile
	$(CC) $(CFLAGS) $< -std=gnu99 -o $@ -lm
$(SELECTBIN): source/selectAlgo.c $(SRCINC) GNUmakefile
	$(CC) $(CFLAGS) $< -o $@
verify/%.vfy: source/algos/%.c $(ALGOSINC) algocfg GNUmakefile
	b=`basename $@ .vfy`; echo -n "cmd="; ./algocfg $$b cbmc; \
	  cmd=`./algocfg $$b cbmc`; \
	  echo $$cmd $(CBMC_ARGS) $< > $@; \
	  $$cmd $(CBMC_ARGS) $< 2>&1 | tee -a $@
	if grep UNSATISFIABLE $@ >/dev/null; then \
	  echo | tee -a $@; b=`basename $@ .vfy`; \
	  echo goto-analyzer -Isource/algos -DCBMC --verify --recursive-interprocedural source/algos/$$b.c | tee -a $@; \
	  goto-analyzer -Isource/algos -DCBMC --verify --recursive-interprocedural source/algos/$$b.c | tee -a $@; \
          $(MAKE) verify/$$b.vfy-trace; \
	fi
CMBC_TRACE_ARGS = --trace --reachability-slice-fb
verify/%.vfy-trace: source/algos/%.c $(ALGOSINC) algocfg GNUmakefile
	b=`basename $@ .vfy-trace`; echo -n "cmd="; ./algocfg $$b cbmc; \
	  cmd=`./algocfg $$b cbmc`; \
	  echo $$cmd $(CMBC_TRACE_ARGS) $(CBMC_ARGS) $< > $@; \
	  $$cmd $(CMBC_TRACE_ARGS) $(CBMC_ARGS) $< 2>&1 | tee -a $@
	if grep UNSATISFIABLE $@ >/dev/null; then \
	  echo | tee -a $@; b=`basename $@ .vfy`; \
	  echo goto-analyzer -Isource/algos -DCBMC --verify --recursive-interprocedural source/algos/$$b.c | tee -a $@; \
	  goto-analyzer -Isource/algos -DCBMC --verify --recursive-interprocedural source/algos/$$b.c | tee -a $@; \
	fi

.PHONY: check clean all lint verify check-verify verify-trace fmt cppcheck clang-tidy fuzz
check: all $(DATA_RAND)
	@-cp algorithms.lst algorithms.lst.bak
	$(DRV) ./$(SELECTBIN) -all
	$(DRV) ./$(SELECTBIN) -which | grep br
	-cp $(BINDIR)/br $(BINDIR)/br1
	$(DRV) ./$(SELECTBIN) -add br1
	-rm $(BINDIR)/br1
	$(DRV) ./$(SELECTBIN) -none $(TESTS)
	$(DRV) ./$(SMARTBIN) -text rand4:rand32 -plen 2:4:16
	$(DRV) ./$(SMARTBIN) -simple abab chbjhxsscsjndwkjnjdnwelabakdlkewdkklewlkdewlkdnewknabdewab
	for t in $(TESTS); do echo $$t -nv; $(DRV) ./$(TESTBIN) $$t -nv; done
	for t in $(TESTS); do echo $$t -nv rand2 2; $(DRV) ./$(TESTBIN) $$t -nv rand2 2; done
	$(DRV) ./$(SELECTBIN) -all
	@-test -f algorithms.lst.bak && mv algorithms.lst.bak algorithms.lst
lint: algocfg cppcheck clang-tidy sanitizer.log
# for newer cppcheck
#CPPCHECK_ARGS="-j4 --enable=warning,portability --inline-suppr --check-level=exhaustive"
CPPCHECK_ARGS = -j4 --enable=warning,portability --inline-suppr
cppcheck:
	cppcheck $(CPPCHECK_ARGS) source/*.c source/algos/*.c
compile_commands.json: GNUmakefile
	+$(MAKE) clean
	bear -- $(MAKE)
clang-tidy.log: compile_commands.json $(ALLSRC)
	clang-tidy source/*.c source/algos/*.c | sed -e"s,$$PWD/,," | tee clang-tidy.log
clang-tidy: clang-tidy.log
sanitizer.log: $(ALLSRC)
	-rm -f sanitizer.log 2>/dev/null
	-./sanitizer.sh 2>sanitizer.log
tests.lst: $(ALLSRC)
	for t in `cat algos.lst`; do ./test "$$t"; done | tee $@
tests-asan.lst: $(ALLSRC)
	+$(MAKE) ASAN=1
	for t in `cat algos.lst`; do ./test-asan "$$t"; done | tee $@
good.lst: algocfg
	./algocfg good | tr ' ' '\n' >$@
asan.lst: algocfg
	./algocfg good 0 | perl -nle'%bad=map{$$_=>1}split/ /;for(split/ /,qx"./algocfg asan"){print $$_ if $$_ ne "\n" and !$$bad{$$_}}' >$@

ifeq (esbmc, $(CBMC))
CBMC_ARGS = -Isource/algos -DCBMC -DESBMC
else
CBMC_ARGS = -Isource/algos -DCBMC --bounds-check --pointer-check --memory-leak-check \
  --div-by-zero-check --signed-overflow-check --unsigned-overflow-check   \
  --pointer-overflow-check --conversion-check --undefined-shift-check     \
  --float-overflow-check --nan-check --enum-range-check
endif
#CBMC_VERSION := $(shell cbmc --version | cut -c1-3)
#CBMC_ARGS += $(shell if test "`echo "$$CBMC_VERSION >= 5.13" | bc`" = 1; then \
#	echo "--pointer-primitive-check"; fi)
# cbmc 5.12.1: --pointer-primitive-check
# UNSATISFIABLE: passes, but needs more depth or builtins (nested loops => memset)
FAIL_VERIFY    = $(shell test -x ./algocfg && ./algocfg VFY_FAIL)
TIMEOUT_VERIFY = $(shell test -x ./algocfg && ./algocfg VFY_TIMEOUT)
NON_CBMC_SRC   = $(addsuffix .c, $(addprefix source/algos/,$(TIMEOUT_VERIFY)))
verify: verify/verify.log
verify/verify.log: $(ALGOSRC) $(ALGOSINC) algocfg GNUmakefile
	for c in $(ALGOSRC); do \
	  echo $$c; b=`basename $$c .c`; ./algocfg $$b cbmc; cmd=`./algocfg $$b cbmc`; \
	  echo $$cmd $(CBMC_ARGS) $$c; \
	  $$cmd $(CBMC_ARGS) $$c || \
            (echo $$cmd $(CBMC_ARGS) "$$c FAILED"; \
	    test $(( `./algocfg $$b VFY_FAIL` + `./algocfg $$b VFY_TIMEOUT` )) -gt 0 || exit 1); \
	done | tee verify/verify.log
	for b in `./algocfg UNSATISFIABLE` `./algocfg VFY_TIMEOUT`; do \
	  echo goto-analyzer -DCBMC --verify --recursive-interprocedural source/algos/$$b.c; \
	  goto-analyzer -DCBMC --verify --recursive-interprocedural source/algos/$$b.c; \
	done | tee -a verify/verify.log;
check-verify: algocfg GNUmakefile
	for c in $(addsuffix .c, $(addprefix source/algos/,$(filter-out $(TIMEOUT_VERIFY),$(TESTS)))); \
	do \
	  echo $$c; b=`basename $$c .c`; ./algocfg $$b cbmc; \
	  cmd=`./algocfg $$b cbmc`; \
	  echo $$cmd $(CBMC_ARGS) $$c; \
	  $$cmd $(CBMC_ARGS) $$c || \
            (echo $$cmd $(CBMC_ARGS) "$$c FAILED"; \
	     test $(( `./algocfg $$b VFY_FAIL` + `./algocfg $$b VFY_TIMEOUT` )) -gt 0 || exit 1); \
	done
# prints the violations
verify-trace: verify/trace.log
verify/trace.log: algocfg $(addsuffix .c, $(addprefix source/algos/,$(FAIL_VERIFY))) $(ALGOSINC) GNUmakefile
	for c in $(addsuffix .c, $(addprefix source/algos/,$(FAIL_VERIFY))); do \
	  echo $$c; b=`basename $$c .c`; ./algocfg $$b cbmc; \
	  cmd=`./algocfg $$b cbmc`; \
	  echo $$cmd --trace $(CBMC_ARGS) $$c; \
	  $$cmd --trace $(CBMC_ARGS) $$c || \
            (echo $$cmd --trace $(CBMC_ARGS) " $$c FAILED"; \
	     test $(( `./algocfg $$b VFY_FAIL` + `./algocfg $$b VFY_TIMEOUT` )) -gt 0 || exit 1); \
	done | tee verify/trace.log
fuzz: test-fuzz algocfg data/rand16/rand16.txt GNUmakefile
	-@test -d data/fuzz || \
	  (mkdir data/fuzz; \
	   for m in `seq 2 2 32`; do \
	     cut -c1-$$m <data/rand16/rand16.txt > data/fuzz/rand16-$$m.txt; \
	   done)
	echo '!#/bin/sh' >fuzz.sh
	-for c in $(ALGOSRC); do \
	  b="`basename $$c .c`"; echo $(MAKE) $$b.fuzz >>fuzz.sh; \
	done; chmod +x fuzz.sh; sh ./fuzz.sh
%.fuzz: algocfg source/algos/%.c $(ALGOSINC) GNUmakefile
	b=`basename $@ .fuzz`; \
	  $(MAKE) FUZZ=1 bin/fuzz/$$b; \
	  $(TIMEOUT_1m) afl-fuzz -i data/fuzz -o fuzz/$$b -- bin/fuzz/$$b; \
	  for c in fuzz/$$b/default/crashes/id\:*; do \
	    if [ -n "$c" ]; then xxd -i $c fuzz/$$b/id$(basename "$c" | cut -c4-9).h; fi; \
	  done; \
	  ps xw|grep 'bin/[f]uzz' |cut -c1-8|xargs kill -9
.PHONY: fuzz-repro
fuzz-repro:
	@for i in fuzz/*/default/crashes/id*; do \
	  default=$$(dirname $$(dirname "$$i")); algo=$$(basename $$(dirname "$$default")); data="$$default/data.t"; \
	  if [ "$$algo" = musl ]; then echo "SKIP $$i (strstr does not support NUL)"; continue; fi; \
	  if [ ! -s "$$data" ]; then echo "SKIP $$i (empty reproduction text)"; continue; fi; \
	  if ./fuzz-repro.sh "$$i"; then echo -e "\033[31;1;4mFAIL\033[0m not repro"; \
	    rm -f "$$i"; else echo -e "\033[32;1;4mREPRO\033[0m"; fi; done

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -Wextra -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax
fmt:
	clang-format -i `find source -name \*.c -o -name \*.h`
clean:
	-rm -f $(BINS) $(HELPERS) 2>/dev/null
TAGS: $(ALLSRC)
	-rm -f TAGS 2>/dev/null
	find . \( -name \*.c -o -name \*.h \) -exec etags -a --language=c \{\} \;
