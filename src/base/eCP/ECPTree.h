#pragma once
#include "ECPConfig.h"
#include "ECPNode.h"
#include "ECPFarthestNeighbour.h"
#include "ECPNearestNeighbour.h"
#include "ECPCluster.h"
#include "ECPQueryOptimisationPolicies.h"
#include "../ExqFunctions.h"
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
    template<typename T, typename U, typename V>
    class ECPTree {
    public:
        /// Constructor
        /// \param _cnfg
        /// \param centroids
        /// \param numClusters
        /// \param func
        /// \param featureDimensions
        /// \param qop
        ECPTree(ECPConfig* _cnfg, vector<ExqDescriptor<T,U,V>*> centroids, int numClusters,
                ExqFunctions<ExqDescriptor<T,U,V>>*& func, int featureDimensions,
                ECPQueryOptimisationPolicies<T,U,V>*& qop, vector<ExqDescriptor<T,U,V>*>* descs);

        ~ECPTree();

        /// Search the tree to find farthest clusters
        /// \param query
        /// \param bias
        /// \param k
        /// \param clusters
        /// \return
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint32_t k,
                                            vector<ECPCluster<T,U,V>*>& clusters);

        ///
        /// \return
        inline bool check_pq() { return _pq.empty(); }

        ///
        /// \param query
        /// \param bias
        /// \param b
        /// \param clusters
        /// \return
        ECPFarthestNeighbour<T,U,V>* search_pq(vector<double>& query, double bias, uint32_t b,
                                               vector<ECPCluster<T,U,V>*>& clusters);
        /// Sanity checks
        void PrintTree();

    private:
        // The configuration of the tree
        ECPConfig* _cnfg;
        ExqFunctions<ExqDescriptor<T,U,V>>* _functions;
        int _featureDimensions;
        // The two-dimensional array of nodes (level, node) and the size at each level
        vector<vector<ECPNode<T,U,V>*>> _nodes;
        int* _levelsizes{};
        // pq for incremental retrieval
        int _expCounter = 0;
        int _skipCounter = 0;
        int _numClusters = 0;
        priority_queue<tuple<int,int,double>,vector<tuple<int,int,double>>,PQ_Compare> _pq;
        set<uint32_t> _bfs;
        // query optimisation policies
        ECPQueryOptimisationPolicies<T,U,V>* _qop;
        // limit for accepting clusters
        int _clusterSizeLimit = 100000000;

        vector<ExqDescriptor<T,U,V>*>* _descs;

        // Build the tree using the list of descriptors at the bottom
        void BuildTree(vector<ExqDescriptor<T,U,V>*> centroids, int numClusters);

        void addChildAtLevel(ExqDescriptor<T,U,V>* centroid, int level);

        // Find the proper node for a particular descriptor at a particular level
        // Used both for insertions and implementing the actual search
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint32_t b, uint32_t level,
                                            vector<ECPCluster<T,U,V>*>& clusters);

        ECPNearestNeighbour<T,U,V>* search(ExqDescriptor<T,U,V>* query, uint32_t k, uint32_t level);


        //int getClusterCount(uint64_t id, vector<ECPCluster<T,U,V>*>& clusters, bool check=false);
    };
}