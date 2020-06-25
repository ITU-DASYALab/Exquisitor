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
        vector<double> distance;
        double aggScore;
        int segment;

        ExqItem() {
            itemId = UINT32_MAX;
            int fromModality = VIS;
            double aggScore = 0.0;
            segment = -1;
        };
    }; //End of struct ExqItem

} //End of namespace exq

#endif //EXQUISITOR_EXQITEM_H
