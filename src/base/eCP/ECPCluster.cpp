#include "ECPCluster.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string>

using namespace exq;

ECPCluster::ECPCluster(ECPConfig *cnfg, FILE *indxfile, FILE *datafile, size_t descSize, int iota) {
    // Note the bookkeeping info for later
    _cnfg = cnfg;
    _datafile = datafile;
    _descriptorSize = descSize;
    // Read the index entry info from the indx file
    size_t res;
    res = fread(&_offset, sizeof(uint32_t), 1, indxfile);
    res = fread(&_pages, sizeof(int), 1, indxfile);
    res = fread(&_descriptors, sizeof(int), 1, indxfile);
    if (res == 0) cout << "Error reading cluster" << endl;
    descriptorIds = vector<uint32_t>(_descriptors);
    // Prepare for scans
    _nextDescriptor = -1;

    _iota = iota;
};

ECPCluster::~ECPCluster() {
    for (int i = 0; i < (int)descriptorList.size(); i++) {
        delete descriptorList[i];
    }
};

void ECPCluster::open() {
    // Seek to the start of the cluster
    fseeko(_datafile, ((off_t)_offset) * _cnfg->getPgeSize(), SEEK_SET);

    // Note that status of the scan
    _nextDescriptor = 0;
}

inline void ECPCluster::close() {
    _nextDescriptor = 0;
};

inline void ECPCluster::setDescriptorId(int i, uint32_t id) {
    this->descriptorIds[i] = id;
}

ExqDescriptorR64* ECPCluster::next() {
    //cout << "(ECPClst) _descriptors: " << _descriptors << endl;
    if (_nextDescriptor == _descriptors)
        return NULL;
    _nextDescriptor++;
    return new ExqDescriptorR64(_datafile, _iota, 1);
};

inline void ECPCluster::PrintCluster(string indent) {
    cout << indent << "Offset: " << _offset << endl;
    cout << indent << "Pages:  " << _pages << endl;
    cout << indent << "Descrs: " << _descriptors << endl;
    //for (int i = 0; i < descriptors; i++) {
    //    PrintDescriptor(descriptorList[i]);
    //}
}