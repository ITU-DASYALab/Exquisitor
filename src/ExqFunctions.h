//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQFUNCTIONS_H
#define EXQUISITOR_EXQFUNCTIONS_H

#pragma once

#include <string>
#include <vector>

#include "ExqR64Descriptor.h"
#include "ExqClassifier.h"
#include "ExqItem.h"

namespace exq {

    using std::string;
    using std::vector;

/**
 * @class ExqFunctions
 * @brief the class contains the scoring and ranking functions utilizing Ratio-i64 compression
 */
    class ExqFunctions {
    public:
        /**
         * @brief constructs the object using a config file containing info about the compression
         */
        ExqFunctions() = default;

        ~ExqFunctions();

        /**
         * @brief calculates distance between hyperplane and item
         */
        double distance(ExqClassifier& hyperplane, ExqR64Descriptor& item);

        /**
         * @brief rank aggregation using scores from each modality
         */
        void rankItems(vector<ExqItem>& items2Rank);

    private:
        int _topFeatureIdShift;
        int _featureIdsShift;
        int _featureRatiosShift;

    }; //End of class ExqFunctions

} //End of namespace exq

#endif //EXQUISITOR_EXQFUNCTIONS_H
