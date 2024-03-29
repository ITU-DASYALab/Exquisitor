//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include <fstream>
#include <iostream>

#include "ExqWorker.h"

using namespace exq;
using namespace std::chrono;
using std::make_pair;
using std::vector;
using std::array;
using std::milli;
using std::cout;
using std::endl;


template<class T>
ExqWorker<T>::ExqWorker() {

}

template<class T>
void ExqWorker<T>::suggest(int& k, vector<ExqItem>& itemsToReturn, vector<ExqClassifier*>& classifiers,
                           int currentSegment, int totalSegments, int noms, int modalities, IExqDataHandler<T>*& handler,
                           vector<IExqFunctions<T>*>& functions, unordered_set<uint32_t> seenItems, float& time,
                           int& totalItemsConsidered, int workerId, ItemFilter& filters, vector<float>& modWeights,
                           vector<int>& slots, bool ffs) {

    time_point<high_resolution_clock> beginOverall = high_resolution_clock::now();
    //time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    auto candidateItems = vector<vector<ExqItem>>(modalities);
    auto descriptors = vector<vector<IExqDescriptor<T>*>>(modalities);
    _workerId = workerId;
    totalItemsConsidered = 0;
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Getting segment " << currentSegment << " descriptors" << endl;
#endif
    handler->getSegmentDescriptors(currentSegment, totalSegments, modalities, descriptors,
                                   seenItems, filters);
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Got segment " << currentSegment << " descriptors" << endl;
#endif
    int* modSize = new int[modalities];
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Getting segment " << currentSegment << " noms" << endl;
#endif
    for (int m = 0; m < modalities; m++) {
        totalItemsConsidered += (int) descriptors[m].size();
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(ExqWorker[" << workerId << "]) Total items considered for segment " << currentSegment << " mod " <<
        m << ": " << totalItemsConsidered << endl;
#endif
        //(position,score)
        pair<int, float> min = make_pair(-1, FLT_MAX);
        modSize[m] = 0;
        candidateItems[m] = vector<ExqItem>();
        candidateItems[m].reserve(noms);
        for (int i = 0; i < (int) descriptors[m].size(); i++) {
            ExqItem candItem = ExqItem();
            candItem.fromModality.push_back(m); //push_back to check if item came from multiple modalities
            candItem.segment = currentSegment;
            candItem.itemId = descriptors[m][i]->getId();
            candItem.distance = vector<float>(modalities);
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
            cout << "(ExqWorker[" << workerId << "]) Getting distance for candidate item " << i << " descId ";
            cout << candItem.itemId << endl;
#endif
            for (int mm = 0; mm < modalities; mm++) {
                vector<float> model = classifiers[mm]->getWeights();
                float bias = classifiers[mm]->getBias();
                candItem.distance[mm] = functions[mm]->distance(model, bias,
                                                                *handler->getDescriptor(candItem.itemId, mm));
            }

            if ((int) candidateItems[m].size() == noms) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ExqWorker[" << workerId << "]) Checking if candidate is better than (" << min.first
                << "," << min.second << ")" << endl;
#endif
                if (candItem.distance[m] > min.second) {
                    candidateItems[m][min.first] = candItem;
                    float newMin = FLT_MAX;
                    for (int j = 0; j < (int) candidateItems[m].size(); j++) {
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
                    min.first = (int) candidateItems[m].size() - 1;
                    min.second = candItem.distance[m];
                }
                modSize[m]++;
            }
        }
    }
    finish = high_resolution_clock::now();

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ExqWorker[" << workerId << "]) Ranking segment " << currentSegment << endl;
#endif
    if (totalItemsConsidered != 0) {
        vector<ExqItem> sortedCandidates = vector<ExqItem>();
        vector<int> itemsPerMod = vector<int>(modalities, 0);
        for (int m = 0; m < modalities; m++) {
            sortedCandidates.insert(sortedCandidates.end(), candidateItems[m].begin(), candidateItems[m].end());
            itemsPerMod[m] = (int)candidateItems[m].size();
            candidateItems[m].clear();
        }
        candidateItems.clear();

        if (ffs) {
            // FFS (set number of slots per modality in output set)
            unordered_set<uint32_t> checked = unordered_set<uint32_t>();
            int start = 0;
            for (int m = 0; m < modalities; m++) {
                if (m != 0) start += itemsPerMod[m-1];
                std::sort(sortedCandidates.begin() + start, sortedCandidates.begin() + (start + itemsPerMod[m]),
                          [m](const ExqItem& lhs, const ExqItem& rhs) {
                    return lhs.distance[m] > rhs.distance[m];
                });
                int inserted = 0;
                for (int i = start; i < (start + itemsPerMod[m]); i++) {
                    if (checked.contains(sortedCandidates[i].itemId)) continue;
                    checked.insert(sortedCandidates[i].itemId);
                    itemsToReturn.push_back(sortedCandidates[i]);
                    inserted++;
                    if (inserted == slots[m]) break;
                }
            }
            itemsPerMod.clear();
        } else {
            // rank aggregation
            functions[0]->sortItems(sortedCandidates, modalities, modWeights);

            logCandidates(sortedCandidates);

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
            cout << "(ExqWorker[" << workerId << "]) Removing duplicates in segment " << currentSegment << endl;
#endif
            int cnt = 0;
            for (int i = 0; i < (int) sortedCandidates.size(); i++) {
                // check for duplicates if multiple modalities are active
                if (sortedCandidates[i].aggScore != -1.0 && modalities > 1) {
                    sortedCandidates[i].segment = currentSegment;

                    itemsToReturn.push_back(sortedCandidates[i]);
                    cnt++;

                    for (int j = i; j < (int) sortedCandidates.size(); j++) {
                        if (sortedCandidates[i].itemId == sortedCandidates[j].itemId) {
                            sortedCandidates[i].fromModality.push_back(sortedCandidates[j].fromModality[0]);
                            sortedCandidates[j].aggScore = -1.0;
                        }
                    }
                } else {
                    itemsToReturn.push_back(sortedCandidates[i]);
                    cnt++;
                }
                if (cnt == k) {
                    break;
                }
            }
        }
    }
    delete modSize;
    finish = high_resolution_clock::now();
    time = duration<float, milli>(finish - beginOverall).count();

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    if (itemsToReturn.size() > 0) {
        cout << "(ExqWorker[" << workerId << "]) Segment " << currentSegment << " Sanity check: " << itemsToReturn[0].itemId
        << " " << itemsToReturn[0].distance[0] << endl;
    }
    cout << "(ExqWorker[" << workerId << "]) Segment " << currentSegment << " finished" << endl;
#endif

}

template class exq::ExqWorker<uint64_t>;
