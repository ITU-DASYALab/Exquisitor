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
        vector<int> fromModality;
        vector<uint32_t> fromCluster;
        vector<double> distance;
        double aggScore;
        int segment;
        vector<double> modRank; // The rank for each modality an item got during aggregation, example: aggScore = 4, mod1Rank = 1, mod2Rank = 3 

        ExqItem() {
            itemId = UINT32_MAX;
            fromModality = vector<int>();
            aggScore = 0.0;
            fromCluster = vector<uint32_t>();
            segment = -1;
            modRank = vector<double>();
        };
    }; //End of struct ExqItem

} //End of namespace exq

#endif //EXQUISITOR_EXQITEM_H
