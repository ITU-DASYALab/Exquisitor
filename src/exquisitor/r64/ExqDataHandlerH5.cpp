//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

//#include <HDFql.hpp>
#include <hdf5.h>
#include <cstring>

#include "ExqDataHandlerH5.h"

using namespace exq;

ExqDataHandlerH5::ExqDataHandlerH5(vector<vector<string>>& compCnfgFiles, int modalities) {
    this->_numModalities = modalities;
    for (int m = 0; m < modalities; m++) {
        this->_descriptors.push_back(vector<ExqDescriptorR64*>());
        this->_topFeatPaths.push_back(compCnfgFiles[m][0]);
        this->_featIdsPaths.push_back(compCnfgFiles[m][1]);
        this->_ratiosPaths.push_back(compCnfgFiles[m][2]);
    }
}

void ExqDataHandlerH5::loadData(int workers) {
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

IExqDescriptor<uint64_t>* ExqDataHandlerH5::getDescriptor(uint32_t i) {
    return this->_descriptors[0][i];
}

IExqDescriptor<uint64>* ExqDataHandlerH5::getDescriptor(uint32_t i, int mod) {
    return this->_descriptors[mod][i];
}

int ExqDataHandlerH5::getTotalItemsCount(int mod) {
    return this->_descriptors[mod].size();
}

vector<bool> ExqDataHandlerH5::selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers,//vector<vector<double>>& model, vector<double>& bias,
                                                     ItemFilter& filters, bool resume) {
    return vector<bool>(1);
}

void ExqDataHandlerH5::getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                                    vector<vector<IExqDescriptor<uint64_t>*>>& descriptors,
                                                    unordered_set<uint32_t>& seenItems, ItemFilter& filters) {
    for (int m = 0; m < modalities; m++) {
        vector<IExqDescriptor<uint64_t>*> descs = vector<IExqDescriptor<uint64_t>*>();
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

ExqDataHandlerH5::~ExqDataHandlerH5() {
    for(auto vecs : this->_descriptors) {
        for (auto desc : vecs) {
            delete desc;
        }
    }
}

void ExqDataHandlerH5::loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile,
        int modality, int workers) {
    //char script[1024];
    vector<uint64_t> topFeats;
    vector<uint64_t> featIds;
    vector<uint64_t> featRatios;
    const string top = topFeatureFile;
    const string feat = featuresFile;
    const string ratio = ratiosFile;
    char* topFeatureFileC = new char[top.size() + 1];
    char* featuresFileC = new char[feat.size() + 1];
    char* ratiosFileC = new char[ratio.size() + 1];
    strcpy(topFeatureFileC, topFeatureFile.c_str());
    strcpy(featuresFileC, featuresFile.c_str());
    strcpy(ratiosFileC, ratiosFile.c_str());

    uint32_t totalItems = dataItemCount(topFeatureFileC, "/data");
    this->_descriptors[modality].reserve(totalItems);
    topFeats.resize(totalItems);
    featIds.resize(totalItems);
    featRatios.resize(totalItems);

    const char* dataset = "/data";
    loadHdf5Dataset(topFeats,
                    topFeatureFileC,
                    0,
                    (totalItems/workers),
                    dataset);
    loadHdf5Dataset(featIds,
                    featuresFileC,
                    0,
                    (totalItems/workers),
                    dataset);
    loadHdf5Dataset(featRatios,
                    ratiosFileC,
                    0,
                    (totalItems/workers),
                    dataset);
    //TODO: Add thread and chunk logic here

    for (uint32_t i = 0; i < totalItems; i++) {
        auto featureIds = ExqArray<uint64_t>(1);
        featureIds.setItem(featIds[i],0);
        auto featureRatios = ExqArray<uint64_t>(1);
        featureRatios.setItem(featRatios[i],0);
        _descriptors[modality].push_back(new ExqDescriptorR64(i, topFeats[i], featIds[i], featRatios[i]));
    }

    delete topFeatureFileC;
    delete featuresFileC;
    delete ratiosFileC;
}

/*
-------------------------------------------------------------------------------
_data_item_count()

Checks the number of items in the HDF5 iota-I64 dataset.
-------------------------------------------------------------------------------
*/
uint32_t ExqDataHandlerH5::dataItemCount    (char* filePath, const char* datasetName) {
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
void ExqDataHandlerH5::loadHdf5Dataset  (vector<uint64_t>& data,
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

    H5Dread(dSet, dType, memSpace, dSpace, H5P_DEFAULT, data.data());
    H5Sclose(memSpace);
    H5Sclose(dSpace);
    H5Dclose(dSet);
    H5Fclose(f);
}

void ExqDataHandlerH5::updateSessionInfo(vector<uint32_t> suggs) {}

void ExqDataHandlerH5::resetSessionInfo(int level) {}