#!/bin/sh
smart=`ps xw | perl -anle'$F[4] eq "./smart" && print $F[0]'`
while [ $smart -gt 0 ]
do
    sleep 2s
    ps xw | perl -anle'$F[4]=~m{^\./bin/} && $F[5] eq "shared" && $F[3]!~/^0:0[0-5]/ && kill("TERM",$F[0])'
    smart=`ps xw | perl -anle'$F[4] eq "./smart" && print $F[0]'`
done
