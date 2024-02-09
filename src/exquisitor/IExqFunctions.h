//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQFUNCTIONSINTERFACE_H
#define EXQUISITOR_EXQFUNCTIONSINTERFACE_H

#pragma once

#include <string>
#include <vector>
#include <utility>

#include "IExqDescriptor.h"
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
    class IExqFunctions {
    public:
        ~IExqFunctions() {};

        virtual int getDescFeatCount() = 0;
        virtual int getDescriptorSize() = 0;
        virtual int getIota() = 0; // Only applicable if the R64 compression is used.

        /**
         * @brief convert compressed descriptor representation into actual feature id and value pairs
         * @return
         */
        virtual ExqArray<pair<int,float>> getDescriptorInformation(IExqDescriptor<T>& descriptor) = 0;

        /**
         * @brief calculates distance between hyperplane and item
         */
        virtual float distance(vector<float>& model, float bias, IExqDescriptor<T>& descriptor) = 0;

        /**
         * @brief sort items using scores from each modality
         */
        virtual void sortItems(vector<ExqItem>& items2Rank, int modalities, vector<float>& modWeights,
                               bool setModRank=false, bool singleMod=false) = 0;

        /**
         * @brief assign aggregate score for multimodal items
         */
        virtual void assignRanking(vector<ExqItem>& items, int nMods, vector<float>& modWeights,
                                   bool setModRank=false) = 0;


    protected:

    }; //End of class ExqFunctions

} //End of namespace exq

template class exq::IExqFunctions<uint64_t>;

#endif //EXQUISITOR_EXQFUNCTIONSINTERFACE_H
