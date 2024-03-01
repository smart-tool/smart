#!/bin/sh
make clean
set -e
make CFLAGS="-g -O3 -march=native -mtune=native -Wall -Wextra -fsanitize=address,undefined"
for t in `cat algos.lst`
do
    ./test "$t"
done
