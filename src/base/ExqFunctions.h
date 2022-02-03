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

        virtual int getDescFeatCount() = 0;
        virtual int getDescriptorSize() = 0;
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
         * @brief sort items using scores from each modality
         */
        virtual void sortItems(vector<ExqItem>& items2Rank, int modalities) = 0;

        /**
         * @brief assign aggregate score for multimodal items
         */
        virtual void assignRanking(vector<ExqItem>& items, int mod) = 0;

    }; //End of class ExqFunctions

} //End of namespace exq

template class exq::ExqFunctions<exq::ExqDescriptor<uint64_t,uint64_t,uint64_t>>;

#endif //EXQUISITOR_EXQFUNCTIONS_H
