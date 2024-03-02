#!/bin/sh
make SANITIZE=1
make -B SANITIZE=1 test
for t in `cat algos.lst`
do
    ./test "$t"
done
