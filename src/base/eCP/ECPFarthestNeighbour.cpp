#include "ECPFarthestNeighbour.h"

using std::cout;
using std::endl;
using namespace exq;

ECPFarthestNeighbour::ECPFarthestNeighbour(vector<double>& query, double bias, uint32_t k,
                                           IExqFunctions<uint64_t>*& functions) {
    // Copy the inputs
    _query           = query;
    _bias            = bias;
    _k               = k;
    _functions       = functions;

    // Allocate space for the nearest neighbors
    _descriptorIDs = vector<uint32_t>(_k);
    _clusterIDs = vector<uint32_t>(_k);
    _distances = vector<double>(_k);

    // Set the distances once to max distance
    for (uint32_t i = 0; i < _k; i++) {
        _distances[i] = -DBL_MAX;
    }

    // Init data for an empty array
    _neighbors   = 0;
    _farthest    = 0;

    _countZeros = 0;
    _countIds = 0;
};

ECPFarthestNeighbour::~ECPFarthestNeighbour() {
    // Clean up data, vectors clean themselves
    _descriptorIDs.clear();
    _clusterIDs.clear();
    _distances.clear();
};

void ECPFarthestNeighbour::updateK(uint32_t k) {
    _descriptorIDs.resize(k);
    _clusterIDs.resize(k);
    _distances.resize(k);
    for (uint32_t i = _k; i < k; i++) {
        _distances[i] = -DBL_MAX;
    }
    _k = k;
    FindFarthest();
}

void ECPFarthestNeighbour::compareAndReplaceFarthest(ExqDescriptorR64* data, uint32_t clusterid) {
    // Find the distance.  If new k-nn found, then replace the farthest one
    double dist = _functions->distance(_query, _bias, *data);
    //printf("(MAIN) Distance: %f\n", dist);
    if (dist > _distances[_farthest]) { //now we want to maximize distance
        ReplaceFarthest(data->getId(), clusterid, dist);
        FindFarthest();
    }
};

//simplified projection calculations from query vector (weight vector from SVM) and data vector
inline double ECPFarthestNeighbour::distance(ExqDescriptorR64* data) {
    return _functions->distance(_query, _bias, *data);
};

inline void ECPFarthestNeighbour::printStuff() {
    cout << "count_ids is: " << _countIds << endl;
    cout << "cound_zeros is: " << _countZeros << endl;
}

inline void ECPFarthestNeighbour::ReplaceFarthest(uint32_t id, uint32_t clusterid, double dist) {
    _descriptorIDs[_farthest] = id;
    _clusterIDs[_farthest]    = clusterid;
    _distances[_farthest]     = dist;
}

inline void ECPFarthestNeighbour::FindFarthest() {
    // The idea is to maintain the lists in order, so that the farthest is always at the end of the list
    // Step one: Deal with the case where we have not found k neighbors
    if (_neighbors < _k-1) {
        // If we have not found k neighbors, then simply make the farthest the slot after the ones found so far
        // Distance is already initialized to FLT_MAX so the comparison works
        _neighbors++;
        _farthest = _neighbors;
    } else {
        // Else the last one is the farthest one
        _neighbors = _k;
        _farthest  = _neighbors-1;
    }

    // Step 2: Maintain the order
    // Special case: If we only have one neighbor, there is no need to re-order
    if (_k == 1)
        return;

    // Step 2: Maintain the order
    // General case: We need to propagate the new neighbor found to it's place in the ordered list
    //               We do this by swapping it with farther neighbors until we find a closer neighbor
    double dist = _distances[_neighbors-1];
    for (int i = _neighbors-2; i >= 0; --i) {
        if (_distances[i] < dist) { //now we only change order if the dist of new neigbor is LARGER than the other distances
            double tdis = _distances[i];
            int tid = _descriptorIDs[i];
            uint64_t tcl = _clusterIDs[i];
            // move forward
            _distances[i] = _distances[i+1];
            _descriptorIDs[i] = _descriptorIDs[i+1];
            _clusterIDs[i] = _clusterIDs[i+1];
            // write temp in the old slot.
            _distances[i+1] = tdis;
            _descriptorIDs[i+1] = tid;
            _clusterIDs[i+1] = tcl;
        } else {
            break;
        }
    }
}

inline vector<double> ECPFarthestNeighbour::getTopDistances() {
    return _distances;
}

inline vector<uint32_t> ECPFarthestNeighbour::getTopIds() {
    return _descriptorIDs;
}

inline void ECPFarthestNeighbour::PrintAnswer() {
    cout << _neighbors << " : " << _neighbors << endl;
    for (uint64_t i = 0; i < _neighbors; i++) {
        cout << i << ":" << _descriptorIDs[i] << ":" << _distances[i] << endl;
    }
}

inline void ECPFarthestNeighbour::open() {
    _scanNext = 0;
}

inline uint32_t* ECPFarthestNeighbour::nextClusterID() {
    if ((uint32_t)_scanNext < _neighbors) {
        return &(_clusterIDs[_scanNext]);
    }
    return NULL;
}


/* This is used for finding top clusters*/
uint32_t* ECPFarthestNeighbour::next() {
    if ((uint32_t)_scanNext < _neighbors) {
        return &(_descriptorIDs[_scanNext++]);
    }
    return NULL;
};

inline void ECPFarthestNeighbour::close() {
    _scanNext = -1;
}

//template<typename T, typename U, typename V>
//void ECPFarthestNeighbour<T,U,V>::printDescriptor(ExqDescriptor<T,U,V> *data) {
//    int bitShift[N_TOP_FEAT];
//    int featID = 0;
//    for(int j = 0; j < N_TOP_FEAT; j++) {
//#ifdef R64_16
//        bitShift[j] = j * 16;// + 4;
//#else
//        bitShift[j] = j * 10 + 4;
//#endif
//    }
//    double featVal = (data->feat_init & INIT_MASK) / INIT_N_VALS;
//    int topFeatID = data->feat_init >> INIT_SHIFT;
//    if (topFeatID == 99) {
//        count_ids++;
//    }
//
//    if(featVal == 0) {
//        count_zeros++;
//    }
//    cout << "topID: " << topFeatID << " - Value: " << featVal << " => ";
//    for (int i = 0; i < N_TOP_FEAT; i++) {
//        featVal *= ((data->feat_ratios >> bitShift[i]) & FEAT_MASK) / FEAT_DIVISOR;
//        featID = (data->feat_ids >> bitShift[i]) & FEAT_MASK;
//        cout << "ID: " << featID << " - Value: " << featVal << "=>";
//    }
//    cout << endl;
//}