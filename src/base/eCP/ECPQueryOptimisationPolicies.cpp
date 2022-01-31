//
// Created by Omar Shahbaz Khan on 27/09/2021.
//

#include "ECPQueryOptimisationPolicies.h"

using namespace exq;

template <typename T, typename U, typename V>
ECPQueryOptimisationPolicies<T,U,V>::ECPQueryOptimisationPolicies(ExpansionType expansionType, int statLevel,
                                                                  vector<ECPCluster<T,U,V>*>& clusters) {
    _expansionType = expansionType;
    _statLevel = statLevel;
    _clusters = clusters;
}

template <typename T, typename U, typename V>
ECPQueryOptimisationPolicies<T,U,V>::~ECPQueryOptimisationPolicies() {

}

template <typename T, typename U, typename V>
void ECPQueryOptimisationPolicies<T,U,V>::gatherInformation(int*& levelSizes, ECPConfig*& cnfg,
                                                            vector<vector<ECPNode<T,U,V>*>>& nodes, int numClusters) {
    _originalCnt = new uint32_t[numClusters];
    _sessionRemainingCnt = new uint32_t[numClusters];
    if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
        _filterExactCnt = new uint32_t[numClusters];
        _filterReturnedCnt = new uint32_t[numClusters];
    }
    for (int i = 0; i < numClusters; i++) {
        _originalCnt[i] = _clusters[i]->getNumDescriptors();
        _sessionRemainingCnt[i] = _clusters[i]->getNumDescriptors();
        if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
            _filterExactCnt[i] = UINT32_MAX;
            _filterReturnedCnt = 0;
        }
    }
    if (_expansionType == ESTIMATED_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
        _combinations = vector<map<string,double>> (levelSizes[_statLevel]);
        for (int i = 0; i < levelSizes[_statLevel]; i++) {
            _combinations[i] = map<string,double>();
        }
        _clusterToStatLevel = new uint32_t[numClusters];
        double* clusterTotals = new double[levelSizes[_statLevel]]();
        cout << "Gathering statistics..." << endl;
        for (int i = 0; i < numClusters; i++) {
            int temp_level = cnfg->getNumLvls()-1;
            int p = i;
            while (temp_level != _statLevel) {
                p = nodes[temp_level][p]->parent;
                temp_level--;
            }
            _clusterToStatLevel[i] = p; //Map from selected cluster to stat level cluster
            //cout << i << "\t" << clusterToStats[i] << endl;
            // Add each items filter statistics to the rightful stat level cluster
            _clusters[i]->open();
            ExqDescriptor<T,U,V>* desc;
            //cout << "Adding combinations for " << i << endl;
            while((desc = _clusters[i]->next()) != NULL) {
                //addCombination(p, desc->id);
                clusterTotals[p]++;
            }
            _clusters[i]->close();
        }

        cout << "Calculating probabilities..." << endl;
        int totalCombinationSize = 0;
        for (int i = 0; i < levelSizes[_statLevel]; i++) {
            for (auto iter = _combinations[i].begin(); iter != _combinations[i].end(); iter++) {
                // Probability based on total number of actual items under parent
                //cout << i << "\t" << iter->first << "\t" << iter->second << "\t" << clusterTotals[i]  << "\t" << iter->second/clusterTotals[i] << endl;
                iter->second = iter->second/clusterTotals[i];
            }
            totalCombinationSize += _combinations[i].size();
        }
        cout << "Stored probabilities" << endl;
        cout << "Total combinations: " << totalCombinationSize << endl;
    }
}

template <typename T, typename U, typename V>
void ECPQueryOptimisationPolicies<T,U,V>::addCombination(uint32_t clusterId, uint32_t descId) {
    //std::string collFilter = "F,";
    //if (globalItemIdFilter > 0 && globalItemIdFilter >= descId) {
    //    collFilter = "T,";
    //}
    //std::string key = collFilter + to_string(metaMap[descId].location) + "," + to_string(metaMap[descId].hour) + "," + to_string(metaMap[descId].day);
    //cout << "clusterId: " << clusterId << ", descId: " << descId << ", Key: " << key << endl;
    //auto res = combinations[clusterId].insert({key,1.0});
    //cout << "Insert res: " << res.first->first << "," << res.first->second << "," << res.second << endl;
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + to_string(metaMap[descId].location) + ",-1,-1";
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + "-1," + to_string(metaMap[descId].hour) + ",-1";
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + "-1,-1," + to_string(metaMap[descId].day);
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + to_string(metaMap[descId].location) + "," + to_string(metaMap[descId].hour) + ",-1";
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + to_string(metaMap[descId].location) + ",-1," + to_string(metaMap[descId].day);
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + "-1," + to_string(metaMap[descId].hour) + "," + to_string(metaMap[descId].day);
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
    //key = collFilter + "-1,-1,-1";
    //res = combinations[clusterId].insert({key,1.0});
    //if (!res.second) {
    //    res.first->second = res.first->second + 1.0;
    //}
}

template <typename T, typename U, typename V>
double ECPQueryOptimisationPolicies<T,U,V>::getClusterCount(uint32_t clusterId) {
    if (_expansionType == GLOBAL_REMAINING_CNT) return _sessionRemainingCnt[clusterId];
    if (_expansionType == FILTER_REMAINING_CNT) return getFilterRemainingCount(clusterId);
    if (_expansionType == ESTIMATED_REMAINING_CNT) return getEstimatedRemainingCount(clusterId);
    if (_expansionType == ALL_REMAINING_CNT) {
        if (_sessionRemainingCnt == 0) return 0.0;
    }
    return _originalCnt[clusterId];
}

template <typename T, typename U, typename V>
inline int ECPQueryOptimisationPolicies<T,U,V>::getFilterRemainingCount(uint32_t clusterId) {
    if (_filterExactCnt[clusterId] == UINT32_MAX) return _sessionRemainingCnt[clusterId];
    return _filterExactCnt[clusterId] - _filterReturnedCnt[clusterId];
}

template <typename T, typename U, typename V>
double ECPQueryOptimisationPolicies<T,U,V>::getEstimatedRemainingCount(uint32_t clusterId) {

}

template <typename T, typename U, typename V>
inline ExpansionType ECPQueryOptimisationPolicies<T,U,V>::getExpType() {
    return _expansionType;
}

template <typename T, typename U, typename V>
inline int ECPQueryOptimisationPolicies<T,U,V>::getStatLevel() {
    return _statLevel;
}

template class exq::ECPQueryOptimisationPolicies<uint64_t, uint64_t, uint64_t>;
