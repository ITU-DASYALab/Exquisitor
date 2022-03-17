#pragma once


#include "ECPConfig.h"
#include "../ExqDescriptor.h"

#include <vector>

namespace exq {

    using std::vector;
    // This class implements a cluster as part of a data file
    template <typename T, typename U, typename V>
    class ECPCluster {
    private:
        // Bookkeeping info
        ECPConfig* _cnfg;
        FILE* _datafile;       // File must be (and remain) open
        int _descriptorSize;

        // Info about the index entry
        uint32_t _offset;      // The page at which the cluster starts
        int _pages;            // The number of pages in the cluster
        int _descriptors;      // The number of descriptors in the cluster

        // Support for scanning an associated cluster or sub-entry
        int _nextDescriptor;

        int _iota;

    public:
        ECPCluster(ECPConfig *cnfg, FILE *indxfile, FILE *datafile, size_t descSize, int iota);

        ~ECPCluster();

        vector<ExqDescriptor<T,U,V>*> descriptorList;
        vector<uint32_t> descriptorIds;

        // Map the cluster and prepare for reading
        void open();

        void setDescriptorId(int i, uint32_t id);

        ExqDescriptor<T,U,V>* next();

        void close();

        int getNumDescriptors() { return _descriptors; }

        // Debugging
        void PrintCluster(string indent = "");
    };
}




