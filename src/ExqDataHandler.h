//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLER_H
#define EXQUISITOR_EXQDATAHANDLER_H

#include <vector>
#include <unordered_set>

#include "ExqDescriptor.h"
#include "ExqItem.h"

namespace exq {

    using std::vector;
    using std::unordered_set;

    template<class T>
    class ExqDataHandler {
    public:
        ExqDataHandler() {}

        virtual void loadData(int modalities, int workers) = 0;

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

        virtual void selectClusters(int b, vector<double>& model, double bias,
                                    double(*dist)(vector<double>&model, double, T&)) = 0;

        virtual void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                           vector<vector<T>>& descriptors, unordered_set<uint32_t>& seenItems) = 0;
    };

}

#endif //EXQUISITOR_EXQDATAHANDLER_H
