//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqDataHandlerECP.h"

using namespace exq;

template <typename T, typename U, typename V>
ExqDataHandlerECP<T,U,V>::ExqDataHandlerECP(vector<string> cnfgFiles, int modalities,
                                            vector<ExqFunctions<ExqDescriptor<T,U,V>>*>& functions,
                                            vector<int>& featureDimensions) {
    _indx = vector<ECPIndex<T,U,V>*>(modalities);
    _modalities = modalities;
    _descriptors = vector<vector<ExqDescriptor<T,U,V>*>>();
    _descriptors.reserve(_modalities);
    for (int m = 0; m < modalities; m++) {
        _indx[m] = new ECPIndex<T,U,V>(new ECPConfig(cnfgFiles[m]), functions[m], featureDimensions[m]);
    }
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::loadData(int workers) {
    cout << "(ExqHandler) Loading data into index" << endl;
    for (int m = 0; m < this->_modalities; m++) {
        _indx[m]->loadDescriptors(_descriptors[m]);
    }
    cout << "(ExqHandler) Done" << endl;
}


template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerECP<T,U,V>::getDescriptor(uint32_t i) {
    return _descriptors[0][i];
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerECP<T,U,V>::getDescriptor(uint32_t i, int mod) {
    return _descriptors[mod][i];
}

template <typename T, typename U, typename V>
int ExqDataHandlerECP<T,U,V>::getTotalItemsCount(int mod) {
    return _descriptors[mod].size();
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::selectClusters(vector<int> b, vector<vector<double>>& model, vector<double>& bias) {
    _b = b;
    for (int m = 0; m < this->_modalities; m++) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
        cout << "(ExqHandler) Selecting " << b[m] << " clusters from mod " << m << endl;
#endif
        _b[m] = b[m];
        _indx[m]->set_b_clusters(model[m], bias[m], _b[m]);
    }
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                                     vector<vector<ExqDescriptor<T,U,V>>>& descriptors,
                                                     unordered_set<uint32_t>& seenItems) {
    auto suggIdsPerMod = vector<vector<uint32_t>>(_modalities);
    auto clusterIdsPerMod = vector<vector<uint32_t>>(_modalities);
    auto totalData = vector<int>(_modalities);
    for (int m = 0; m < _modalities; m++) {
        int chnk = _b[m]/ totalSegments;
        suggIdsPerMod[m] = vector<uint32_t>();
        clusterIdsPerMod[m] = vector<uint32_t>();
        _indx[m]->search(chnk, totalData[m], suggIdsPerMod[m], clusterIdsPerMod[m], currentSegment, totalSegments);
    }
}

template class exq::ExqDataHandlerECP<uint64_t,uint64_t,uint64_t>;
