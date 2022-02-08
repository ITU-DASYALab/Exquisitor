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

template <typename T, typename U, typename V>
ECPCluster<T,U,V>::ECPCluster(ECPConfig *cnfg, FILE *indxfile, FILE *datafile, size_t descSize) {
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
};

template <typename T, typename U, typename V>
ECPCluster<T,U,V>::~ECPCluster() {
    for (int i = 0; i < this->descriptorList.size(); i++) {
        delete this->descriptorList[i];
    }
};

template <typename T, typename U, typename V>
void ECPCluster<T,U,V>::open() {
    // Seek to the start of the cluster
    fseeko(_datafile, ((off_t)_offset) * _cnfg->getPgeSize(), SEEK_SET);

    // Note that status of the scan
    _nextDescriptor = 0;
}

template <typename T, typename U, typename V>
inline void ECPCluster<T,U,V>::close() {
    _nextDescriptor = 0;
};

template <typename T, typename U, typename V>
inline void ECPCluster<T,U,V>::setDescriptorId(int i, uint32_t id) {
    this->descriptorIds[i] = id;
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ECPCluster<T,U,V>::next() {
    //cout << "(ECPClst) _descriptors: " << _descriptors << endl;
    if (_nextDescriptor == _descriptors)
        return NULL;
    _nextDescriptor++;
    return new ExqDescriptor<T,U,V>(_datafile, 1);
};

template <typename T, typename U, typename V>
inline void ECPCluster<T,U,V>::PrintCluster(string indent) {
    cout << indent << "Offset: " << _offset << endl;
    cout << indent << "Pages:  " << _pages << endl;
    cout << indent << "Descrs: " << _descriptors << endl;
    //for (int i = 0; i < descriptors; i++) {
    //    PrintDescriptor(descriptorList[i]);
    //}
}

template class exq::ECPCluster<uint64_t,uint64_t,uint64_t>;

