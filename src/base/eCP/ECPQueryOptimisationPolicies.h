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
    template<typename T, typename U, typename V>
    class ECPQueryOptimisationPolicies {
    public:
        /// Constructor
        /// \param expansionType
        /// \param statLevel - Level of the eCP index to gather statistics from. Deeper levels require more storage
        /// and initial computation time.
        /// \param clusters - Vector of clusters
        /// \param itemProps - Item level properties
        /// \param vidProps - Video/Group level properties
        ECPQueryOptimisationPolicies(ExpansionType expansionType, int statLevel, vector<ECPCluster<T,U,V>*>& clusters,
                                     vector<ItemProperties>* itemProps, vector<vector<Props>>* vidProps, int modality);

        ~ECPQueryOptimisationPolicies();

        /// Collects all the necessary information for the selected QOP
        /// \param levelSizes - Number of clusters for each level
        /// \param cnfg - ECPConfig object
        /// \param nodes - The index tree
        /// \param numClusters - Total number of clusters in index
        void gatherInformation(int*& levelSizes, ECPConfig*& cnfg, vector<vector<ECPNode<T,U,V>*>>& nodes,
                               int numClusters);

        /// Add combination for statistics policy
        /// \param clusterId
        /// \param descId
        void addCombination(uint32_t clusterId, uint32_t descId);

        ///
        /// \param clusterId
        /// \return
        double getClusterCount(uint32_t clusterId);

        ///
        /// \param r
        void updateSessionClusterCount(vector<uint32_t> suggs);

        ///
        /// \param clusterId
        /// \return
        int getFilterRemainingCount(uint32_t clusterId);

        ///
        /// \param clusterId
        /// \param passed
        void setFilterRemainingCount(uint32_t clusterId, int passed);

        ///
        /// \param clusterId
        /// \return
        double getEstimatedRemainingCount(uint32_t clusterId);

        ///
        /// \return
        ExpansionType getExpType();

        ///
        /// \return
        int getStatLevel();

    private:
        ExpansionType _expansionType;
        int _statLevel = 1;
        int _modality;
        std::string _descMapFname;
        vector<ECPCluster<T,U,V>*> _clusters;
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
