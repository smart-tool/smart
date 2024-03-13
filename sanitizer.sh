#!/bin/sh
make SANITIZE=1
for t in `cat algos.lst`
do
    ./test-asan "$t"
done
