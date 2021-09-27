#pragma once


#include "ECPConfig.h"
#include "base/ExqDescriptor.h"

#include <vector>

namespace exq {

    using std::vector;
    // This class implements a cluster as part of a data file
    template <typename T, typename U, typename V>
    class ECPCluster {
    private:
        // Bookkeeping info
        ECPConfig* cnfg;
        FILE* datafile;       // File must be (and remain) open
        int descriptorSize;

        // Info about the index entry
        uint64_t offset;      // The page at which the cluster starts
        int pages;            // The number of pages in the cluster
        int descriptors;      // The number of descriptors in the cluster

        // Support for scanning an associated cluster or sub-entry
        int nextDescriptor;

    public:
        ECPCluster(ECPConfig *_cnfg, FILE *_indxfile, FILE *_datafile, size_t descSize);

        ~ECPCluster();

        vector<ExqDescriptor<T,U,V>*> descriptorList;
        vector<uint32_t> descriptorIds;

        // Map the cluster and prepare for reading
        void open();

        void setDescriptorId(int i, uint32_t id);

        ExqDescriptor<T,U,V>* next();

        void close();

        int getNumDescriptors() { return descriptors; }

        // Debugging
        void PrintCluster(string indent = "");
    };
}



