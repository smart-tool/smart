#!/bin/sh
./logo
CC="${CC:-gcc}"
uname_p="$(uname -p)"
if [ $uname_p = x86_64 ]
then
    CFLAGS="-march=native -mtune=native"
    # we don't care that much about windows non-compat warnings yet
    uname_s="$(uname -s)"
    case "$uname_s" in
        MINGW*)  ;;
        CYGWIN*) ;;
        MSYS_NT*) ;;
        *) CFLAGS="$CFLAGS -Wall" ;;
    esac
    SSE2="-msse2"
fi
echo "	Compiling smart.c..................................[OK]"
$CC source/smart.c -O3 $CFLAGS $SSE2 -o smart -lm -std=gnu99
echo "	Compiling show.c...................................[OK]"
$CC source/show.c -O3 $CFLAGS $SSE2 -o show
echo "	Compiling selectAlgo.c.............................[OK]"
$CC source/selectAlgo.c -O3 $CFLAGS -o select
echo "	Compiling test.c...................................[OK]"
$CC source/test.c -O3 $CFLAGS -o test
echo "	Generating random text buffers....................."
$CC source/textgen.c -o textgen -std=gnu99
./textgen
echo "	Compiling all string matching algorithms..........."
$CC source/compilesm.c -o compilesm -std=gnu99
./compilesm
echo " "
echo " "
