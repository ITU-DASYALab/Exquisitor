#include "ECPTree.h"
#include <cmath>

using namespace exq;

template<typename T, typename U, typename V>
ECPTree<T,U,V>::ECPTree(ECPConfig* cnfg, vector<ExqDescriptor<T,U,V>*> centroids, int numClusters,
                        ExqFunctions<ExqDescriptor<T,U,V>*>*& func, int featureDimensions) {
    _cnfg = cnfg;
    _functions = func;
    _featureDimensions = featureDimensions;

    if (_cnfg->getNumLvls() < 2) {
        cout << "Tree: Level 2 index is the mininum for the Tree (level = " << _cnfg->getNumLvls() << ")" << endl;
        exit(EXIT_FAILURE);
    }

    BuildTree(centroids, numClusters);
}

template<typename T, typename U, typename V>
ECPTree<T,U,V>::~ECPTree()
{
    for (uint64_t l = 0; l < _cnfg->getNumLvls(); l++) {
        for (int i=0; i < _levelsizes[l]; i++) {
            delete _nodes[l][i];
        }
        _nodes[l].clear();
    }
    _nodes.clear();
    delete _levelsizes;
}

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::BuildTree(vector<ExqDescriptor<T,U,V>*> centroids, int numClusters)
{
    uint64_t fanout = (uint64_t) ceil(pow((double)numClusters, (1.0/(_cnfg->getNumLvls())) ) );

    // Allocate and set the level sizes
    _levelsizes = new int[_cnfg->getNumLvls()];
    _levelsizes[0] = fanout;
    for (uint64_t i = 1; i < _cnfg->getNumLvls()-1; i++)
        _levelsizes[i] = _levelsizes[i-1]*fanout;
    _levelsizes[_cnfg->getNumLvls()-1] = numClusters;

    // Allocate the internal node arrays
    // The last level will not be used in the search but including it makes the code cleaner
    _nodes = vector<vector<ECPNode<T,U,V>*>>(_cnfg->getNumLvls());
    for (uint64_t i = 0; i < _cnfg->getNumLvls(); i++)
        _nodes[i] = vector<ECPNode<T,U,V>*>(_levelsizes[i]);

    // The cluster centroids form all the node centroids, so we start by propagating them up in the hierarchy
    for (int i=0; i<numClusters; i++)
        for (uint64_t l=0; l < _cnfg->getNumLvls(); l++)
            if (i < _levelsizes[l])
                // Each descriptor is both the centroid of the node and a child of it (last parameter = 1)
                _nodes[l][i] = new ECPNode<T,U,V>(new ExqDescriptor<T,U,V>(centroids[i]), 1);

    cout << "Loading child nodes" << endl;
    // Then we take the extra centroids on each level L and insert into a node on level L-1
    for (uint64_t l = 1; l < _cnfg->getNumLvls(); l++)
        for (int i = _levelsizes[l-1]; i < _levelsizes[l]; i++)
            addChildAtLevel(new ExqDescriptor<T,U,V>(_nodes[l][i]->getCentroid()), l-1);
    cout << "Done" << endl;
}

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::addChildAtLevel(ExqDescriptor<T,U,V>* centroid, int level)
{
    uint64_t *clusterID;
    // Find the correct node to insert into
    ECPNearestNeighbour<T,U,V>* qa = search(centroid, 1, level);

    // Get the result and insert
    qa->open();
    while ((clusterID = qa->next()) != NULL)
        if (*clusterID != centroid->id)
            _nodes[level][*clusterID]->addChild(centroid);
    qa->close();

    delete qa;
}

template<typename T, typename U, typename V>
ECPFarthestNeighbour<T,U,V>* ECPTree<T,U,V>::search(vector<double>& query, double bias, uint64_t k, uint64_t depth, vector<ECPCluster<T,U,V>*>& clusters)
{
    //printf("Training...\n");
    if (depth > _cnfg->getNumLvls())
        return NULL;
    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees

    //printf("(MAIN) Searching Tree\n");
    auto result = new ECPFarthestNeighbour<T,U,V>(query, bias, k, _functions);
    //printf("(MAIN) AnswerMax initialized\n");
    for (int i = 0; i < _levelsizes[0]; i++) {
        if( clusters[_nodes[0][i]->get(0)->id]->getNumDescriptors() == 0) {
            continue;
        }
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
    };

    //printf("(MAIN) Scanning Clusters in answer\n");
    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint64_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPFarthestNeighbour<T,U,V>(query, bias, k, _functions);

        // Open the current result and scan those clusters
        result->open();
        //int x = 0;
        for (uint64_t i = 0; i < k; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint64_t *clusterID = result->next();
            if (clusterID == NULL)
                break;

            // Loop through the cluster and insert into results
            for (int j = 0; j < _nodes[l-1][*clusterID]->getNumChildren(); j++) {
                int numDescriptors = clusters[_nodes[l-1][*clusterID]->get(j)->id]->getNumDescriptors();
                if(numDescriptors == 0) {
                    continue;
                }
                //TODO: Make a global variable: clusterSizeLimit
                //if(numDescriptors > 100) {
                //  continue;
                //}
                //x++;

                //TODO: Might be useful at some point
                // Check if number of alreadySeen items makes this cluster useless
                //int not_seen = 0;
                //for (int p = 0; p < numDescriptors; p++) {
                //    if (alreadySeen[descriptor->descriptorList[p]->id]) {
                //        not_seen++;
                //    }
                //}
                //if (not_seen == numDescriptors) {
                //    continue;
                //}

                temp_result->compareAndReplaceFarthest(_nodes[l-1][*clusterID]->get(j), *clusterID);
            }
        }
        //printf("(MAIN) x: %d\n", x);
        result->close();

        // Replace the old result with the new result
        delete result;
        //delete temp_result;
        result = temp_result;
    }
    // Reached the lowest level, so return
    return result;
}

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>* ECPTree<T,U,V>::search(ExqDescriptor<T,U,V>* query, uint64_t k, uint64_t depth)
{
    if (depth > _cnfg->getNumLvls())
        return NULL;

    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
    auto result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);
    for (int i = 0; i < _levelsizes[0]; i++) {
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
    };

    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint64_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);

        // Open the current result and scan those clusters
        result->open();
        for (uint64_t i = 0; i < k; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint64_t *clusterID = result->next();
            if (clusterID == NULL)
                break;

            // Loop through the cluster and insert into results
            for (int j = 0; j < _nodes[l-1][*clusterID]->getNumChildren(); j++)
                temp_result->compareAndReplaceFarthest(_nodes[l-1][*clusterID]->get(j), *clusterID);
        }
        result->close();

        // Replace the old result with the new result
        delete result;
        result = temp_result;
    }

    // Reached the lowest level, so return
    return result;
}

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>* ECPTree<T,U,V>::search(ExqDescriptor<T,U,V>* query, uint64_t k, uint64_t depth, vector<ECPCluster<T,U,V>*>& clusters, int mod) {
    if (depth > _cnfg->getNumLvls())
        return NULL;

    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
    ECPNearestNeighbour<T,U,V>* result;
    //if (mod == -1) {
    result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);
    //} else {
    //    result = new ECPNearestNeighbour<T,U,V>(k, _functions, _featureDimensions);
    //}
    for (int i = 0; i < _levelsizes[0]; i++) {
        if(clusters[_nodes[0][i]->get(0)->id]->getNumDescriptors() == 0) {
            continue;
        }
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
    }
    
    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint64_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        ECPNearestNeighbour<T,U,V>* temp_result;
        //if (mod == -1) {
        temp_result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);
        //} else {
        //    temp_result = new ECPNearestNeighbour<T,U,V>(mod, k);
        //}

        // Open the current result and scan those clusters
        result->open();
        for (uint64_t i = 0; i < k; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint64_t *clusterID = result->next();
            if (clusterID == NULL)
                break;

            // Loop through the cluster and insert into results
            for (int j = 0; j < _nodes[l-1][*clusterID]->getNumChildren(); j++) {
                int numDescriptors = clusters[_nodes[l-1][*clusterID]->get(j)->id]->getNumDescriptors();
                if(numDescriptors == 0) {
                    continue;
                }
                temp_result->compareAndReplaceFarthest(_nodes[l-1][*clusterID]->get(j), *clusterID);
            }
        }
        result->close();

        // Replace the old result with the new result
        delete result;
        result = temp_result;
    }

    // Reached the lowest level, so return
    return result;
}

template<typename T, typename U, typename V>
ECPFarthestNeighbour<T,U,V>* ECPTree<T,U,V>::search(vector<double>& query, double bias, uint64_t k, vector<ECPCluster<T,U,V>*>& clusters) { return search(query, bias, k, _cnfg->getNumLvls() - 1, clusters); }

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>* ECPTree<T,U,V>::search(ExqDescriptor<T,U,V>* query, uint64_t k, vector<ECPCluster<T,U,V>*>& clusters, int mod) { return search(query, k, _cnfg->getNumLvls() - 1, clusters, mod); }

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::PrintTree()
{
    // Print the level info, ignoring the lowest level
    cout << "Level Sizes:" ;
    for (uint64_t l = 0; l < _cnfg->getNumLvls()-1; l++) {
        cout << "[" << l <<  "] = " << _levelsizes[l] << "; ";
    }
    cout << endl;

    // Print the structure, ignoring the lowest level
    string indent = "";
    cout << "******************************************" << endl;
    for (uint64_t l = 0; l < _cnfg->getNumLvls()-1; l++) {
        cout << "Level " << l << endl;
        for (int i=0; i < _levelsizes[l]; i++) {
            cout << indent << "Level " << l << " / Node " << i << endl;
            _nodes[l][i]->PrintNode(indent);
        }
        indent += "  ";
    }
    cout << "******************************************" << endl;
}

template class exq::ECPTree<uint64_t, uint64_t, uint64_t>;

