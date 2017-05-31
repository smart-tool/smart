#!/bin/bash
./logo
echo "	Compiling smart.c..................................[OK]"
gcc source/smart.c -O3 -msse2 -o smart -lm -std=gnu99
echo "	Compiling show.c...................................[OK]"
gcc source/show.c -O3 -msse2 -o show 
echo "	Compiling selectAlgo.c.............................[OK]"
gcc source/selectAlgo.c -O3 -o select
echo "	Compiling test.c...................................[OK]"
gcc source/test.c -O3 -o test
echo "	Generating random text buffers....................."
gcc source/textgen.c -o textgen -std=gnu99
./textgen
echo "	Compiling all string matching algorithms..........."
gcc source/compilesm.c -o compilesm -std=gnu99
./compilesm
echo " "
echo " "