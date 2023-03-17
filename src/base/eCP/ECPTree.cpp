#include "ECPTree.h"
#include <cmath>

using namespace exq;

ECPTree::ECPTree(ECPConfig* cnfg, vector<ExqDescriptorR64*> centroids, int numClusters,
                 IExqFunctions<ExqDescriptorR64>*& func, int featureDimensions,
                 ECPQueryOptimisationPolicies*& qop,
                 vector<ExqDescriptorR64*>* descs) {
    _cnfg = cnfg;
    _functions = func;
    _featureDimensions = featureDimensions;
    _numClusters = numClusters;
    _levelsizes = new int[_cnfg->getNumLvls()];
    _qop = qop;
    _descs = descs;

    if (_cnfg->getNumLvls() < 2) {
        cout << "(ECPTree) Level 2 index is the mininum for the Tree (level = " << _cnfg->getNumLvls() << ")" << endl;
        exit(EXIT_FAILURE);
    }

    buildTree(centroids, numClusters);
}

ECPTree::~ECPTree() {
    for (uint32_t l = 0; l < _cnfg->getNumLvls(); l++) {
        for (int i=0; i < _levelsizes[l]; i++) {
            delete _nodes[l][i];
        }
        _nodes[l].clear();
    }
    _nodes.clear();
    delete _levelsizes;
}

void ECPTree::buildTree(vector<ExqDescriptorR64*> centroids, int numClusters) {
    uint32_t fanout = (uint64_t) ceil(pow((double)numClusters, (1.0/(_cnfg->getNumLvls())) ) );
    // Allocate and set the level sizes
    _levelsizes[0] = (int) fanout;
    for (uint32_t i = 1; i < _cnfg->getNumLvls()-1; i++)
        _levelsizes[i] = _levelsizes[i-1]*fanout;
    _levelsizes[_cnfg->getNumLvls()-1] = numClusters;
    // Allocate the internal node arrays
    // The last level will not be used in the search but including it makes the code cleaner
    _nodes = vector<vector<ECPNode*>>(_cnfg->getNumLvls());
    for (uint32_t i = 0; i < _cnfg->getNumLvls(); i++) {
        cout << "Levelsize " << i << ": " << _levelsizes[i] << endl;
        _nodes[i] = vector<ECPNode*>(_levelsizes[i]);
    }
    // The cluster centroids form all the node centroids, so we start by propagating them up in the hierarchy
    for (int i = 0; i < numClusters; i++) {
        for (uint32_t l = 0; l < _cnfg->getNumLvls(); l++) {
            if (i < _levelsizes[l]) {
                // Each descriptor is both the centroid of the node and a child of it (last parameter = 1)
                _nodes[l][i] = new ECPNode(new ExqDescriptorR64(centroids[i]), 1);
            }
        }
    }
    cout << "(ECPTree) Loading child nodes" << endl;
    // Then we take the extra centroids on each level L and insert into a node on level L-1
    for (uint32_t l = 1; l < _cnfg->getNumLvls(); l++) {
        for (int i = _levelsizes[l - 1]; i < _levelsizes[l]; i++)
            addChildAtLevel(new ExqDescriptorR64(_nodes[l][i]->getCentroid()), l - 1);
    }

    _qop->gatherInformation(_levelsizes, _cnfg, _nodes, numClusters);
    cout << "(ECPTree) Done" << endl;
}

void ECPTree::addChildAtLevel(ExqDescriptorR64* centroid, int level) {
    uint32_t *clusterID;
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Search for nn to place child" << endl;
#endif
    // Find the correct node to insert into
    ECPNearestNeighbour *qa = search(centroid, 1, level);

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Add children" << endl;
#endif
    // Get the result and insert
    qa->open();
    while ((clusterID = qa->next()) != NULL) {
        if (*clusterID != centroid->getId()) {
            _nodes[level][*clusterID]->addChild(centroid);
            if (level != (int) (_cnfg->getNumLvls() - 1)) {
                _nodes[level + 1][centroid->getId()]->parent = *clusterID;
            }
        }
    }
    qa->close();
    delete qa;
}

ECPFarthestNeighbour* ECPTree::search(vector<double>& query, double bias, uint32_t b, uint32_t depth,
                                             vector<ECPCluster*>& clusters) {
    if (depth > _cnfg->getNumLvls())
        return NULL;
    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
    auto result = new ECPFarthestNeighbour(query, bias, b, _functions);
    _bfs.clear();
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPTree) Initializing PQ" << endl;
#endif
    _pq = priority_queue<tuple<int,int,double>,vector<tuple<int,int,double>>,PQ_Compare>();
    for (int i = 0; i < _levelsizes[0]; i++) {
        if( clusters[_nodes[0][i]->get(0)->getId()]->getNumDescriptors() == 0) {
            continue;
        }
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
        // Add all nodes from the first level to the pq
        _pq.push(make_tuple(0, i, result->distance(_nodes[0][i]->get(0))));
    };

    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint32_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPFarthestNeighbour(query, bias, b, _functions);
        // Open the current result and scan those clusters
        result->open();
        for (uint32_t i = 0; i < b; i++) {
            // sanity check, if the found neighbours are fewer than treeB.
            uint32_t *clusterID = result->next();
            if (clusterID == NULL)
                break;
            // Loop through the cluster and insert into results
            for (int j = 0; j < _nodes[l-1][*clusterID]->getNumChildren(); j++) {
                int numDescriptors = clusters[_nodes[l-1][*clusterID]->get(j)->getId()]->getNumDescriptors();
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
        _bfs.insert(*clusterID);
        _pq.push(make_tuple(_cnfg->getNumLvls() - 1, *clusterID, DBL_MAX));
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

ECPFarthestNeighbour* ECPTree::search_pq(vector<double>& query, double bias, uint32_t b,
                                         vector<ECPCluster*>& clusters) {
    auto res = new ECPFarthestNeighbour(query, bias, b, _functions);
    uint32_t k = 0;
    uint32_t bx = b;
    double thresh = b*100;
    double total_count = 0.0;
    int exp = 0;
    int statLevel = _qop->getStatLevel();
    ExpansionType expansionType = _qop->getExpType();

    while (1) {
        while (!_pq.empty() && k != bx) {
            // Level Id, Node Id, Distance Score
            tuple<int,int,double> top = _pq.top();
            _pq.pop();
            if (std::get<0>(top) == (int) _cnfg->getNumLvls() - 1) { // If cluster, add to res
                int numDescriptors =
                        clusters[_nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->getId()]->getNumDescriptors();
                if (numDescriptors > _clusterSizeLimit || numDescriptors == 0) {
                    continue;
                }
#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "(ECPTree) Cluster " << _nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->id << " " << std::get<2>(top) << endl;
#endif
                double count =
                        _qop->getClusterCount(_nodes[std::get<0>(top)][std::get<1>(top)]->get(0)->getId());
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
                            double cnt = _qop->getClusterCount(_nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->getId());
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
                        _bfs.find(_nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->getId()) != _bfs.end()) {
                        continue;
                    }
                    _pq.push(make_tuple(
                            std::get<0>(top) + 1, // Next Level
                            _nodes[std::get<0>(top)][std::get<1>(top)]->get(i)->getId(), // Node/Cluster Id on Next Level
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

        if (_pq.empty() || total_count >= ((int)thresh)) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
            cout << "(ECPTree) Total Count " << total_count << endl;
            cout << "(ECPTree) PQ " << _pq.empty() << endl;
#endif
        //    if (pq.empty()) pq_empty = true;
            break;
        } else {
            if ((bx*2) < (uint32_t)_numClusters) {
                //Expand b by power of 2
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Total Count/Threshold:" << total_count << "/" << thresh << endl;
                cout << "Expanding b to " << (bx*2) << "/" << _numClusters<< endl;
#endif
                _expCounter++;
                exp++;
                bx *= 2;
                res->updateK(bx);
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
                res->updateK(bx);
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
        res->updateK(k);
    }

    //cout << "PQ(" << pq.size() << ") Searched!" << endl;
    return res;
}

ECPNearestNeighbour* ECPTree::search(ExqDescriptorR64* query, uint32_t k, uint32_t depth) {
    if (depth > _cnfg->getNumLvls())
        return NULL;
    // Need to find up to k neighbors from the tree
    // Start at the root, scan the children to find the best k sub-trees
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Init nn object" << endl;
#endif
    auto result = new ECPNearestNeighbour(query, k, _functions, _featureDimensions);
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPTree) Compare and replace farthest" << endl;
#endif
    for (int i = 0; i < _levelsizes[0]; i++) {
        result->compareAndReplaceFarthest(_nodes[0][i]->get(0));
    };
    // Now loop to the requested depth, always scanning the clusters in the answer
    for (uint32_t l = 1; l <= depth; l++) {
        // create a new result set for the current depth search.
        auto temp_result = new ECPNearestNeighbour(query, k, _functions, _featureDimensions);
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

ECPFarthestNeighbour* ECPTree::search(vector<double>& query, double bias, uint32_t k,
                                      vector<ECPCluster*>& clusters) {
    return search(query, bias, k, _cnfg->getNumLvls() - 1, clusters);
}

void ECPTree::PrintTree() {
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