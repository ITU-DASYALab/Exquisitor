//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include <fstream>
#include <iostream>

#include "ExqWorker.h"

using namespace exq;
using namespace std::chrono;
using std::make_pair;
using std::to_string;
using std::ofstream;
using std::ifstream;
using std::vector;
using std::array;
using std::milli;
using std::cout;
using std::endl;


template<typename T>
ExqWorker<T>::ExqWorker() {

}

template<typename T>
void ExqWorker<T>::suggest(int& k, vector<ExqItem>& itemsToReturn, vector<ExqClassifier*>& classifiers,
                           int currentSegment, int totalSegments, int noms, int modalities, ExqDataHandler<T>*& handler,
                           vector<ExqFunctions<T>*>& functions, unordered_set<uint32_t> seenItems, double& time,
                           int& totalItemsConsidered, int workerId, ItemFilter& filters) {

    time_point<high_resolution_clock> beginOverall = high_resolution_clock::now();
    //time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    vector<vector<ExqItem>> candidateItems = vector<vector<ExqItem>>(modalities);
    vector<vector<T>> descriptors = vector<vector<T>>(modalities);
    totalItemsConsidered = 0;
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Getting segment " << currentSegment << " descriptors" << endl;
#endif
    handler->getSegmentDescriptors(currentSegment, totalSegments, modalities, descriptors, seenItems);
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Got segment " << currentSegment << " descriptors" << endl;
#endif
    int modSize[modalities];
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Getting segment " << currentSegment << " noms" << endl;
#endif
    for (int m = 0; m < modalities; m++) {
        totalItemsConsidered += descriptors[m].size();
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(ExqWorker[" << workerId << "]) Total items considered for segment " << currentSegment << ": "
        << totalItemsConsidered << endl;
#endif
        //(position,score)
        pair<int,double> min = make_pair(-1, DBL_MAX);
        modSize[m] = 0;
        candidateItems[m] = vector<ExqItem>();
        candidateItems[m].reserve(noms);
        for (int i = 0; i < (int)descriptors[m].size(); i++) {
            ExqItem candItem = ExqItem();
            candItem.fromModality = m;
            candItem.segment = currentSegment;
            candItem.itemId = descriptors[m][i].id;
            candItem.distance = vector<double>(modalities);
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
            cout << "(ExqWorker[" << workerId << "]) Getting distance for candidate item " << i << endl;
#endif
            for (int mm = 0; mm < modalities; mm++) {
                vector<double> model = classifiers[mm]->getWeights();
                double bias = classifiers[mm]->getBias();
                candItem.distance[mm] = functions[m]->distance(model, bias, descriptors[mm][i]);
            }

            if ((int)candidateItems[m].size() == noms) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ExqWorker[" << workerId << "]) Checking if candidate is better than (" << min.first
                << "," << min.second << ")" << endl;
#endif
                if (candItem.distance[m] > min.second) {
                    candidateItems[m][min.first] = candItem;
                    double newMin = DBL_MAX;
                    for (int j = 0; j < (int)candidateItems[m].size(); j++) {
                        if (candidateItems[m][j].distance[m] < newMin) {
                            min.first = j;
                            min.second = candidateItems[m][j].distance[m];
                            newMin = min.second;
                        }
                    }
                }
            } else {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ExqWorker[" << workerId << "]) Adding candidate" << endl;
#endif
                candidateItems[m].push_back(candItem);
                if (candItem.distance[m] < min.second) {
                    min.first = (int)candidateItems[m].size()-1;
                    min.second = candItem.distance[m];
                }
                modSize[m]++;
            }
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
            cout << "(ExqWorker[" << workerId << "]) Checked candidate item " << i << endl;
#endif
        }
    }
    finish = high_resolution_clock::now();

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Ranking segment " << currentSegment << endl;
#endif
    vector<ExqItem> sortedCandidates = vector<ExqItem>();
    for (int m = 0; m < modalities; m++) {
        sortedCandidates.insert(sortedCandidates.end(), candidateItems[m].begin(), candidateItems[m].end());
        candidateItems[m].clear();
    }
    functions[0]->sortItems(sortedCandidates, modalities);

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Removing duplicates in segment " << currentSegment << endl;
#endif
    int cnt = 0;
    for (int i = 0; i < (int)sortedCandidates.size(); i++) {
        if (sortedCandidates[i].aggScore != -1) {
            sortedCandidates[i].segment = currentSegment;

            itemsToReturn.push_back(sortedCandidates[i]);
            cnt++;

            for (int j = i; j < (int)candidateItems.size(); j++) {
                if (sortedCandidates[i].itemId == sortedCandidates[j].itemId) {
                    sortedCandidates[j].aggScore = -1.0;
                }
            }
        }
        if (cnt == k) {
            break;
        }
    }
    finish = high_resolution_clock::now();
    time = duration<double, milli>(finish - beginOverall).count();

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Segment " << currentSegment << " Sanity check: " << itemsToReturn[0].itemId
    << " " << itemsToReturn[0].distance[0] << endl;
    cout << "(ExqWorker[" << workerId << "]) Segment " << currentSegment << " finished" << endl;
#endif
}

template<typename T>
void ExqWorker<T>::logInfo(string info, int workerId) {
    string fname = "logs/workers/worker_" + to_string(workerId) + ".log";
    ofstream log(fname, std::ios_base::out | std::ios_base::app);
    log << info << "\n";
    log.close();
}

template class exq::ExqWorker<exq::ExqDescriptor<uint64_t,uint64_t,uint64_t>>;

