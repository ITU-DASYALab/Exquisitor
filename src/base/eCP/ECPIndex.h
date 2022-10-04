#pragma once

#include "ECPConfig.h"
#include "ECPTree.h"
#include "ECPCluster.h"
#include "ECPFarthestNeighbour.h"
#include "ECPQueryOptimisationPolicies.h"

#include "../Metadata/ItemFilter.h"
#include "../ExqDescriptor.h"
#include "../ExqFunctions.h"

#include <string>
#include <cstdio>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>

namespace exq {
    using std::vector;
    using std::unordered_set;

    template <typename T, typename U, typename V>
    class ECPIndex {
    public:
        ECPIndex(ECPConfig* _cnfg, ExqFunctions<ExqDescriptor<T,U,V>>*& functions, int featureDimensions,
                 int modality, vector<ItemProperties> itemProps=vector<ItemProperties>(),
                 vector<vector<Props>> vidProps=vector<vector<Props>>(),
                 ExpansionType expansionType=ORIGINAL_CNT, int statLevel=1);

        ~ECPIndex();

        void loadDescriptors(vector<ExqDescriptor<T,U,V>*>& desc);

        void search(int chnk, int& totalData, vector<uint32_t>& suggIds,
                    int run, int segments, unordered_set<uint32_t>& seenItems, ItemFilter& filters);

        bool set_b_clusters(vector<double> query, double bias, int b, bool resume=false);

        void updateSessionInfo(vector<uint32_t> suggs);

        void PrintIndex();

        //uint64_t* next(int pos, int mod, int &index);

        void saveClusterDistribution(uint64_t numC, ECPConfig* cnfg, FILE* indxFile, FILE* dataFile, int mod);

    private:
        // Bookkeeping info
        ECPConfig* _cnfg;
        uint32_t _maxClusters;
        uint32_t _totalItems = 0;
        size_t _indexEntrySize;

        // query optimisation policies
        ECPQueryOptimisationPolicies<T,U,V>* _qop;

        // File and mapping info about the index file
        FILE* _indxFile;
        off_t _fileSize;

        // Info about the data file, which must be opened as well
        FILE* _dataFile;

        // Metadata info
        vector<ItemProperties> _itemProperties;
        //collection -> video -> property -> value(s)
        vector<vector<Props>> _vidProperties;

        // Data structures
        vector<ECPCluster<T,U,V>*> _clusters = vector<ECPCluster<T,U,V>*>();
        vector<uint32_t> _bClusters = vector<uint32_t>();
        ECPTree<T,U,V>* _tree;
    };
}