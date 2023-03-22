//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_IEXQDATAHANDLER_H
#define EXQUISITOR_IEXQDATAHANDLER_H

#include <vector>
#include <unordered_set>

#include "IExqDescriptor.h"
#include "IExqFunctions.h"
#include "ExqItem.h"
#include "Metadata/ItemFilter.h"

namespace exq {

    using std::vector;
    using std::unordered_set;

    template<class T>
    class IExqDataHandler {
    public:
        IExqDataHandler() {}

        virtual void loadData(int workers) = 0;

        /**
         * Returns descriptor for item i. Assumes modality is 0.
         * @param i - item id
         * @return Descriptor for item i.
         */
        virtual IExqDescriptor<T>* getDescriptor(uint32_t i) = 0;

        /**
         * Returns descriptor for item i from specific modality
         * @param i - item id
         * @param mod - modality
         * @return Descriptor for item i in modality mod
         */
        virtual IExqDescriptor<T>* getDescriptor(uint32_t i, int mod) = 0;

        virtual int getTotalItemsCount(int mod) = 0;

        virtual vector<bool> selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers,//vector<vector<double>>& model, vector<double>& bias,
                                            ItemFilter& filters, bool resume=false) = 0;

        virtual void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                           vector<vector<IExqDescriptor<T>*>>& descriptors,
                                           unordered_set<uint32_t>& seenItems,
                                           ItemFilter& filters) = 0;

        virtual void updateSessionInfo(vector<uint32_t> suggs) = 0;
        virtual void resetSessionInfo() = 0;
    };

}

template class exq::IExqDataHandler<uint64_t>;

#endif //EXQUISITOR_IEXQDATAHANDLER_H
