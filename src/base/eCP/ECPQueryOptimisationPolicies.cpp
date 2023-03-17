//
// Created by Omar Shahbaz Khan on 27/09/2021.
//

#include "ECPQueryOptimisationPolicies.h"
#include <filesystem>

using namespace exq;

ECPQueryOptimisationPolicies::ECPQueryOptimisationPolicies(ExpansionType expansionType, int statLevel,
                                                           vector<ECPCluster*>& clusters,
                                                           vector<ItemProperties>* itemProps,
                                                           vector<vector<Props>>* vidProps,
                                                           int modality) {
    _expansionType = expansionType;
    _statLevel = statLevel;
    _clusters = clusters;
    _itemProps = itemProps;
    _vidProps = vidProps;
    _modality = modality;
    _descMapFname = "exqData/descIdToClusterId_mod" + std::to_string(_modality) + ".dat";
    _originalCnt = nullptr;
    _sessionRemainingCnt = nullptr;
    _filterExactCnt = nullptr;
    _filterReturnedCnt = nullptr;
    _clusterToStatLevel = nullptr;
}


ECPQueryOptimisationPolicies::~ECPQueryOptimisationPolicies() {
    delete _originalCnt;
    delete _sessionRemainingCnt;
    delete _filterExactCnt;
    delete _filterReturnedCnt;
    delete _clusterToStatLevel;
}


void ECPQueryOptimisationPolicies::gatherInformation(int*& levelSizes, ECPConfig*& cnfg,
                                                     vector<vector<ECPNode*>>& nodes, int numClusters) {
    _numClusters = numClusters;
    cout << "Gathering information for QOP" << endl;
    _originalCnt = new uint32_t[_numClusters];
    _sessionRemainingCnt = new uint32_t[_numClusters];
    if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
        _filterExactCnt = new uint32_t[_numClusters];
        _filterReturnedCnt = new uint32_t[_numClusters];
    }
    std::map<uint32_t,uint32_t> descToCluster = std::map<uint32_t,uint32_t>();
    for (int i = 0; i < _numClusters; i++) {
        _originalCnt[i] = _clusters[i]->getNumDescriptors();
        _sessionRemainingCnt[i] = _clusters[i]->getNumDescriptors();
        if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
            _filterExactCnt[i] = UINT32_MAX;
            _filterReturnedCnt[i] = 0;
        }
        _clusters[i]->open();
        ExqDescriptorR64 *desc;
        while ((desc = _clusters[i]->next()) != NULL) {
            descToCluster[desc->getId()] = i;
        }
    }
    for (int i = 0; i < (int) descToCluster.size(); i++) {
        _descToCluster.push_back(descToCluster[i]);
    }

#ifdef STORE_QOP
    // check if data directory exists
    if (!std::filesystem::exists("exqData")) {
        std::filesystem::create_directory("exqData");
    }
    // check if file for descriptor id to cluster id exists, if not create and fill it
    if (!std::filesystem::exists(_descMapFname)) {
        cout << "Storing descriptorId to clusterId file (line number = descId)" << endl;
        std::ofstream f;
        f.open(_descMapFname);
        for (int i = 0; i < (int) descToCluster.size(); i++) {
            _descToCluster.push_back(descToCluster[i]);
            f << std::to_string(descToCluster[i]) << "\n";
        }
        f.close();
    }
    /** 
     * TODO: If the files exists load from them, which means moving the exist check up and come up with a way for
     * deletion/update as shifting modalities/collection/filters require this
     */
#endif

    if (_expansionType == ESTIMATED_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
        _combinations = vector<map<string,double>> (levelSizes[_statLevel]);
        for (int i = 0; i < levelSizes[_statLevel]; i++) {
            _combinations[i] = map<string,double>();
        }
        _clusterToStatLevel = new uint32_t[numClusters];
        auto* clusterTotals = new double[levelSizes[_statLevel]]();
        cout << "Gathering statistics..." << endl;
        for (int i = 0; i < numClusters; i++) {
            int temp_level = (int) cnfg->getNumLvls()-1;
            int p = i;
            while (temp_level != _statLevel) {
                p = nodes[temp_level][p]->parent;
                temp_level--;
            }
            _clusterToStatLevel[i] = p; //Map from selected cluster to stat level cluster
            //cout << i << "\t" << clusterToStats[i] << endl;
            // Add each items filter statistics to the rightful stat level cluster
            _clusters[i]->open();
            ExqDescriptorR64* desc;
            //cout << "Adding combinations for " << i << endl;
            while((desc = _clusters[i]->next()) != NULL) {
                addCombination(p, desc->getId());
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
        cout << "Done" << endl;
        cout << "Total combinations: " << totalCombinationSize << endl;
    }
}


void ECPQueryOptimisationPolicies::addCombination(uint32_t clusterId, uint32_t descId) {
    /**
     * Old method from research version of Exquisitor will not work for this version of Exquisitor
     * TODO: Overhaul it towards using optimal C++ functionality and the current filter management structure
     */

    //struct MetaProbabilities {
    //    // collection -> probabiltiy
    //    map<int,float> collectionIdProb;
    //    // collection -> video -> probability
    //    vector<map<int,float>> videoIdProb;
    //    // property -> value(s) -> probability
    //    MetaPropsProbs stdPropProbs;
    //    MetaPropsProbs collPropProbs;
    //    // collection -> video -> property -> value(s) -> probability
    //    vector<map<int,MetaPropsProbs>> vidPropProbs;
    //};

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


double ECPQueryOptimisationPolicies::getClusterCount(uint32_t clusterId) {
    if (_expansionType == GLOBAL_REMAINING_CNT) return _sessionRemainingCnt[clusterId];
    if (_expansionType == FILTER_REMAINING_CNT) return getFilterRemainingCount(clusterId);
    if (_expansionType == ESTIMATED_REMAINING_CNT) return getEstimatedRemainingCount(clusterId);
    if (_expansionType == ALL_REMAINING_CNT) {
        double est = getEstimatedRemainingCount(clusterId);
        if (_sessionRemainingCnt[clusterId] == 0) return 0.0;
        if (_filterExactCnt[clusterId] == UINT32_MAX) {
            // FRC not cached
            if (_sessionRemainingCnt[clusterId] < est) return _sessionRemainingCnt[clusterId];
            return est;
        }
        // FRC cached
        return getFilterRemainingCount(clusterId);
    }
    return _originalCnt[clusterId];
}


inline int ECPQueryOptimisationPolicies::getFilterRemainingCount(uint32_t clusterId) {
    if (_filterExactCnt[clusterId] == UINT32_MAX) return _sessionRemainingCnt[clusterId];
    if (_filterExactCnt[clusterId] == 0) return 0;
    int frc = _filterExactCnt[clusterId] - _filterReturnedCnt[clusterId];
    if (frc > 0) return frc;
    return 0.0;
}


inline void ECPQueryOptimisationPolicies::setFilterRemainingCount(uint32_t clusterId, int passed) {
    _filterExactCnt[clusterId] = passed;
    _filterReturnedCnt[clusterId] = 0;
}


double ECPQueryOptimisationPolicies::getEstimatedRemainingCount(uint32_t clusterId) {
    /**
     * Old method from research version of Exquisitor will not work for this version of Exquisitor
     * TODO: Overhaul it towards using optimal C++ functionality and the current filter management structure
     */
//    double fcount = 0.0;
//    if (expandType == 3 || expandType == 7 || expandType == 4 || expandType == 8) {
//        fcount = 0;
//        if (filterInfo->f == 0 && globalItemIdFilter == 0) {
//            //cout << "Count for cluster " << id << " is " << clusterCount[id] << endl;
//            fcount = (double)sessionClusterCount[id];
//        } else {
//            if (sessionClusterCount[id] == 0) return 0.0;
//            map<string,double>::iterator it;
//            for (uint32_t j = 0; j < filterInfo->keys->size(); j++) {
//                it = combinations[clusterToStats[id]].find(filterInfo->keys->at(j));
//                if (it != combinations[clusterToStats[id]].end()) {
//                    //cout << "Cluster " << id << " has " << clusterCount[id] << " items. For key " << filterInfo->keys->at(j) << " the probability cluster " << clusterToStats[id] << " has " << combinations[clusterToStats[id]].find(filterInfo->keys->at(j))->second << endl;
//                    fcount += (double)sessionClusterCount[id] * it->second;
//                }
//                //cout << "fcount = " << fcount << endl;
//            }
//            //cout << "Total fcount = " << fcount << endl;
//        }
//        if (check) {
//            return fcount;
//        }
//    }

    return 0.0;
}


inline ExpansionType ECPQueryOptimisationPolicies::getExpType() {
    return _expansionType;
}


inline int ECPQueryOptimisationPolicies::getStatLevel() {
    return _statLevel;
}


inline void ECPQueryOptimisationPolicies::updateSessionClusterCount(vector<uint32_t> suggs) {
    for (auto s : suggs) {
        if (_sessionRemainingCnt[_descToCluster[s]] > 0) {
            // update session count
            _sessionRemainingCnt[_descToCluster[s]]--;
            // update filter returned count
            if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
                _filterReturnedCnt[_descToCluster[s]]++;
            }
        }
    }
}


inline void ECPQueryOptimisationPolicies::resetSession() {
    for (int i = 0; i < _numClusters; i++) {
        _sessionRemainingCnt[i] = _originalCnt[i];
        if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
            _filterExactCnt[i] = UINT32_MAX;
            _filterReturnedCnt[i] = 0;
        }
    }
}


inline void ECPQueryOptimisationPolicies::resetFilterCount() {
    for (int i = 0; i < _numClusters; i++) {
        if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
            _filterExactCnt[i] = UINT32_MAX;
            _filterReturnedCnt[i] = 0;
        }
    }
}