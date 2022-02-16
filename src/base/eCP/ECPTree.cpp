#include "ECPTree.h"
#include <cmath>

using namespace exq;

template<typename T, typename U, typename V>
ECPTree<T,U,V>::ECPTree(ECPConfig* cnfg, vector<ExqDescriptor<T,U,V>*> centroids, int numClusters,
                        ExqFunctions<ExqDescriptor<T,U,V>>*& func, int featureDimensions,
                        ECPQueryOptimisationPolicies<T,U,V>*& qop) {
    _cnfg = cnfg;
    _functions = func;
    _featureDimensions = featureDimensions;
    _numClusters = numClusters;

    if (_cnfg->getNumLvls() < 2) {
        cout << "(ECPTree) Level 2 index is the mininum for the Tree (level = " << _cnfg->getNumLvls() << ")" << endl;
        exit(EXIT_FAILURE);
    }

    _qop = qop;

    BuildTree(centroids, numClusters);
}

template<typename T, typename U, typename V>
ECPTree<T,U,V>::~ECPTree() {
    for (uint32_t l = 0; l < _cnfg->getNumLvls(); l++) {
        for (int i=0; i < _levelsizes[l]; i++) {
            delete _nodes[l][i];
        }
        _nodes[l].clear();
    }
    _nodes.clear();
    delete _levelsizes;
}

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::BuildTree(vector<ExqDescriptor<T,U,V>*> centroids, int numClusters) {
    uint32_t fanout = (uint64_t) ceil(pow((double)numClusters, (1.0/(_cnfg->getNumLvls())) ) );
    // Allocate and set the level sizes
    _levelsizes = new int[_cnfg->getNumLvls()];
    _levelsizes[0] = fanout;
    for (uint32_t i = 1; i < _cnfg->getNumLvls()-1; i++)
        _levelsizes[i] = _levelsizes[i-1]*fanout;
    _levelsizes[_cnfg->getNumLvls()-1] = numClusters;
    // Allocate the internal node arrays
    // The last level will not be used in the search but including it makes the code cleaner
    _nodes = vector<vector<ECPNode<T,U,V>*>>(_cnfg->getNumLvls());
    for (uint32_t i = 0; i < _cnfg->getNumLvls(); i++)
        _nodes[i] = vector<ECPNode<T,U,V>*>(_levelsizes[i]);
    // The cluster centroids form all the node centroids, so we start by propagating them up in the hierarchy
    for (int i=0; i<numClusters; i++) {
        for (uint32_t l = 0; l < _cnfg->getNumLvls(); l++) {
            if (i < _levelsizes[l]) {
                // Each descriptor is both the centroid of the node and a child of it (last parameter = 1)
                _nodes[l][i] = new ECPNode<T, U, V>(new ExqDescriptor<T, U, V>(centroids[i]), 1);
            }
        }
    }
    cout << "(ECPTree) Loading child nodes" << endl;
    // Then we take the extra centroids on each level L and insert into a node on level L-1
    for (uint32_t l = 1; l < _cnfg->getNumLvls(); l++) {
        for (int i = _levelsizes[l - 1]; i < _levelsizes[l]; i++)
            addChildAtLevel(new ExqDescriptor<T, U, V>(_nodes[l][i]->getCentroid()), l - 1);
    }

    _qop->gatherInformation(_levelsizes, _cnfg, _nodes, numClusters);
    cout << "(ECPTree) Done" << endl;
}

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::addChildAtLevel(ExqDescriptor<T,U,V>* centroid, int level) {
    uint32_t *clusterID;
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Search for nn to place child" << endl;
#endif
    // Find the correct node to insert into
    ECPNearestNeighbour<T, U, V> *qa = search(centroid, 1, level);

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Add children" << endl;
#endif
    // Get the result and insert
    qa->open();
    while ((clusterID = qa->next()) != NULL) {
        if (*clusterID != centroid->id) {
            _nodes[level][*clusterID]->addChild(centroid);
            if (level != (int) (_cnfg->getNumLvls() - 1)) {
                _nodes[level + 1][centroid->id]->parent = *clusterID;
            }
        }
    }
    qa->close();
    delete qa;
}

template<typename T, typename U, typename V>
ECPFarthestNeighbour<T,U,V>* ECPTree<T,U,V>::search(vector<double>& query, double bias, uint32_t b, uint32_t depth,
                                                    vector<ECPCluster<T,U,V>*>& clusters) {
    if (depth > _cnfg->getNumLvls())
        return NULL;
    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
    auto result = new ECPFarthestNeighbour<T,U,V>(query, bias, b, _functions);
    bfs.clear();
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPTree) Initializing PQ" << endl;
#endif
    pq = priority_queue<tuple<int,int,double>,vector<tuple<int,int,double>>,PQ_Compare>();
    for (int i = 0; i < _levelsizes[0]; i++) {
        if( clusters[_nodes[0][i]->get(0)->id]->getNumDescriptors() == 0) {
            continue;
        }
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
        // Add all nodes from the first level to the pq
        pq.push(make_tuple(0,i,result->distance(_nodes[0][i]->get(0))));
    };

    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint32_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPFarthestNeighbour<T,U,V>(query, bias, b, _functions);
        // Open the current result and scan those clusters
        result->open();
        for (uint32_t i = 0; i < b; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint32_t *clusterID = result->next();
            if (clusterID == NULL)
                break;
            // Loop through the cluster and insert into results
            for (int j = 0; j < _nodes[l-1][*clusterID]->getNumChildren(); j++) {
                int numDescriptors = clusters[_nodes[l-1][*clusterID]->get(j)->id]->getNumDescriptors();
                if(numDescriptors == 0 || numDescriptors > _clusterSizeLimit) {
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
    // Add bfs results into pq with max score
    uint32_t *clusterID;
    result->open();
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPTree) Inserting found clusters" << endl;
#endif
    while ((clusterID = result->next()) != NULL) {
        bfs.insert(*clusterID);
        pq.push(make_tuple(_cnfg->getNumLvls()-1,*clusterID,DBL_MAX));
    }
    delete result;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPTree) Searching PQ" << endl;
#endif
    // Check if early incremental retrieval is necessary
    result = this->search_pq(query, bias, b,clusters);
    // Reached the lowest level, so return
    return result;
}

template<typename T, typename U, typename V>
ECPFarthestNeighbour<T,U,V>* ECPTree<T,U,V>::search_pq(vector<double>& query, double bias, uint32_t b,
                                                       vector<ECPCluster<T,U,V>*>& clusters) {
    auto res = new ECPFarthestNeighbour<T,U,V>(query, bias, b, _functions);
    uint32_t k = 0;
    uint32_t bx = b;
    double thresh = b*100;
    double total_count = 0.0;
    int exp = 0;
    int statLevel = _qop->getStatLevel();
    ExpansionType expansionType = _qop->getExpType();

    while (1) {
        while (!pq.empty() && k != bx) {
            // Level Id, Node Id, Distance Score
            tuple<int,int,double> top = pq.top();
            pq.pop();
            if (std::get<0>(top) == (int) _cnfg->getNumLvls() - 1) { // If cluster, add to res
                int numDescriptors =
                        clusters[_nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->id]->getNumDescriptors();
                if (numDescriptors > _clusterSizeLimit || numDescriptors == 0) {
                    continue;
                }
#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ECPTree) Cluster " << _nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->id << " " << std::get<2>(top) << endl;
#endif
                double count =
                        _qop->getClusterCount(_nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->id);
                if (count > 0.0) {
                    total_count += count;
                    res->compareAndReplaceFarthest(_nodes[std::get<0>(top)][std::get<1>(top)]->get(0));
                    k++;
                } else {
                    _skipCounter++;
                }
            } else { // If Node, add children to priority queue
#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ECPTree) Node " << _nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->id << " " << std::get<2>(top) << endl;
#endif
                for (int i = 0; i < _nodes[std::get<0>(top)][std::get<1>(top)]->getNumChildren(); i++) {
                    if (expansionType == ESTIMATED_REMAINING_CNT || expansionType == ALL_REMAINING_CNT) {
                        if (std::get<0>(top) + 1 == statLevel) {
                            double cnt = _qop->getClusterCount(_nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->id);
#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                            cout << _nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->id << " count: " << cnt << endl;
#endif
                            if (cnt == 0.0) {
                                _skipCounter++;
                                continue;
                            }
                        }
                    }
                    // This cluster has already been processed
                    if (std::get<0>(top) + 1 == (int) _cnfg->getNumLvls() - 1 &&
                        bfs.find(_nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->id) != bfs.end()) {
                        continue;
                    }
                    pq.push(make_tuple(
                            std::get<0>(top) + 1, // Next Level
                            _nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->id, // Node/Cluster Id on Next Level
                            res->distance(_nodes[std::get<0>(top)][std::get<1>(top)]->get(i)) // Score of Node/Cluster
                    ));
                }
            }
        }
        if (exp > 0) {
            if (total_count > 0.0 && bx*2 > 65536) break; //Break after 2^16
            thresh *= 0.5; // Half threshold each expansion
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
            cout << "threshold: " << thresh << " next bx: " << bx*2 << endl;
#endif
        }

        if (pq.empty() || total_count >= ((int)thresh)) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
            cout << "(ECPTree) Total Count " << total_count << endl;
#endif
        //    if (pq.empty()) pq_empty = true;
            break;
        } else {
            if ((bx*2) < (uint32_t)_numClusters) {
                //Expand b by power of 2
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Expanding b to " << (bx*2) << "/" << _numClusters<< endl;
#endif
                _expCounter++;
                exp++;
                bx *= 2;
                res->setK(bx);
            } else if (bx == (uint32_t)_numClusters) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Can not expand further" << endl;
#endif
                break;
            } else {
                _expCounter++;
                exp++;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Expanding b to max clusters " << _numClusters << endl;
#endif
                bx = _numClusters;
                res->setK(bx);
            }
        }
    }
    //if (pq_empty && (expandType == 2 || expandType == 6 || expandType == 4 || expandType == 8) && k < bx) {
    //    for (int i = 0; i < this->numClusters; i++) {
    //        if (filterCacheEC[mod][i] != -1) {
    //            if (filterCacheEC[mod][i] - filterCacheRC[mod][i] > 0) {
    //                //cout << "Remaining Cluster " << i << " with " << filterCacheEC[mod][i] - filterCacheRC[mod][i] << endl;
    //                //cout << nodes[(int)cnfg->getNumLvls()-1][i]->get(0)->id << endl;
    //                res->compareAndReplaceFarthest(_nodes[(int)_cnfg->getNumLvls()-1][i]->get(0));
    //                k++;
    //            }
    //            if (k == bx) {
    //                break;
    //            }
    //        }
    //    }
    //}
    if (k != bx) {
        res->setK(k);
    }

    //cout << "PQ(" << pq.size() << ") Searched!" << endl;
    return res;
}

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>* ECPTree<T,U,V>::search(ExqDescriptor<T,U,V>* query, uint32_t k, uint32_t depth) {
    if (depth > _cnfg->getNumLvls())
        return NULL;
    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Init nn object" << endl;
#endif
    auto result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Compare and replace farthest" << endl;
#endif
    for (int i = 0; i < _levelsizes[0]; i++) {
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
    };
    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint32_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPNearestNeighbour<T,U,V>(query, k, _functions, _featureDimensions);
        // Open the current result and scan those clusters
        result->open();
        for (uint32_t i = 0; i < k; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint32_t *clusterID = result->next();
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
ECPFarthestNeighbour<T,U,V>* ECPTree<T,U,V>::search(vector<double>& query, double bias, uint32_t k,
                                                    vector<ECPCluster<T,U,V>*>& clusters) {
    return search(query, bias, k, _cnfg->getNumLvls() - 1, clusters);
}

template<typename T, typename U, typename V>
void ECPTree<T,U,V>::PrintTree() {
    // Print the level info, ignoring the lowest level
    cout << "Level Sizes:" ;
    for (uint32_t l = 0; l < _cnfg->getNumLvls()-1; l++) {
        cout << "[" << l <<  "] = " << _levelsizes[l] << "; ";
    }
    cout << endl;
    // Print the structure, ignoring the lowest level
    string indent = "";
    cout << "******************************************" << endl;
    for (uint32_t l = 0; l < _cnfg->getNumLvls()-1; l++) {
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