//
// Created by matt on 26/05/2022.
//
#include "../../searchinfo.h"

#ifndef SMARTC_BITSTATS2_H
#define SMARTC_BITSTATS2_H

struct algoValueNames getAlgoValueNames() {
    struct algoValueNames names = {0};
    setAlgoValueName(&names, 0, "bits1", "Count of bits set in index1");
    setAlgoValueName(&names, 1, "empty1", "Count of empty index1 entries");
    setAlgoValueName(&names, 2, "bits2", "Count of bits set in index2");
    setAlgoValueName(&names, 3, "empty2", "Count of empty index2 entries");
    return names;
}

#endif //SMARTC_BITSTATS2_H
