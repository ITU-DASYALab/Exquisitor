//
// Created by Omar Shahbaz Khan on 27/09/2021.
//

#ifndef EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H
#define EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H

#include "ECPConfig.h"
#include "ECPCluster.h"
#include "ECPNode.h"

#include "../Metadata/ItemProperties.h"

#include <vector>
#include <cmath>
#include <map>

namespace exq {
    // Expansion Type
    enum ExpansionType {
        ORIGINAL_CNT,
        GLOBAL_REMAINING_CNT,
        FILTER_REMAINING_CNT,
        ESTIMATED_REMAINING_CNT,
        ALL_REMAINING_CNT
    }; 

    using std::vector;
    using std::map;

    // struct MetaPropsProbs {
    //     vector<map<int,float>> props;
    // };
    /**
     * TODO: Determine if this data structure is the best approach for Estimated Count
     * collectionIdProb - probability of items being from a collection
     * videoIdProb - probability of items being from a video of a collection
     * stdPropProbs - probabilities of items standard properties
     * collPropProbs - probabilities of items collection properties
     * vidPropProbs - probabilties of items video properties
     */
    // struct MetaProbabilities {
    //     // collection -> probabiltiy
    //     map<int,float> collectionIdProb = map<int,float>();
    //     // collection -> video -> probability
    //     vector<map<int,float>> videoIdProb = vector<map<int,float>>();
    //     // property -> value(s) -> probability
    //     MetaPropsProbs stdPropProbs;
    //     MetaPropsProbs collPropProbs;
    //     // collection -> video -> property -> value(s) -> probability
    //     vector<map<int,MetaPropsProbs>> vidPropProbs = vector<map<int,MetaPropsProbs>>();
    // };

    /// This class contains the query optimisation policies used in the eCP index.
    /// There are 5 policies in total (see enum ExpansionType).
    class ECPQueryOptimisationPolicies {
    public:
        /// Constructor
        /// \param expansionType
        /// \param statLevel - Level of the eCP index to gather statistics from. Deeper levels require more storage
        /// and initial computation time.
        /// \param clusters - Vector of clusters
        /// \param itemProps - Item level properties
        /// \param vidProps - Video/Group level properties
        ECPQueryOptimisationPolicies(ExpansionType expansionType, int statLevel, vector<ECPCluster*>& clusters,
                                     vector<ItemProperties>* itemProps, vector<vector<Props>>* vidProps, int modality);

        ~ECPQueryOptimisationPolicies();

        /// Collects all the necessary information for the selected QOP
        /// \param levelSizes - Number of clusters for each level
        /// \param cnfg - ECPConfig object
        /// \param nodes - The index tree
        /// \param numClusters - Total number of clusters in index
        void gatherInformation(int*& levelSizes, ECPConfig*& cnfg, vector<vector<ECPNode*>>& nodes,
                               int numClusters);

        /// Add combination for statistics policy
        /// \param clusterId
        /// \param descId
        void addCombination(uint32_t clusterId, uint32_t descId);

        ///
        /// \param clusterId
        /// \return
        float getClusterCount(uint32_t clusterId);

        inline int getFilterRemainingCount(uint32_t clusterId) {
            if (_filterExactCnt[clusterId] == UINT32_MAX) return _sessionRemainingCnt[clusterId];
            if (_filterExactCnt[clusterId] == 0) return 0;
            int frc = _filterExactCnt[clusterId] - _filterReturnedCnt[clusterId];
            if (frc > 0) return frc;
            return 0;
        }


        inline void setFilterRemainingCount(uint32_t clusterId, int passed) {
            _filterExactCnt[clusterId] = passed;
            _filterReturnedCnt[clusterId] = 0;
        }


        float getEstimatedRemainingCount(uint32_t clusterId);

        inline ExpansionType getExpType() {
            return _expansionType;
        }


        inline int getStatLevel() {
            return _statLevel;
        }

        inline void updateSessionClusterCount(vector<uint32_t> suggs) {
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

        inline void resetSession() {
            for (int i = 0; i < _numClusters; i++) {
                _sessionRemainingCnt[i] = _originalCnt[i];
                if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
                    _filterExactCnt[i] = UINT32_MAX;
                    _filterReturnedCnt[i] = 0;
                }
            }
        }

        inline void resetFilterCount() {
            for (int i = 0; i < _numClusters; i++) {
                if (_expansionType == FILTER_REMAINING_CNT || _expansionType == ALL_REMAINING_CNT) {
                    _filterExactCnt[i] = UINT32_MAX;
                    _filterReturnedCnt[i] = 0;
                }
            }
        }


    private:
        ExpansionType _expansionType;
        int _statLevel = 1;
        int _modality;
        int _numClusters;
        std::string _descMapFname;
        vector<ECPCluster*> _clusters;
        uint32_t* _originalCnt;
        uint32_t* _sessionRemainingCnt;
        uint32_t* _filterExactCnt;
        uint32_t* _filterReturnedCnt;
        uint32_t* _clusterToStatLevel;
        // filter combinations for ERC
        vector<map<std::string,double>> _combinations;
        vector<uint32_t> _descToCluster = vector<uint32_t>();

        vector<ItemProperties>* _itemProps;
        vector<vector<Props>>* _vidProps;

        // Gets all 2^n-1 combinations from a vector of strings
        inline void _addCombinations(int clusterId, vector<string>& keys) {
            string emptyString;
            vector<string> allCombinations;
            int n = (int) std::pow(2.0, (double) keys.size());
            cout << "Number of keys: " << keys.size() << endl;
            allCombinations.reserve(n); // reserve 2^n
            allCombinations.push_back(emptyString); // add empty string
            for (auto it = allCombinations.begin(); it < allCombinations.end(); it++) {
                auto currSize = allCombinations.size();
                for (size_t j = 0; j < currSize; j++) {
                    allCombinations.push_back(allCombinations[j] + *it);
                }
            }
            // remove empty string
            allCombinations.erase(allCombinations.begin());
            cout << "Combinations being added for cluster " << clusterId << endl;
            for (auto cmb : allCombinations) {
                cout << cmb << endl;
                auto res = _combinations[clusterId].insert({cmb,1.0});
                if (!res.second) {
                    res.first->second = res.first->second + 1.0;
                }
            }
        }
    };
}





#endif //EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H
