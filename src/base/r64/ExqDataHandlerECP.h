//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLERECP_H
#define EXQUISITOR_EXQDATAHANDLERECP_H

#pragma once

#include <string>

#include "../IExqDataHandler.h"
#include "../IExqDescriptor.h"
#include "ExqDescriptorR64.h"

#include "../eCP/ECPIndex.h"

namespace exq {

    using std::string;

    class ExqDataHandlerECP : public IExqDataHandler<uint64_t>{
    public:
        /**
         * Constructor for loading the eCP index
         * \param cnfgFile - Config file containing metadata information about the cluster index
         */
        ExqDataHandlerECP(vector<string> cnfgFiles, int modalities,
                          vector<IExqFunctions<uint64_t>*>& functions, vector<int>& featureDimensions,
                          vector<ItemProperties> itemProps, vector<vector<Props>> vidProps, ExpansionType expType,
                          int statLevel);

        ~ExqDataHandlerECP();

        void loadData(int workers) override;

        IExqDescriptor<uint64_t>* getDescriptor(uint32_t i) override;

        IExqDescriptor<uint64_t>* getDescriptor(uint32_t i, int mod) override;

        int getTotalItemsCount(int mod) override;

        vector<bool> selectClusters(vector<int> b, vector<ExqClassifier*>& classifiers, //vector<vector<double>>& model, vector<double>& bias,
                                    ItemFilter& filters, bool resume=false) override;

        void getSegmentDescriptors(int currentSegment, int totalSegments, int modalities,
                                   vector<vector<IExqDescriptor<uint64_t>*>>& descriptors,
                                   unordered_set<uint32_t>& seenItems,
                                   ItemFilter& filters) override;

        void updateSessionInfo(vector<uint32_t> suggs) override;
        void resetSessionInfo();

    private:
        int _modalities;
        vector<int> _b;
        vector<ECPIndex*> _indx;
        vector<vector<ExqDescriptorR64*>> _descriptors;
        vector<string> _topFeatPaths;
        vector<string> _featIdsPaths;
        vector<string> _ratiosPaths;
    }; //End of class ExqIndex

} //End of namespace exq


#endif //EXQUISITOR_EXQDATAHANDLERECP_H
