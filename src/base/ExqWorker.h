//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQWORKER_H
#define EXQUISITOR_EXQWORKER_H

#pragma once

#include <vector>
#include <string>

#include "r64/ExqDescriptorR64.h"
#include "IExqDataHandler.h"
#include "IExqFunctions.h"
#include "ExqItem.h"

namespace exq {

    using std::vector;
    using std::string;

    template<class T>
    class ExqWorker {
    public:
        ExqWorker();

        void suggest(int& k, vector<ExqItem>& itemsToReturn, vector<ExqClassifier*>& classifiers,
                     int currentSegment, int totalSegments, int noms, int modalities,
                     IExqDataHandler<T>*& handler, vector<IExqFunctions<T>*>& functions,
                     unordered_set<uint32_t> seenItems, double& times, int& totalItemsConsidered, 
                     int workerId, ItemFilter& filters, vector<double>& modWeights, vector<int>& slots, bool ffs);

    private:
        void logInfo(string info, int workerId);
    }; //End of class ExqWorker

} //End of namespace exq

#endif //EXQUISITOR_EXQWORKER_H
