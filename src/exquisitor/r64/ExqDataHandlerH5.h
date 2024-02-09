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

#include "../IExqDataHandler.h"
#include "../IExqDescriptor.h"
#include "../IExqFunctions.h"
#include "ExqDescriptorR64.h"

namespace exq {

    using std::vector;
    using std::array;
    using std::string;

    class ExqDataHandlerH5 : public IExqDataHandler<uint64_t> {
    public:
        ExqDataHandlerH5(vector<vector<string>>& compCnfgFiles, int modalities);

        void loadData(int workers) override;

        IExqDescriptor<uint64_t>* getDescriptor(uint32_t i) override;

        IExqDescriptor<uint64_t>* getDescriptor(uint32_t i, int mod) override;

        int getTotalItemsCount(int mod) override;

        vector<bool> selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers,//vector<vector<double>>& model, vector<double>& bias,
                            ItemFilter& filters, bool resume=false) override;

        void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                   vector<vector<IExqDescriptor<uint64_t>*>>& descriptors,
                                   unordered_set<uint32_t>& seenItems,
                                   ItemFilter& filters) override;

        void updateSessionInfo(vector<uint32_t> suggs) override;
        void resetSessionInfo(int level) override;

        ~ExqDataHandlerH5();

    private:
        int _numModalities;
        vector<vector<ExqDescriptorR64*>> _descriptors;
        vector<string> _topFeatPaths;
        vector<string> _featIdsPaths;
        vector<string> _ratiosPaths;

        void loadDescriptorsFromFiles(string topFeatureFile, string featuresFile, string ratiosFile, int modality, int workers);
        uint32_t dataItemCount(char* filePath, const char* datasetName);
        void loadHdf5Dataset(vector<uint64_t>& data, char* filePath, hsize_t chunkOffset, hsize_t nChunk, const char* datasetName);
    }; //End of class ExqDataHandler

} //End of namespace exq

#endif //EXQUISITOR_EXQDATAHANDLERH5_H
