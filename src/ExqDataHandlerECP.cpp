//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqDataHandlerECP.h"

using namespace exq;

template <typename T, typename U, typename V>
ExqDataHandlerECP<T,U,V>::ExqDataHandlerECP(string cnfgFile, int modalities, int iota) {
    this->cnfgFile = cnfgFile;
    //TODO: Read cnfg file and get paths for index data files (ecp/Config.h)
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::loadData(int modalities, int workers) {
    //TODO: Load the data using index (ecp/Index.h)
    //TODO: Get all the descriptor pointers into descriptor vector
}


template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerECP<T,U,V>::getDescriptor(uint32_t i) {
    return this->_descriptors[0][i];
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerECP<T,U,V>::getDescriptor(uint32_t i, int mod) {
    return this->_descriptors[mod][i];
}

template <typename T, typename U, typename V>
int ExqDataHandlerECP<T,U,V>::getTotalItemsCount(int mod) {
    return this->_descriptors[mod].size();
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::selectClusters(vector<int> b, vector<vector<double>>& model, vector<double>& bias,
                                              ExqFunctions<ExqDescriptor<T,U,V>>& functions) {
    //TODO: Select b clusters from both modalities (ecp/Index.h)
    //TODO: Need to take an array of int instead of b to support multiple modality b
    //TODO: Take vector<vector<double>&> model to support multiple modalities
    //TODO: vector<double> bias to support multiple modalities
}

template <typename T, typename U, typename V>
void ExqDataHandlerECP<T,U,V>::getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                                    vector<vector<ExqDescriptor<T,U,V>>>& descriptors,
                                                    unordered_set<uint32_t>& seenItems) {
    //TODO: b / totalSegments = number of clusters per segment for both modalities
}