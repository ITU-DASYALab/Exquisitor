//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQITEM_H
#define EXQUISITOR_EXQITEM_H

#pragma once

#include <array>

#include "ExqConstants.h"

namespace exq {

    using std::array;

    struct ExqItem {
        uint32_t itemId;
        int fromModality;
        array<double, N_MOD> distance;
        double aggScore;
        int workerId;
    }; //End of struct ExqItem

} //End of namespace exq

#endif //EXQUISITOR_EXQITEM_H