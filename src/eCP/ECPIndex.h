#pragma once

#include "ECPConfig.h"
#include "ECPTree.h"
#include "ECPCluster.h"
#include "ECPFarthestNeighbour.h"

#include "../ExqDescriptor.h"
#include "../ExqFunctions.h"

#include <string>
#include <cstdio>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <vector>

namespace exq {
    using std::vector;

    template <typename T, typename U, typename V>
    class ECPIndex {
    public:
        ECPIndex(ECPConfig* _cnfg, ExqFunctions<ExqDescriptor<T,U,V>>*& functions);

        ~ECPIndex();

        void loadDescriptors(vector<ExqDescriptor<T,U,V>*>*);

        void search(int chnk, int& totalData, vector<uint32_t>& suggIds, int run, int segments);

        void set_b_clusters(vector<double>& query, double bias, int b);

        void PrintIndex();

        //uint64_t* next(int pos, int mod, int &index);

        void saveClusterDistribution(uint64_t numC, ECPConfig* cnfg, FILE* indxFile, FILE* dataFile, int mod);

    private:
        // Bookkeeping info
        ECPConfig* _cnfg;
        uint64_t _maxClusters;
        uint64_t _totalItems = 0;
        uint64_t _orgB;

        // File and mapping info about the index file
        FILE* _indxFile;
        off_t _fileSize;

        // Info about the data file, which must be opened as well
        FILE* _dataFile;

        // Data structures
        vector<ECPCluster<T,U,V>*> _clusters;
        vector<uint32_t> _bClusters;
        ECPTree<T,U,V>* _tree;
    };
}