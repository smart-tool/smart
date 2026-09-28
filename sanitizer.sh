#!/bin/sh
make SANITIZE=1
echo test all good
for t in `./algocfg good`
do
    ./test-asan "$t"
done
echo test all bad
for t in `./algocfg good 0`
do
    ./test-asan "$t"
done
echo now test the broken asan algos with some full smart test
# (sleep 10s; ./kill-tests-asan.sh &)
for t in `./algocfg asan 0`
do
    timeout 5s ./test-asan "$t" rand32
done
