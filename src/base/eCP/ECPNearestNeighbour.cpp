#include "ECPNearestNeighbour.h"
//now "farthest" is actually "least distance", that is like before the "weakest" neigbor (farthest neigbor).

using namespace exq;
using std::cout;
using std::endl;

ECPNearestNeighbour::ECPNearestNeighbour(ExqDescriptorR64* _query, uint32_t _k,
                                         IExqFunctions<uint64_t>*& functions,
                                         int featureDimensions) {
#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPNN) Copy input" << endl;
#endif
    // Copy the inputs
    query           = _query;
    k               = _k;
    _functions      = functions;
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

ECPNearestNeighbour::ECPNearestNeighbour(uint32_t _k, IExqFunctions<uint64_t>*& functions,
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

ECPNearestNeighbour::~ECPNearestNeighbour() {
    // Clean up data
    delete[] clusterIDs;
    delete[] descriptorIDs;
    delete[] distances;
};

void ECPNearestNeighbour::compareAndReplaceFarthest(ExqDescriptorR64* data, uint32_t clusterid) {
    double dist;
    // Find the distance. If new k-nn found, then replace the farthest one
    dist = Distance(data);
    // This was used for kNN for old Exquisitor. If this is to be used again the ExqFunctions
    // should contain the necessary function. For this version it is better to create a new ECPkNearestNeighbor.cpp
    // which uses .h of this.
    //dist = calculateDistanceNN(data->feat_init, data->feat_ids, data->feat_ratios, mod);

    if (dist < distances[farthest]) { //now we want to minimize distance to construct the tree
        ReplaceFarthest(data->getId(), clusterid, dist);
        FindFarthest();
    }
};

void ECPNearestNeighbour::setHelperArrayForDistance() {
    auto arr = _functions->getDescriptorInformation(*query);

    for (int i = 0; i < arr.getSize(); i++) {
        pair<int,float> feat = arr.getItem(i);
        indexQueryArr[0][i] = feat.first;
        extendedValueArr[feat.first] = feat.second;
    }
};

//simplified projection calculations from query vector (weight vector from SVM) and data vector.
double ECPNearestNeighbour::Distance(ExqDescriptorR64* data) {
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

void ECPNearestNeighbour::FindFarthest() {
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

