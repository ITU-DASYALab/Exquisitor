//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLERECP_H
#define EXQUISITOR_EXQDATAHANDLERECP_H

#pragma once

#include <string>
#include <ExqDataHandler.h>
#include <ExqDescriptor.h>
#include <eCP/ECPIndex.h>

namespace exq {

    using std::string;

    template<typename T, typename U, typename V>
    class ExqDataHandlerECP : public ExqDataHandler<ExqDescriptor<T,U,V>>{
    public:
        /**
         * Constructor for loading the eCP index
         * \param cnfgFile - Config file containing metadata information about the cluster index
         */
        ExqDataHandlerECP(vector<string> cnfgFiles, int modalities);

        ~ExqDataHandlerECP();

        void loadData(int workers) override;

        ExqDescriptor<T,U,V>* getDescriptor(uint32_t i) override;

        ExqDescriptor<T,U,V>* getDescriptor(uint32_t i, int mod) override;

        int getTotalItemsCount(int mod) override;

        void selectClusters(vector<int> b, vector<vector<double>>& model, vector<double>& bias,
                            vector<ExqFunctions<ExqDescriptor<T,U,V>>>& functions) override;

        void getSegmentDescriptors(int currentSegment, int totalSegments,
                                   vector<vector<ExqDescriptor<T,U,V>>>& descriptors,
                                   unordered_set<uint32_t>& seenItems) override;

    private:
        int _modalities;
        vector<int> _b;
        vector<ECPIndex<T,U,V>*> _indx;
        vector<vector<ExqDescriptor<T,U,V>*>> _descriptors;
        vector<string> _topFeatPaths;
        vector<string> _featIdsPaths;
        vector<string> _ratiosPaths;

    }; //End of class ExqIndex

} //End of namespace exq


#endif //EXQUISITOR_EXQDATAHANDLERECP_H
