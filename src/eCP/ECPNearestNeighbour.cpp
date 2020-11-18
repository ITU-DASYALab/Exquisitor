#include "ECPNearestNeighbour.h"
#include "Helper.h"
//now "farthest" is actually "least distance", that is like before the "weakest" neigbor (farthest neigbor).

using namespace exq;
using std::cout;
using std::endl;

template<typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>::ECPNearestNeighbour(ExqDescriptor<T,U,V>* _query, uint64_t _k)
{
    // Copy the inputs
    query           = _query;
    mod             = -1;
    k               = _k;

    // Allocate space for the nearest neighbors
    descriptorIDs   = new uint64_t[k];
    clusterIDs      = new uint64_t[k];
    distances       = new double[k];

    // Set the distances once to max distance
    for (uint64_t i = 0; i < k; i++) {
        distances[i] = FLT_MAX;
    }

    // Init data for an empty array
    neighbors   = 0;
    farthest    = 0;


    for(uint64_t i = 0; i < N_TOTAL_FEAT; i++) {
        extendedValueArr[i] = 0;
    }

    for(uint64_t y = 0; y < QUERY_Y; y++) {
        for(uint64_t x = 0; x < QUERY_X; x++) {
            indexQueryArr[y][x] = 0;
        }
    }
    setHelperArrayForDistance();

};

template <typename T, typename U, typename V>
ECPNearestNeighbour<T,U,V>::ECPNearestNeighbour(int _mod, uint64_t _k)
{
    query           = NULL;
    mod             = _mod;
    k               = _k;

    // Allocate space for the nearest neighborsi
    descriptorIDs   = new uint64_t[k];
    clusterIDs      = new uint64_t[k];
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
ECPNearestNeighbour<T,U,V>::~ECPNearestNeighbour()
{
    // Clean up data
    delete[] clusterIDs;
    delete[] descriptorIDs;
    delete[] distances;
};

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::compareAndReplaceFarthest(ExqDescriptor<T,U,V>* data, uint64_t clusterid)
{
    double dist;
    // Find the distance.  If new k-nn found, then replace the farthest one
    if (mod == -1) {
        dist = Distance(data);
    } else {
        dist = calculateDistanceNN(data->feat_init, data->feat_ids, data->feat_ratios, mod);
    }

    if (dist < distances[farthest]) { //now we want to minimize distance to construct the tree
        ReplaceFarthest(data->id, clusterid, dist);
        FindFarthest();
    }
};

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::setHelperArrayForDistance() {

    int  bitShift[N_TOP_FEAT];
    int featID = 0;

    for(int j = 0; j < N_TOP_FEAT; j++) {
#ifdef R64_16
        bitShift[j] = j * 16; //+ 4;  //4 - 14 - 24 etc.
#else
        bitShift[j] = j * 10 + 4;  //4 - 14 - 24 etc.
#endif
    }

    double featVal = (query->feat_init & INIT_MASK) / INIT_N_VALS;
    int topIndx = query->feat_init >> INIT_SHIFT;

    indexQueryArr[0][0] = topIndx;
    extendedValueArr[topIndx] = featVal;

    for (int i = 0; i < N_TOP_FEAT; i++) {

        featVal *= ((query->feat_ratios >> bitShift[i]) & FEAT_MASK) / FEAT_DIVISOR;
        featID = (query->feat_ids >> bitShift[i]) & FEAT_MASK;

        indexQueryArr[0][i+1] = featID;
        extendedValueArr[featID] = featVal;

    }

};

//simplified projection calculations from query vector (weight vector from SVM) and data vector.
template <typename T, typename U, typename V>
double ECPNearestNeighbour<T,U,V>::Distance(ExqDescriptor<T,U,V>* data) {

    double dist = 0.0;

    int bitShift[N_TOP_FEAT];
    int featID = 0;

    for(int j = 0; j < N_TOP_FEAT; j++) {
#ifdef R64_16
        bitShift[j] = j * 16;// + 4;  //4 - 14 - 24 etc.
#else
        bitShift[j] = j * 10 + 4;  //4 - 14 - 24 etc.
#endif
    }
    double featVal = (data->feat_init & INIT_MASK) / INIT_N_VALS;
    int topFeatID = data->feat_init >> INIT_SHIFT;

    bool isFewerTop = (topFeatID == 0 && featVal == 0);
    double queryFeatVal = extendedValueArr[topFeatID];

    if(!isFewerTop && queryFeatVal != 0) {
        dist += (featVal - queryFeatVal) * (featVal - queryFeatVal);
        for (int i = 0; i < QUERY_X; i++) {
            if( indexQueryArr[0][i] == topFeatID) {
                indexQueryArr[1][i] = 1; //setting the id as found
            }
        }
    }
    else {
        dist+= featVal * featVal; //query desc does not include same id
    }

    for (int i = 0; i < N_TOP_FEAT; i++) {
        featVal *= ((data->feat_ratios >> bitShift[i]) & FEAT_MASK) / FEAT_DIVISOR;
        featID = (data->feat_ids >> bitShift[i]) & FEAT_MASK;

        //Preventing if Query point has index 0 in top features and data point has fewer than 7 top features (resulting in 0s for featID) then the value of
        //the index in Query point won't get dublicated.
        if (featID == 0 && featVal==0) {
            continue;
        }

        queryFeatVal = extendedValueArr[featID];
        if(queryFeatVal != 0) {
            dist += (featVal - queryFeatVal) * (featVal - queryFeatVal);
            for (int i = 0; i < QUERY_X; i++) {
                if(indexQueryArr[0][i] == featID) {
                    indexQueryArr[1][i] = 1; //setting the id as found
                }
            }
        }
        else {
            dist+= featVal * featVal;
        }
    }
    for (int i = 0; i < QUERY_X; i++) {
        if(indexQueryArr[1][i] == 1) {
            indexQueryArr[1][i] = 0;
        }
        else {
            int indx = indexQueryArr[0][i];
            dist+= extendedValueArr[indx] * extendedValueArr[indx];
        }
    }
    return dist;
};

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::ReplaceFarthest(uint64_t id, uint64_t clusterId, double dist)
{
    descriptorIDs[farthest] = id;
    clusterIDs[farthest]    = clusterId;
    distances[farthest]     = dist;
}

template <typename T, typename U, typename V>
inline void ECPNearestNeighbour<T,U,V>::FindFarthest()
{
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
void ECPNearestNeighbour<T,U,V>::PrintAnswer(int queryId)
{
    cout << neighbors << " : " << neighbors << endl;
    for (uint64_t i = 0; i < neighbors; i++)
        cout << queryId << ":"
            << i << ":"
            << descriptorIDs[i] << ":"
            << distances[i] << endl;
}

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::open()
{
    scanNext = 0;
}

template <typename T, typename U, typename V>
uint64_t* ECPNearestNeighbour<T,U,V>::nextClusterID()
{
    if ((uint64_t)scanNext < neighbors) {
        return &(clusterIDs[scanNext]);
    }
    return NULL;
}

template <typename T, typename U, typename V>
uint64_t* ECPNearestNeighbour<T,U,V>::next()
{
    if ((uint64_t)scanNext < neighbors) {
        return &(descriptorIDs[scanNext++]);
    }
    return NULL;
}

template <typename T, typename U, typename V>
double* ECPNearestNeighbour<T,U,V>::getDistanceForDescriptor()
{
    return &(distances[scanNext-1]);
}

template <typename T, typename U, typename V>
void ECPNearestNeighbour<T,U,V>::close()
{
    scanNext = -1;
}
