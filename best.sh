#!/bin/sh
set -x
#make -s clean
make -s algocfg
make -s
# perl -ane 'push @a,[$F[0],$F[4]] if $F[4]>0; END{@a = sort {$a->[1] <=> $b->[1]} @a; print lc($_->[0])," " for @a[0..20];}' all/englishTexts.txt

# english-16: ufndmq4 lwfr4 twfr4 twfrq4 simdkr skip4 bxs4 fsbndmq41 fsbndmq42 qf34 qf43 tso5 wfr4 wfrq4 sbndmq4 ufndmq6 bsdm4 bsdm5 bxs3 lwfr3 lwfr5
# english-64: qf62 epsm hc4 hc6 shc5 shc6 shc7 shc8 fhc4 skip5 qf72 wfr5 wfrq5 twfr5 twfr6 twfrq6 hc5 hc7 hc8 lhc4 lhc8
# english-256: qf43 epsm hc8 shc4 fhc7 qf62 wfrq4 wfrq5 twfr5 twfr6 twfrq5 twfrq6 hc5 hc6 lhc8 shc6 fhc3 fhc5 fhc6 fhc8 skip6
# rand64-16: libc simdkr sbndm2 fsbndm sbndmq2 bxs2 fsbndm-w4 fsbndmq20 qf26 shc2 bndmq2 bxs3 fsbndm-w6 fsbndm-w8 shc3 bsdm3 fs-w6 fsbndmq21 fsbndmq32 qf28 twfrq2
# rand64-64: qf34 tsa epsm lhc3 shc2 shc3 fhc3 sbndmq2 bxs2 fs-w8 fsbndm-w6 fsbndmq20 fsbndmq32 lwfr4 qf28 qf43 wfrq3 twfr3 twfr4 twfrq4 hc2
# rand64-256: qf34 hc4 hc7 lhc4 shc4 fhc2 fhc3 qf26 qf28 qf43 qf62 qf72 twfrq4 twfrq5 epsm hc3 hc6 lhc5 lhc8 shc2 shc3

# old:
# rand4-16: lwfr4 lwfr5 twfr4 twfr5 twfrq5 ufndmq6 qf42 qf43 twfrq4 bsdm4 tso5 wfr4 wfr5 wfrq4 wfrq5 skip4 sbndmq4 ufndmq4 bxs4 fsbndmq41 qf44
# rand16-16: bxs3 fsbndmq31 qf34 sbndmq2 bxs2 fsbndmq41 fsbndmq42 qf24 qf26 qf28 twfr4 ebom skip4 sbndm2 sbndmq4 ufndmq4 bsdm3 bsdm4 bxs4 fsbndmq20 lwfr3
# rand16-8: simdkr qf24 qf26 epsm ebom sbndmq2 bxs2 qf28 fsbndmq31 sbndm2 bsdm3 bxs3 fsbndmq20 qf34 sebom ufndmq2 qf33 bndmq2 ufndmq4 fsbndmq42 fsbndm
# rand64-256: faoso4 faoso6 bxs3 ft3 dbww dbww2 sbndm-w6 tsa sbndm-w4 tso5 musl1 skip4 qf26 qf28 qf34 qf43 qf44 qf53 wfr4 wfrq4 wfrq5
# french-8: simdkr musl1 epsm bsdm3 fsbndmq31 qf28 bxs3 qf26 qf34 ufndmq4 bxs2 ebom sbndmq2 fsbndmq42 libc1 sbndm2 fsbndmq20 fsbndmq41 qf33 twfr2 ufndmq2

./select -backup -none qf34 qf43 tsa epsm hc4 lhc3 lhc7 fhc3 sbndmq2 bxs2 fs-w8 fsbndm-w6 fsbndmq20 lwfr4 wfrq3 twfr3 twfrq4 simdkr krsimd musl1 bql skip6 tvsbs fsbndmq41
#was: tvsbs fjs hash8 so sbndm sbndm2 sbndm-bmh fndm sbndmq8 ufndmq2 dbww dbww2 fs-w2 lwfr5 qf43 qf62 sbndm-w2 tsa wfr2 twfrq2 twfrq4 wom libc1 musl1 simdkr epsm

./smart -text all -all -txt
./select -restore
