//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQITEM_H
#define EXQUISITOR_EXQITEM_H

#pragma once

#include <array>


namespace exq {

    using std::array;
    using std::vector;

    struct ExqItem {
        uint32_t itemId;
        int fromModality;
        uint64_t fromCluster;
        vector<double> distance;
        double aggScore;
        int segment;
        vector<double> modRank;

        ExqItem() {
            itemId = UINT32_MAX;
            fromModality = 0;
            aggScore = 0.0;
            fromCluster = UINT64_MAX;
            segment = -1;
            modRank = vector<double>();
        };
    }; //End of struct ExqItem

} //End of namespace exq

#endif //EXQUISITOR_EXQITEM_H
