//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

//#include <HDFql.hpp>
#include <hdf5.h>
#include <cstring>

#include "ExqDataHandlerR64.h"

using namespace exq;

ExqDataHandlerR64::ExqDataHandlerR64(vector<vector<string>>& compCnfgFiles, int modalities, vector<bool>& activeModalities, int workers) {
    for (int m = 0; m < N_MOD; m++) {
        if (!activeModalities[m]) {
            continue;
        }
        this->items = new vector<vector<ExqDescriptor<uint64_t,uint64_t,uint64_t>*>*>();
        this->items->reserve(modalities);
        for (int i = 0; i < modalities; i++) {
            this->items->push_back(new vector<ExqDescriptor<uint64_t, uint64_t, uint64_t>*>());
            loadDescriptorsFromFiles(
                    compCnfgFiles[m][TOP_FEATURES_PATH],
                    compCnfgFiles[m][FEATURE_IDS_PATH],
                    compCnfgFiles[m][FEATURE_RATIOS_PATH],
                    m,
                    workers
            );
        }
    }
}

void ExqDataHandlerR64::loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile, int modality, int workers) {
    //char script[1024];
    vector<uint64_t> topFeats;
    vector<uint64_t> featIds;
    vector<uint64_t> featRatios;
    //uint64_t* topFeats;
    //uint64_t* featIds;
    //uint64_t* featRatios;

    char topFeatureFileC[topFeatureFile.size() + 1];
    char featuresFileC[featuresFile.size() + 1];
    char ratiosFileC[ratiosFile.size() + 1];
    strcpy(topFeatureFileC, topFeatureFile.c_str());
    strcpy(featuresFileC, featuresFile.c_str());
    strcpy(ratiosFileC, ratiosFile.c_str());

    uint32_t totalItems = dataItemCount(topFeatureFileC, "/data");
    this->items->at(modality)->reserve(totalItems);
    topFeats.reserve(totalItems);
    featIds.reserve(totalItems);
    featRatios.reserve(totalItems);

    const char* dataset = "/data";
    loadHdf5Dataset((void**)&(topFeats),
                       topFeatureFileC,
                       0,
                       0,
                       dataset);
    loadHdf5Dataset((void**)&(featIds),
                       featuresFileC,
                       0,
                       0,
                       dataset);
    loadHdf5Dataset((void**)&(featRatios),
                       ratiosFileC,
                       0,
                       0,
                       dataset);
    //TODO: Add thread and chunk logic here
    //TODO: Is it possible to use HDFql instead? Utilize 3 cursors and run through them creating Descriptors.
    //sprintf(script, "USE FILE %s", topFeatureFileC);
    //HDFql::execute(script);
    //HDFql::variableRegister((void*)&topFeats);
    //sprintf(script, "SELECT FROM data INTO MEMORY %d", HDFql::variableGetNumber((void*)&topFeats));
    //HDFql::execute(script);
    //HDFql::execute("CLOSE FILE");

    //sprintf(script, "USE FILE %s", featuresFileC);
    //HDFql::execute(script);
    //HDFql::variableRegister((void*)&featIds);
    //sprintf(script, "SELECT FROM data INTO MEMORY %d", HDFql::variableGetNumber((void*)&featIds));
    //HDFql::execute(script);
    //HDFql::execute("CLOSE FILE");

    //sprintf(script, "SELECT FROM /data %s", ratiosFileC);
    //HDFql::execute(script);
    //HDFql::variableRegister((void*)&featRatios);
    //sprintf(script, "SELECT FROM data INTO MEMORY %d", HDFql::variableGetNumber((void*)&featRatios));
    //HDFql::execute(script);
    //HDFql::execute("CLOSE FILE");

    for (uint32_t i = 0; i < totalItems; i++) {
        items->at(modality)->push_back(new ExqDescriptor<uint64_t,uint64_t,uint64_t>(i, topFeats[i], featIds[i], featRatios[i]));
    }
}

ExqDataHandlerR64::~ExqDataHandlerR64() {}

/*
-------------------------------------------------------------------------------
_data_item_count()

Checks the number of items in the HDF5 iota-I64 dataset.
-------------------------------------------------------------------------------
*/
uint32_t ExqDataHandlerR64::dataItemCount    (char* filePath, const char* datasetName) {
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
void ExqDataHandlerR64::loadHdf5Dataset  (void** data,
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


