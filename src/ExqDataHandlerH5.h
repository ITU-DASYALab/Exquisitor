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
#include "ExqFunctions.h"

namespace exq {

    using std::vector;
    using std::array;
    using std::string;

    template<typename T, typename U, typename V>
    class ExqDataHandlerH5 : public ExqDataHandler<ExqDescriptor<T,U,V>> {
    public:
        ExqDataHandlerH5(vector<vector<string>>& compCnfgFiles, int modalities);

        void loadData(int modalities, int workers) override;

        ExqDescriptor<T,U,V>* getDescriptor(uint32_t i) override;

        ExqDescriptor<T,U,V>* getDescriptor(uint32_t i, int mod) override;

        int getTotalItemsCount(int mod) override;

        void selectClusters(vector<int> b, vector<vector<double>>& model, vector<double>& bias,
                            ExqFunctions<ExqDescriptor<T,U,V>>& functions) override;

        void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                   vector<vector<ExqDescriptor<T,U,V>>>& descriptors,
                                   unordered_set<uint32_t>& seenItems) override;

        ~ExqDataHandlerH5();

    private:
        vector<vector<ExqDescriptor<T,U,V>*>> _descriptors;
        vector<string> _topFeatPaths;
        vector<string> _featIdsPaths;
        vector<string> _ratiosPaths;

        void loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile, int modality, int workers);
        uint32_t dataItemCount(char* filePath, const char* datasetName);
        void loadHdf5Dataset(void** data, char* filePath, hsize_t chunkOffset, hsize_t nChunk, const char* datasetName);
    }; //End of class ExqDataHandler

} //End of namespace exq

#endif //EXQUISITOR_EXQDATAHANDLERH5_H
