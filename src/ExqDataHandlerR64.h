//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLERR64_H
#define EXQUISITOR_EXQDATAHANDLERR64_H

#pragma once

#include <vector>
#include <array>
#include <string>

#include <hdf5.h>

#include "ExqDescriptor.h"
#include "ExqConstants.h"

namespace exq {

    using std::vector;
    using std::array;
    using std::string;

    class ExqDataHandlerR64 {
    public:
        //TODO: Change this to DescriptorContainer<T,U,V>
        vector<vector<ExqDescriptor<uint64_t,uint64_t,uint64_t>*>*> items;

        ExqDataHandlerR64(vector<vector<string>>& compCnfgFiles, int modalities, vector<bool>& activeModalities, int workers);

        ~ExqDataHandlerR64();


    private:
        void loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile, int modality, int workers);
        uint32_t dataItemCount(char* filePath, const char* datasetName);
        void loadHdf5Dataset(void** data, char* filePath, hsize_t chunkOffset, hsize_t nChunk, const char* datasetName);

    }; //End of class ExqDataHandler

} //End of namespace exq

#endif //EXQUISITOR_EXQDATAHANDLERR64_H
