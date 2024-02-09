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
    };
}





#endif //EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H
