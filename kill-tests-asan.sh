#!/bin/sh
exe=`ps xw | perl -anle'$F[4] eq "./test-asan" && print $F[0]'`
while test -n "$exe" && test $exe -gt 0
do
    sleep 5s
    ps xw | perl -anle'$F[4]=~m{^\./test-asan} && $F[3]=~/^[1-9]:/ &&
print("kill $_") && kill("TERM",$F[0])'
    exe=`ps xw | perl -anle'$F[4] eq "./test-asan" && print $F[0]'`
    sleep 1s
done
