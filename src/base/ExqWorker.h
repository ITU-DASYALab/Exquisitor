//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQWORKER_H
#define EXQUISITOR_EXQWORKER_H

#pragma once

#include <vector>
#include <string>

#include "ExqDescriptor.h"
#include "ExqDataHandler.h"
#include "ExqFunctions.h"
#include "ExqItem.h"

namespace exq {

    using std::vector;
    using std::string;

    template<typename T>
    class ExqWorker {
    public:
        ExqWorker();

        void suggest(int& k, vector<ExqItem>& itemsToReturn, vector<ExqClassifier*>& classifiers,
                     int currentSegment, int totalSegments, int noms, int modalities,
                     ExqDataHandler<T>*& handler, vector<ExqFunctions<T>*>& functions, unordered_set<uint32_t> seenItems,
                     double& times, int& totalItemsConsidered, int workerId, ItemFilter& filters);
    private:
        void logInfo(string info, int workerId);
    }; //End of class ExqWorker

} //End of namespace exq

#endif //EXQUISITOR_EXQWORKER_H
