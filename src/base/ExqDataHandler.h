//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLER_H
#define EXQUISITOR_EXQDATAHANDLER_H

#include <vector>
#include <unordered_set>

#include "ExqDescriptor.h"
#include "ExqFunctions.h"
#include "ExqItem.h"
#include "Metadata/ItemFilter.h"

namespace exq {

    using std::vector;
    using std::unordered_set;

    template<class T>
    class ExqDataHandler {
    public:
        ExqDataHandler() {}

        virtual void loadData(int workers) = 0;

        /**
         * Returns descriptor for item i. Assumes modality is 0.
         * @param i - item id
         * @return Descriptor for item i.
         */
        virtual T* getDescriptor(uint32_t i) = 0;

        /**
         * Returns descriptor for item i from specific modality
         * @param i - item id
         * @param mod - modality
         * @return Descriptor for item i in modality mod
         */
        virtual T* getDescriptor(uint32_t i, int mod) = 0;

        virtual int getTotalItemsCount(int mod) = 0;

        virtual vector<bool> selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers,//vector<vector<double>>& model, vector<double>& bias,
                                            ItemFilter& filters, bool resume=false) = 0;

        virtual void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                           vector<vector<T>>& descriptors,
                                           unordered_set<uint32_t>& seenItems,
                                           ItemFilter& filters) = 0;

        virtual void updateSessionInfo(vector<uint32_t> suggs) = 0;
        virtual void resetSessionInfo() = 0;
    };

}

template class exq::ExqDataHandler<exq::ExqDescriptor<uint64_t,uint64_t,uint64_t>>;

#endif //EXQUISITOR_EXQDATAHANDLER_H
