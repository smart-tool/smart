#!/bin/sh
make SANITIZE=1
# test all, not just the working algos
for t in `cat algos.lst`
do
    ./test-asan "$t"
done
