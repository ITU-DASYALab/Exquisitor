//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQFUNCTIONS_H
#define EXQUISITOR_EXQFUNCTIONS_H

#pragma once

#include <string>
#include <vector>
#include <utility>

#include "ExqDescriptor.h"
#include "ExqClassifier.h"
#include "ExqItem.h"
#include "ExqArray.h"

namespace exq {

    using std::string;
    using std::vector;
    using std::pair;

    /**
     * @class ExqFunctions
     * @brief the class contains the scoring and ranking functions
     */
    template <class T>
    class ExqFunctions {
    public:
        ~ExqFunctions() {};

        /**
         * @brief convert compressed descriptor representation into actual feature id and value pairs
         * @return
         */
        virtual ExqArray<pair<int,float>> getDescriptorInformation(T& descriptor) = 0;

        /**
         * @brief calculates distance between hyperplane and item
         */
        virtual double distance(vector<double>& model, double bias, T& descriptor) = 0;

        /**
         * @brief rank aggregation using scores from each modality
         */
        virtual void rankItems(vector<ExqItem>& items2Rank) = 0;

    protected:
        int nFeatures;
    }; //End of class ExqFunctions

} //End of namespace exq

#endif //EXQUISITOR_EXQFUNCTIONS_H
