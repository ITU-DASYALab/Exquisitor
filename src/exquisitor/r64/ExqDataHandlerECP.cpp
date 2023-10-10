//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqDataHandlerECP.h"

using namespace exq;

ExqDataHandlerECP::ExqDataHandlerECP(vector<string> cnfgFiles, int modalities,
                                     vector<IExqFunctions<uint64_t>*>& functions,
                                     vector<int>& featureDimensions,
                                     vector<ItemProperties> itemProps,
                                     vector<vector<Props>> vidProps,
                                     ExpansionType expType,
                                     int statLevel) {
    _indx = vector<ECPIndex*>(modalities);
    _modalities = modalities;
    _descriptors = vector<vector<ExqDescriptorR64*>>(_modalities);
    for (int m = 0; m < modalities; m++) {
        _descriptors[m] = vector<ExqDescriptorR64*>();
        _indx[m] = new ECPIndex(new ECPConfig(cnfgFiles[m]), functions[m], featureDimensions[m],
                                m, &_descriptors[m], itemProps, vidProps, expType, statLevel);
    }
}

ExqDataHandlerECP::~ExqDataHandlerECP() {
    for (int m = 0; m < _modalities; m++) {
        delete _indx[m];
    }
}

void ExqDataHandlerECP::loadData(int workers) {
    cout << "(ExqHandler) Loading data into index...";
    for (int m = 0; m < this->_modalities; m++) {
        _indx[m]->loadDescriptors();
    }
    cout << "Done!" << endl;
}


inline IExqDescriptor<uint64_t>* ExqDataHandlerECP::getDescriptor(uint32_t i) {
    return (IExqDescriptor<uint64_t>*) _descriptors[0][i];
}

inline IExqDescriptor<uint64_t>* ExqDataHandlerECP::getDescriptor(uint32_t i, int mod) {
    return (IExqDescriptor<uint64_t>*) _descriptors[mod][i];
}

inline int ExqDataHandlerECP::getTotalItemsCount(int mod) {
    return (int) _descriptors[mod].size();
}

vector<bool> ExqDataHandlerECP::selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers,
                                               ItemFilter& filters, bool resume) {
    _b = b;
    vector<bool> empty = vector<bool>(_modalities);
    for (int m = 0; m < _modalities; m++) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
        cout << "(ExqHandler) Selecting " << b[m] << " clusters from mod " << m << endl;
#endif
        if (b[m] == 0) {
            empty[m] = true;
            continue;
        }
        _b[m] = b[m];
        //empty[m] = _indx[m]->set_b_clusters(model[m], bias[m], _b[m]);
        empty[m] = _indx[m]->set_b_clusters(classifiers[m]->getWeights(), classifiers[m]->getBias(), _b[m],
                                            resume);
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
        cout << "(ExqHandler) PQ state: " << empty[m] << endl;
#endif
    }
    return empty;
}

void ExqDataHandlerECP::getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                              vector<vector<IExqDescriptor<uint64_t>*>>& descriptors,
                                              unordered_set<uint32_t>& seenItems, ItemFilter& filters) {
    auto suggIdsPerMod = vector<vector<uint32_t>>(_modalities);
    auto clusterIdsPerMod = vector<vector<uint32_t>>(_modalities);
    auto totalData = vector<int>(_modalities);
    for (int m = 0; m < _modalities; m++) {
        int chnk = _b[m] / totalSegments;
        suggIdsPerMod[m] = vector<uint32_t>();
        _indx[m]->search(chnk, totalData[m], suggIdsPerMod[m], currentSegment,
                         totalSegments, seenItems, filters);
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(ExqHandler) totalData[" << m << "]: " << totalData[m] << endl;
        cout << "(ExqHandler) suggIdsPerMod[" << m << "].size(): " << suggIdsPerMod[m].size() << endl;
        cout << "(ExqHandler) clusterIdsPerMod[" << m << "].size(): " << clusterIdsPerMod[m].size() << endl;
#endif
        descriptors[m] = vector<IExqDescriptor<uint64_t>*>();
        for (int i = 0; i < (int)suggIdsPerMod[m].size(); i++) {
            descriptors[m].push_back(_descriptors[m][suggIdsPerMod[m][i]]);
        }
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(ExqHandler) descriptors[" << m << "].size(): " << descriptors[m].size() << endl;
        cout << "(ExqHandler) descriptorIds: ";
        for (int i = 0; i < (int)suggIdsPerMod[m].size(); i++) {
            cout << descriptors[m][i]->getId() << ", ";
        }
        cout << endl;
#endif
    }
}

void ExqDataHandlerECP::updateSessionInfo(vector<uint32_t> suggs) {
    for (int m = 0; m < _modalities; m++) {
        _indx[m]->updateSessionInfo(suggs);
    }
}

void ExqDataHandlerECP::resetSessionInfo(int level) {
    for (int m = 0; m < _modalities; m++) {
        _indx[m]->resetSessionInfo(level);
    }
}