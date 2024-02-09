#pragma once
#include "ECPConfig.h"
#include "ECPNode.h"
#include "ECPFarthestNeighbour.h"
#include "ECPNearestNeighbour.h"
#include "ECPCluster.h"
#include "ECPQueryOptimisationPolicies.h"
#include "../IExqFunctions.h"
#include "../Metadata/ItemProperties.h"

#include <queue>
#include <set>
#include <tuple>

namespace exq {
    using std::set;
    using std::vector;
    using std::priority_queue;
    using std::tuple;
    using std::make_tuple;

    /// Compare operator struct for priority queue
    struct PQ_Compare {
        bool operator()(const tuple<int,int,double>& lhs, const tuple<int,int,double>& rhs) {
            return std::get<2>(lhs) < std::get<2>(rhs);
        }
    };

    /// The eCP index tree class
    class ECPTree {
    public:
        /// Constructor
        /// \param _cnfg
        /// \param centroids
        /// \param numClusters
        /// \param func
        /// \param featureDimensions
        /// \param qop
        ECPTree(ECPConfig* _cnfg, vector<ExqDescriptorR64*> centroids, int numClusters,
                IExqFunctions<uint64_t>*& func, int featureDimensions,
                ECPQueryOptimisationPolicies*& qop, vector<ExqDescriptorR64*>* descs);

        ~ECPTree();

        /// Search the tree to find farthest clusters
        /// \param query
        /// \param bias
        /// \param k
        /// \param clusters
        /// \return
        ECPFarthestNeighbour* search(vector<float>& query, float bias, uint32_t k,
                                     vector<ECPCluster*>& clusters);

        ///
        /// \return
        inline bool check_pq() { return _pq.empty(); }

        ///
        /// \param query
        /// \param bias
        /// \param b
        /// \param clusters
        /// \return
        ECPFarthestNeighbour* search_pq(vector<float>& query, float bias, uint32_t b,
                                        vector<ECPCluster*>& clusters);
        /// Sanity checks
        void PrintTree();

    private:
        // The configuration of the tree
        ECPConfig* _cnfg;
        IExqFunctions<uint64_t>* _functions;
        int _featureDimensions;
        // The two-dimensional array of nodes (level, node) and the size at each level
        vector<vector<ECPNode*>> _nodes;
        int* _levelsizes{};
        // pq for incremental retrieval
        int _expCounter = 0;
        int _skipCounter = 0;
        int _numClusters = 0;
        priority_queue<tuple<int,int,float>,vector<tuple<int,int,float>>,PQ_Compare> _pq;
        set<uint32_t> _bfs;
        // query optimisation policies
        ECPQueryOptimisationPolicies* _qop;
        // limit for accepting clusters
        int _clusterSizeLimit = 1000000;

        vector<ExqDescriptorR64*>* _descs;

        // Build the tree using the list of descriptors at the bottom
        void buildTree(vector<ExqDescriptorR64*> centroids, int numClusters);

        void addChildAtLevel(ExqDescriptorR64* centroid, int level);

        // Find the proper node for a particular descriptor at a particular level
        // Used both for insertions and implementing the actual search
        ECPFarthestNeighbour* search(vector<float>& query, float bias, uint32_t b, uint32_t level,
                                     vector<ECPCluster*>& clusters);

        ECPNearestNeighbour* search(ExqDescriptorR64* query, uint32_t k, uint32_t level);


        //int getClusterCount(uint64_t id, vector<ECPCluster<T,U,V>*>& clusters, bool check=false);
    };
}