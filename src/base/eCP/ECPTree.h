#pragma once
#include "ECPConfig.h"
#include "ECPNode.h"
#include "ECPFarthestNeighbour.h"
#include "ECPNearestNeighbour.h"
#include "ECPCluster.h"
#include "../ExqFunctions.h"
#include "ECPQueryOptimisationPolicies.h"

#include <queue>
#include <set>
#include <tuple>

namespace exq {
    using std::set;
    using std::vector;
    using std::priority_queue;
    using std::tuple;
    using std::make_tuple;

    struct PQ_Compare {
        bool operator()(const tuple<int,int,double>& lhs, const tuple<int,int,double>& rhs) {
            return std::get<2>(lhs) < std::get<2>(rhs);
        }
    };

    template<typename T, typename U, typename V>
    class ECPTree {
    public:
        ECPTree(ECPConfig* _cnfg, vector<ExqDescriptor<T,U,V>*> centroids, int numClusters,
                ExqFunctions<ExqDescriptor<T,U,V>*>*& func, int featureDimensions,
                ECPQueryOptimisationPolicies<T,U,V>*& qop);

        ~ECPTree();

        // Search the tree to find farthest clusters
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint64_t k,
                                            vector<ECPCluster<T,U,V>*>& clusters);

        // Sanity checks
        void PrintTree();
    private:
        // The configuration of the tree
        ECPConfig* _cnfg;
        ExqFunctions<ExqDescriptor<T,U,V>*>* _functions;
        int _featureDimensions;
        // The two-dimensional array of nodes (level, node) and the size at each level
        vector<vector<ECPNode<T,U,V>*>> _nodes;
        int* _levelsizes;
        // pq for incremental retrieval
        double _threshold = 0.0;
        int _expCounter = 0;
        int _skipCounter = 0;
        priority_queue<tuple<int,int,double>,vector<tuple<int,int,double>>,PQ_Compare> pq;
        set<uint32_t> bfs;
        // query optimisation policies
        ECPQueryOptimisationPolicies<T,U,V>* _qop;
        // limit for accepting clusters
        int _clusterSizeLimit = 100000000;

        // Build the tree using the list of descriptors at the bottom
        void BuildTree(vector<ExqDescriptor<T,U,V>*> centroids, int numClusters);

        void addChildAtLevel(ExqDescriptor<T,U,V>* centroid, int level);

        // Find the proper node for a particular descriptor at a particular level
        // Used both for insertions and implementing the actual search
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint64_t b, uint64_t level,
                                            vector<ECPCluster<T,U,V>*>& clusters);

        ECPNearestNeighbour<T,U,V>* search(ExqDescriptor<T,U,V>* query, uint64_t k, uint64_t level);

        ECPFarthestNeighbour<T,U,V>* search_pq(vector<double>& query, double bias, uint32_t b,
                                               vector<ECPCluster<T,U,V>*>& clusters);

        //int getClusterCount(uint64_t id, vector<ECPCluster<T,U,V>*>& clusters, bool check=false);
    };
}