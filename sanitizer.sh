#!/bin/sh
make SANITIZE=1
# test all, not just the working algos
for t in `cat algos.lst`
do
    ./test-asan "$t"
done
# now test the broken asan algos with some full smart test
for t in `grep '^#' asan.lst | cut -c2-`
do
    ./test-asan "$t" rand2 2
done
