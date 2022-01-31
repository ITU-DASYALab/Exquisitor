//
// Created by Omar Shahbaz Khan on 27/09/2021.
//

#ifndef EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H
#define EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H

#include "ECPConfig.h"
#include "ECPCluster.h"
#include "ECPNode.h"

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

    template<typename T, typename U, typename V>
    class ECPQueryOptimisationPolicies {
    public:
        ECPQueryOptimisationPolicies(ExpansionType expansionType, int statLevel, vector<ECPCluster<T,U,V>*>& clusters);

        ~ECPQueryOptimisationPolicies();

        void gatherInformation(int*& levelSizes, ECPConfig*& cnfg, vector<vector<ECPNode<T,U,V>*>>& nodes,
                               int numClusters);

        void addCombination(uint32_t clusterId, uint32_t descId);

        double getClusterCount(uint32_t clusterId);
        int getFilterRemainingCount(uint32_t clusterId);
        double getEstimatedRemainingCount(uint32_t clusterId);

        ExpansionType getExpType();
        int getStatLevel();

    private:
        ExpansionType _expansionType;
        int _statLevel = 1;
        vector<ECPCluster<T,U,V>*> _clusters;
        uint32_t* _originalCnt;
        uint32_t* _sessionRemainingCnt;
        uint32_t* _filterExactCnt;
        uint32_t* _filterReturnedCnt;
        uint32_t* _clusterToStatLevel;
        vector<map<std::string,double>> _combinations;
    };
}





#endif //EXQUISITOR_ECPQUERYOPTIMISATIONPOLICIES_H
