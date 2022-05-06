//
// Created by matt on 05/05/2022.
//
#include "../../searchinfo.h"

#ifndef SMART_BITSTATS_H
#define SMART_BITSTATS_H

struct algoValueNames getAlgoValueNames() {
    struct algoValueNames names = {0};
    setAlgoValueName(&names, 0, "bitcount", "Count of bits set in index");
    setAlgoValueName(&names, 1, "empty", "Count of empty index entries");
    return names;
}

#endif //SMART_BITSTATS_H
