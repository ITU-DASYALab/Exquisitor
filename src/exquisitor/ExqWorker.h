//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQWORKER_H
#define EXQUISITOR_EXQWORKER_H

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "IExqDataHandler.h"
#include "IExqFunctions.h"
#include "ExqItem.h"

namespace exq {

    using std::vector;
    using std::string;
    using std::ofstream;
    using std::ifstream;
    using std::to_string;
    using std::cout;
    using std::endl;

    template<class T>
    class ExqWorker {
    public:
        ExqWorker();

        void suggest(int& k, vector<ExqItem>& itemsToReturn, vector<ExqClassifier*>& classifiers,
                     int currentSegment, int totalSegments, int noms, int modalities,
                     IExqDataHandler<T>*& handler, vector<IExqFunctions<T>*>& functions,
                     unordered_set<uint32_t> seenItems, float& times, int& totalItemsConsidered, 
                     int workerId, ItemFilter& filters, vector<float>& modWeights, vector<int>& slots, bool ffs);

    private:
        int _workerId = -1;

        inline void logInfo(string info) {
            string fname = "logs/workers/worker_" + to_string(_workerId) + ".log";
            ofstream log(fname, std::ios_base::out | std::ios_base::app);
            log << info << std::endl;
            log.close();
        };

        inline void logCandidates(vector<ExqItem>& candidates) {
            string fname = "logs/workers/worker_" + to_string(_workerId) + "_noms.log";
            ofstream log(fname, std::ios_base::out | std::ios_base::app);
            log << "[";
            for (size_t i = 0; i < candidates.size(); i++) {
                log << "(" << candidates[i].itemId << "," << candidates[i].distance[0] << ")";
                if ((i+1) != candidates.size()) log << ",";
            }
            log << "]" << endl;
            log.close();
        };

    }; //End of class ExqWorker

} //End of namespace exq

#endif //EXQUISITOR_EXQWORKER_H
