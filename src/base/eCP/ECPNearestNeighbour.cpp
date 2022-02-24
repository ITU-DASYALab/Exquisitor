#include "ECPNearestNeighbour.h"
//now "farthest" is actually "least distance", that is like before the "weakest" neigbor (farthest neigbor).

using namespace exq;
using std::cout;
using std::endl;

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>::ECPNearestNeighbour(ExqDescriptor<T,U,V>* _query, uint32_t _k,
                                                ExqFunctions<ExqDescriptor<T,U,V>>*& functions,
                                                int featureDimensions) {
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Copy input" << endl;
#endif
    // Copy the inputs
    query           = _query;
    k               = _k;
    _functions = functions;
    _featureDimensions = featureDimensions;

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Allocate space" << endl;
#endif
    // Allocate space for the nearest neighbors
    descriptorIDs   = new uint32_t[k];
    clusterIDs      = new uint32_t[k];
    distances       = new double[k];

    // Set the distances once to max distance
    for (uint32_t i = 0; i < k; i++) {
        distances[i] = FLT_MAX;
    }

    // Init data for an empty array
    neighbors   = 0;
    farthest    = 0;

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Allocate helper arrays" << endl;
#endif
    extendedValueArr = vector<double>(_featureDimensions);
    for(uint32_t i = 0; i < (uint32_t)_featureDimensions; i++) {
        extendedValueArr[i] = 0;
    }

    indexQueryArr = vector<vector<double>>();
    for(uint32_t y = 0; y < 2; y++) {
        indexQueryArr.push_back(vector<double>(_functions->getDescFeatCount()));
        for(uint32_t x = 0; x < (uint32_t)_functions->getDescFeatCount(); x++) {
            indexQueryArr[y][x] = 0;
        }
    }

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Set helper array distance" << endl;
#endif
    setHelperArrayForDistance();
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Initialized" << endl;
#endif
};

template <typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>::ECPNearestNeighbour(uint32_t _k, ExqFunctions<ExqDescriptor<T,U,V>>*& functions,
                                                int featureDimensions) {
    k               = _k;
    _functions      = functions;
    _featureDimensions = featureDimensions;

    // Allocate space for the nearest neighborsi
    descriptorIDs   = new uint32_t[k];
    clusterIDs      = new uint32_t[k];
    distances       = new double[k];

    // Set the distances once to max distance
    for (uint64_t i = 0; i < k; i++) {
        distances[i] = FLT_MAX;
    }

    // Init data for an empty array
    neighbors   = 0;
    farthest    = 0;
}

template <typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>::~ECPNearestNeighbour() {
    // Clean up data
    delete[] clusterIDs;
    delete[] descriptorIDs;
    delete[] distances;
};

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::compareAndReplaceFarthest(ExqDescriptor<T,U,V>* data, uint32_t clusterid) {
    double dist;
    // Find the distance. If new k-nn found, then replace the farthest one
    dist = Distance(data);
    // This was used for kNN for old Exquisitor. If this is to be used again the ExqFunctions
    // should contain the necessary function. For this version it is better to create a new ECPkNearestNeighbor.cpp
    // which uses .h of this.
    //dist = calculateDistanceNN(data->feat_init, data->feat_ids, data->feat_ratios, mod);

    if (dist < distances[farthest]) { //now we want to minimize distance to construct the tree
        ReplaceFarthest(data->id, clusterid, dist);
        FindFarthest();
    }
};

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::setHelperArrayForDistance() {
    auto arr = _functions->getDescriptorInformation(*query);

    for (int i = 0; i < arr.getSize(); i++) {
        pair<int,float> feat = arr.getItem(i);
        indexQueryArr[0][i] = feat.first;
        extendedValueArr[feat.first] = feat.second;
    }
};

//simplified projection calculations from query vector (weight vector from SVM) and data vector.
template <typename T, typename U, typename V>
double ECPNearestNeighbour<T,U,V>::Distance(ExqDescriptor<T,U,V>* data) {
    double dist = 0.0;

#if defined(DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
    cout << "(ECPNN) Calculating nn distance from desc " << query->id << " to " << data->id << endl;
#endif
    auto arr = _functions->getDescriptorInformation(*data);
    double queryFeatVal = 0.0;
    for (int i = 0; i < arr.getSize(); i++) {
        pair<int,float> feat = arr.getItem(i);
        if (feat.first == 0 && feat.second == 0) continue;
        queryFeatVal = extendedValueArr[feat.first];
        if (queryFeatVal != 0) {
            dist += (feat.second - queryFeatVal) * (feat.second - queryFeatVal);
            for (int j = 0; j < _functions->getDescFeatCount(); j++) {
                if (indexQueryArr[0][j] == feat.first) {
                    indexQueryArr[1][j] = 1;
                }
            }
        } else {
            dist += feat.second * feat.second;
        }
    }

    for (int i = 0; i < _functions->getDescFeatCount(); i++) {
        if(indexQueryArr[1][i] == 1) {
            indexQueryArr[1][i] = 0;
        }
        else {
            int j = indexQueryArr[0][i];
            dist+= extendedValueArr[j] * extendedValueArr[j];
        }
    }
    return dist;
};

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::ReplaceFarthest(uint32_t id, uint64_t clusterId, double dist) {
    descriptorIDs[farthest] = id;
    clusterIDs[farthest]    = clusterId;
    distances[farthest]     = dist;
}

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::FindFarthest() {
    // The idea is to maintain the lists in order, so that the farthest is always at the end of the list

    // Step one: Deal with the case where we have not found k neighbors
    if (neighbors < k-1) {
        // If we have not found k neighbors, then simply make the farthest the slot after the ones found so far
        // Distance is already initialized to FLT_MAX so the comparison works
        neighbors++;
        farthest = neighbors;
    } else {
        // Else the last one is the farthest one
        neighbors = k;
        farthest  = neighbors-1;
    }

    // Step 2: Maintain the order
    // Special case: If we only have one neighbor, there is no need to re-order
    if (k == 1)
        return;

    // Step 2: Maintain the order
    // General case: We need to propagate the new neighbor found to it's place in the ordered list
    //               We do this by swapping it with farther neighbors until we find a closer neighbor
    double dist = distances[neighbors-1];
    for (int i = neighbors-2; i >= 0; --i) {
        if (distances[i] > dist) { //now we only change order if the dist of new neigbor is LESS than the other distances
            double  tdis        = distances[i];
            int    tid         = descriptorIDs[i];
            uint64_t tcl         = clusterIDs[i];
            // move forward
            distances[i]       = distances[i+1];
            descriptorIDs[i]   = descriptorIDs[i+1];
            clusterIDs[i]      = clusterIDs[i+1];
            // write temp in the old slot.
            distances[i+1]     = tdis;
            descriptorIDs[i+1] = tid;
            clusterIDs[i+1]    = tcl;
        } else {
            break;
        }
    }
}

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::PrintAnswer(int queryId) {
    cout << neighbors << " : " << neighbors << endl;
    for (uint64_t i = 0; i < neighbors; i++)
        cout << queryId << ":"
            << i << ":"
            << descriptorIDs[i] << ":"
            << distances[i] << endl;
}

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::open() {
    scanNext = 0;
}

template <typename T, typename U, typename V>
inline uint32_t* ECPNearestNeighbour<T,U,V>::nextClusterID() {
    if ((uint32_t)scanNext < neighbors) {
        return &(clusterIDs[scanNext]);
    }
    return NULL;
}

template <typename T, typename U, typename V>
inline uint32_t* ECPNearestNeighbour<T,U,V>::next() {
    if ((uint32_t)scanNext < neighbors) {
        return &(descriptorIDs[scanNext++]);
    }
    return NULL;
}

template <typename T, typename U, typename V>
inline double* ECPNearestNeighbour<T,U,V>::getDistanceForDescriptor() {
    return &(distances[scanNext-1]);
}

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::close() {
    scanNext = -1;
}

template class exq::ECPNearestNeighbour<uint64_t, uint64_t, uint64_t>;
