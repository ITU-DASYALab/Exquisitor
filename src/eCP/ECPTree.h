#pragma once
#include "ECPConfig.h"
#include "ECPNode.h"
#include "ECPFarthestNeighbour.h"
#include "ECPNearestNeighbour.h"
#include "ECPCluster.h"
#include "../ExqFunctions.h"

namespace exq {
    using std::vector;

    template<typename T, typename U, typename V>
    class ECPTree {
    public:
        ECPTree(ECPConfig* _cnfg, vector<ExqDescriptor<T,U,V>*> centroids, int numClusters, ExqFunctions<ExqDescriptor<T,U,V>>*& func);

        ~ECPTree();

        // Search the tree to find farthest clusters
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint64_t k, vector<ECPCluster<T,U,V>*>& clusters);

        ECPNearestNeighbour<T,U,V>* search(ExqDescriptor<T,U,V>* query, uint64_t k, vector<ECPCluster<T,U,V>*>& clusters, int mod);

        void printClusterItems(uint64_t clusterID);

        // Sanity checks
        void PrintTree();
    private:
        // The configuration of the tree
        ECPConfig* _cnfg;
        ExqFunctions<ExqDescriptor<T,U,V>>** _functions;
        // The two-dimensional array of nodes (level, node) and the size at each level
        vector<vector<ECPNode<T,U,V>*>> _nodes;
        int* _levelsizes;

        // Build the tree using the list of descriptors at the bottom
        void BuildTree(vector<ExqDescriptor<T,U,V>*> centroids, int numClusters);

        void addChildAtLevel(ExqDescriptor<T,U,V>* centroid, int level);

        // Find the proper node for a particular descriptor at a particular level
        // Used both for insertions and implementing the actual search
        // Answer *search(Descriptor *query, uint_t k, uint_t level);
        ECPFarthestNeighbour<T,U,V>* search(vector<double>& query, double bias, uint64_t k, uint64_t level, vector<ECPCluster<T,U,V>*>& clusters);

        ECPNearestNeighbour<T,U,V>* search(ExqDescriptor<T,U,V>* query, uint64_t k, uint64_t level, vector<ECPCluster<T,U,V>*>& clusters, int mod);

        ECPNearestNeighbour<T,U,V>* search(ExqDescriptor<T,U,V>* query, uint64_t k, uint64_t level);

    };
}