#pragma once


#include "ECPConfig.h"
#include "../r64/ExqDescriptorR64.h"

#include <vector>

namespace exq {

    using std::vector;

    // This class implements a cluster as part of a data file
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

        vector<ExqDescriptorR64*> descriptorList;
        vector<uint32_t> descriptorIds;

        // Map the cluster and prepare for reading
        inline void open() {
            // Seek to the start of the cluster
#ifdef _MSC_VER
            _fseeki64(_datafile, ((__int64)_offset) * _cnfg->getPageSize(), SEEK_SET);
#else
            fseeko(_datafile, ((off_t)_offset) * _cnfg->getPageSize(), SEEK_SET);
#endif

            // Note that status of the scan
            _nextDescriptor = 0;
        }

        inline void close() {
            _nextDescriptor = 0;
        };

        inline ExqDescriptorR64* next() {
            //cout << "(ECPClst) _descriptors: " << _descriptors << endl;
            if (_nextDescriptor == _descriptors)
                return NULL;
            _nextDescriptor++;
            return new ExqDescriptorR64(_datafile, _iota, 1);
        };

        inline void setDescriptorId(int i, uint32_t id) {
            this->descriptorIds[i] = id;
        }

        inline int getNumDescriptors() { return _descriptors; }

        // Debugging
        inline void PrintCluster(string indent) {
            cout << indent << "Offset: " << _offset << endl;
            cout << indent << "Pages:  " << _pages << endl;
            cout << indent << "Descrs: " << _descriptors << endl;
            //for (int i = 0; i < descriptors; i++) {
            //    PrintDescriptor(descriptorList[i]);
            //}
        }
    };
}




