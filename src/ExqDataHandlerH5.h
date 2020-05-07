//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLERH5_H
#define EXQUISITOR_EXQDATAHANDLERH5_H

#pragma once

#include <vector>
#include <array>
#include <string>

#include <hdf5.h>

#include "ExqDataHandler.h"
#include "ExqDescriptor.h"
#include "ExqConstants.h"

namespace exq {

    using std::vector;
    using std::array;
    using std::string;

    class ExqDataHandlerH5 : public ExqDataHandler<uint64_t,uint64_t,uint64_t> {
    public:
        ExqDataHandlerH5(vector<vector<string>>& compCnfgFiles, int modalities, vector<bool>& activeModalities, int workers);

        ~ExqDataHandlerH5();

    private:
        void loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile, int modality, int workers);
        uint32_t dataItemCount(char* filePath, const char* datasetName);
        void loadHdf5Dataset(void** data, char* filePath, hsize_t chunkOffset, hsize_t nChunk, const char* datasetName);
    }; //End of class ExqDataHandler

} //End of namespace exq

#endif //EXQUISITOR_EXQDATAHANDLERH5_H
