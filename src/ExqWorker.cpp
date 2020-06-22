//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqWorker.h"

using namespace exq;
using std::make_pair;
using std::vector;
using std::array;

template<typename T>
void ExqWorker::suggest(int& k, vector<ExqItem>& itemsToReturn, vector<double>& model, double bias, int currentSegment,
                        int totalSegments, int noms, int modalities, ExqDataHandler<T>& handler,
                        ExqFunctions<T>& functions) {
    vector<ExqItem> candidateItems = vector<ExqItem>();
    vector<vector<T>> descriptors = vector<vector<T>>();
    handler.getSegmentDescriptors(currentSegment, totalSegments, modalities, descriptors);
    int modSize[modalities];
    //void (*descInfo)(T&) = functions.getDescriptorInformation;
    //void (*dist)(vector<double>&, double, T&) = functions.distance;
    for (int m = 0; m < modalities; m++) {
        pair<int,double> min = make_pair(-1, -DBL_MAX);
        modSize[m] = 0;
        for (int i = 0; i < descriptors[m].size(); i++) {
            ExqItem candItem = ExqItem();
            candItem.fromModality = m;
            candItem.segment = currentSegment;
            candItem.itemId = descriptors[m][i].id;
            candItem.distance.resize(modalities);
            candItem.distance[m] = functions.distance(model, bias, descriptors[m][i]);
            for (int mm = 0; mm < modalities; mm++) {
                if (mm == m) continue;
                candItem.distance[mm] = functions.distance(model, bias, descriptors[mm][i]);
            }

            if (candidateItems.size() == noms * (modalities+1)) {
                if (candItem.distance[m] > min.second) {
                    candidateItems[min.first] = candItem;
                    min.first = candItem.itemId;
                    min.second = candItem.distance[m];
                }
            } else {
                candidateItems.push_back(candItem);
                if (candItem.distance[m] > min.second) {
                    min.first = candItem.itemId;
                    min.second = candItem.distance[m];
                }
                modSize[m]++;
            }
        }
    }

    //TODO: Utilize Functions rankItems function to fuse modalities and get k candidates
}
