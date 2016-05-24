#!/bin/bash
./logo
echo "	Compiling smart.c..................................[OK]"
gcc source/smart.c -O3 -msse2 -o smart
echo "	Compiling show.c...................................[OK]"
gcc source/show.c -O3 -msse2 -o show
echo "	Compiling selectAlgo.c.............................[OK]"
gcc source/selectAlgo.c -O3 -o select
echo "	Compiling test.c...................................[OK]"
gcc source/test.c -O3 -o test
echo "	Compiling all string matching algorithms..........."
gcc source/compilesm.c -o compilesm
./compilesm
echo " "
echo " "