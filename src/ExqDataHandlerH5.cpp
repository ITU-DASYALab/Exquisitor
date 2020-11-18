//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

//#include <HDFql.hpp>
#include <hdf5.h>
#include <cstring>

#include "ExqDataHandlerH5.h"

using namespace exq;

template <typename T, typename U, typename V>
ExqDataHandlerH5<T,U,V>::ExqDataHandlerH5(vector<vector<string>>& compCnfgFiles, int modalities) {
    this->_numModalities = modalities;
    for (int m = 0; m < modalities; m++) {
        this->_descriptors.push_back(vector<ExqDescriptor<T,U,V>*>());
        this->_topFeatPaths.push_back(compCnfgFiles[m][TOP_FEATURES_PATH]);
        this->_featIdsPaths.push_back(compCnfgFiles[m][FEATURE_IDS_PATH]);
        this->_ratiosPaths.push_back(compCnfgFiles[m][FEATURE_RATIOS_PATH]);
    }
}

template <typename T, typename U, typename V>
void ExqDataHandlerH5<T,U,V>::loadData(int workers) {
    for (int m = 0; m < this->_numModalities; m++) {
        loadDescriptorsFromFiles(
                this->_topFeatPaths[m],
                this->_featIdsPaths[m],
                this->_ratiosPaths[m],
                m,
                workers
        );
    }
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerH5<T,U,V>::getDescriptor(uint32_t i) {
    return this->_descriptors[0][i];
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ExqDataHandlerH5<T,U,V>::getDescriptor(uint32_t i, int mod) {
    return this->_descriptors[mod][i];
}

template <typename T, typename U, typename V>
int ExqDataHandlerH5<T,U,V>::getTotalItemsCount(int mod) {
    return this->_descriptors[mod].size();
}

template <typename T, typename U, typename V>
void ExqDataHandlerH5<T,U,V>::selectClusters(vector<int> b, vector<vector<double>>& model, vector<double>& bias,
                                             vector<ExqFunctions<ExqDescriptor<T,U,V>>>& functions) {}

template <typename T, typename U, typename V>
void ExqDataHandlerH5<T,U,V>::getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                                    vector<vector<ExqDescriptor<T,U,V>>>& descriptors,
                                                    unordered_set<uint32_t>& seenItems) {
    for (int m = 0; m < modalities; m++) {
        vector<ExqDescriptor<T,U,V>> descs = vector<ExqDescriptor<T,U,V>>();
        int segmentSize = this->getTotalItemsCount(m)/totalSegments;
        int startIndex = currentSegment * segmentSize;
        int stopIndex;
        if (currentSegment + 1 == totalSegments) {
            stopIndex = this->getTotalItemsCount(m);
        } else {
            stopIndex = startIndex + segmentSize;
        }

        for (int i = startIndex; i < stopIndex; i++) {
            if (seenItems.find(i) != seenItems.end()) continue;
            descs.push_back(this->getDescriptor(i, m));
        }
        descriptors.push_back(descs);
    }
}

template <typename T, typename U, typename V>
ExqDataHandlerH5<T,U,V>::~ExqDataHandlerH5() {
    for(auto vecs : this->_descriptors) {
        for (auto desc : vecs) {
            delete desc;
        }
    }
}

template <typename T, typename U, typename V>
void ExqDataHandlerH5<T,U,V>::loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile,
        int modality, int workers) {
    //char script[1024];
    vector<T> topFeats;
    vector<U> featIds;
    vector<V> featRatios;

    char topFeatureFileC[topFeatureFile.size() + 1];
    char featuresFileC[featuresFile.size() + 1];
    char ratiosFileC[ratiosFile.size() + 1];
    strcpy(topFeatureFileC, topFeatureFile.c_str());
    strcpy(featuresFileC, featuresFile.c_str());
    strcpy(ratiosFileC, ratiosFile.c_str());

    uint32_t totalItems = dataItemCount(topFeatureFileC, "/data");
    this->_descriptors[modality].reserve(totalItems);
    topFeats.reserve(totalItems);
    featIds.reserve(totalItems);
    featRatios.reserve(totalItems);

    const char* dataset = "/data";
    loadHdf5Dataset((void**)&(topFeats),
                       topFeatureFileC,
                       0,
                       (totalItems/workers),
                       dataset);
    loadHdf5Dataset((void**)&(featIds),
                       featuresFileC,
                       0,
                       (totalItems/workers),
                       dataset);
    loadHdf5Dataset((void**)&(featRatios),
                       ratiosFileC,
                       0,
                       (totalItems/workers),
                       dataset);
    //TODO: Add thread and chunk logic here

    for (uint32_t i = 0; i < totalItems; i++) {
        this->_descriptors[modality].push_back(new ExqDescriptor<T,U,V>(i, topFeats[i], featIds[i], featRatios[i]));
    }
}

/*
-------------------------------------------------------------------------------
_data_item_count()

Checks the number of items in the HDF5 iota-I64 dataset.
-------------------------------------------------------------------------------
*/
template <typename T, typename U, typename V>
uint32_t ExqDataHandlerH5<T,U,V>::dataItemCount    (char* filePath, const char* datasetName) {
    hid_t f, dSet, dSpace;
    uint64_t itemCount;

    f = H5Fopen(filePath, H5F_ACC_RDONLY, H5P_DEFAULT);
    dSet = H5Dopen(f, datasetName, H5P_DEFAULT);
    dSpace = H5Dget_space(dSet);
    itemCount = H5Sget_simple_extent_npoints(dSpace);

    H5Sclose(dSpace);
    H5Dclose(dSet);
    H5Fclose(f);

    return itemCount;
}

/*
-------------------------------------------------------------------------------
_load_hdf5_dataset()

Loads a chunk of the iota-I64 HDF5 dataset.
-------------------------------------------------------------------------------
*/
template <typename T, typename U, typename V>
void ExqDataHandlerH5<T,U,V>::loadHdf5Dataset  (void** data,
                                         char* filePath,
                                         hsize_t chunkOffset,
                                         hsize_t nChunk,
                                         const char* datasetName) {
    hid_t f, dSet, dType, dSpace, memSpace;
    hsize_t offset[1], nData[1];

    f = H5Fopen(filePath, H5F_ACC_RDONLY, H5P_DEFAULT);
    dSet = H5Dopen(f, datasetName, H5P_DEFAULT);
    dType = H5Dget_type(dSet);
    dSpace = H5Dget_space(dSet);

    nData[0] = nChunk;
    offset[0] = chunkOffset;

    memSpace = H5Screate_simple(1, nData, NULL);
    H5Sselect_hyperslab(dSpace, H5S_SELECT_SET, offset, NULL,
                        nData, NULL);

    *data = malloc(nData[0] * sizeof(uint64_t));

    H5Dread(dSet, dType, memSpace, dSpace, H5P_DEFAULT, *data);
    H5Sclose(memSpace);
    H5Sclose(dSpace);
    H5Dclose(dSet);
    H5Fclose(f);
}

template class exq::ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>;
