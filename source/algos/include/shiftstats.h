//
// Created by matt on 09/05/2022.
//

#include "../../searchinfo.h"

#ifndef SMART_SHIFTSTATS_H
#define SMART_SHIFTSTATS_H

struct algoValueNames getAlgoValueNames() {
    struct algoValueNames names = {0};
    setAlgoValueName(&names, 0, "shiftsum", "Sum of shifts in the table");
    return names;
}

#endif //SMART_SHIFTSTATS_H
