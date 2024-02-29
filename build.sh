#!/bin/sh
./logo
CC="${CC:-gcc}"
MACHINE="$(uname -m)"
ARCH="$($CC -dumpmachine | cut -f1 -d-)"
if [ $ARCH = x86_64 ]
then
    CFLAGS="-march=native -mtune=native"
    SSE2="-msse2"
fi
if [ $MACHINE = x86_64 ]
then
    # we don't care that much about windows non-compat warnings yet
    uname_s="$(uname -s)"
    case "$uname_s" in
        MINGW*)  ;;
        CYGWIN*) ;;
        MSYS_NT*) ;;
        *) CFLAGS="$CFLAGS -Wall" ;;
    esac
fi
BINDIR=bin
DRV=
if [ "$ARCH" != "$MACHINE" ]; then
    echo cross complation to $MACHINE on $ARCH
    CROSS=1
    CFLAGS="-Wall"
    BINDIR=bin/$ARCH
    DRV="qemu-$ARCH "
fi
echo "	Compiling smart.c..................................[OK]"
$CC source/smart.c -O3 $CFLAGS $SSE2 -o smart -lm -std=gnu99 -DBINDIR="\"$BINDIR\""
echo "	Compiling show.c...................................[OK]"
$CC source/show.c -O3 $CFLAGS $SSE2 -o show
echo "	Compiling selectAlgo.c.............................[OK]"
$CC source/selectAlgo.c -O3 $CFLAGS -o select -DBINDIR="\"$BINDIR\""
echo "	Compiling test.c...................................[OK]"
$CC source/test.c -O3 $CFLAGS -o test -DBINDIR="\"$BINDIR\""
echo "	Generating random text buffers....................."
$CC source/textgen.c -o textgen -std=gnu99
$DRV./textgen
echo "	Compiling all string matching algorithms..........."
$CC source/compilesm.c -o compilesm -std=gnu99 -DBINDIR="\"$BINDIR\""
$DRV./compilesm
echo " "
echo " "
