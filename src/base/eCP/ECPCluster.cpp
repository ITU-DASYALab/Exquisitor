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
ECPCluster<T,U,V>::ECPCluster(ECPConfig *_cnfg, FILE *_indxfile, FILE *_datafile, size_t descSize)
{
    // Note the bookkeeping info for later
    cnfg = _cnfg;
    datafile = _datafile;
    descriptorSize = descSize; //ATH breyta
    // Read the index entry info from the indx file
    size_t res;
    res = fread(&offset, sizeof(uint64_t), 1, _indxfile);
    res = fread(&pages, sizeof(int), 1, _indxfile);
    res = fread(&descriptors, sizeof(int), 1, _indxfile);
    if (res == 0) cout << "Error reading cluster" << endl;
    descriptorIds = vector<uint32_t>(descriptors);
    // Prepare for scans
    nextDescriptor = -1;
};

template <typename T, typename U, typename V>
ECPCluster<T,U,V>::~ECPCluster()
{
    for (int i = 0; i < this->descriptorList.size(); i++) {
        delete this->descriptorList[i];
    }
};

template <typename T, typename U, typename V>
void ECPCluster<T,U,V>::open()
{
    // Seek to the start of the cluster
    fseeko(datafile, ((off_t)offset) * cnfg->getPgeSize(), SEEK_SET);

    // Note that status of the scan
    nextDescriptor = 0;
}

template <typename T, typename U, typename V>
void ECPCluster<T,U,V>::close()
{
    nextDescriptor = 0;
};

template <typename T, typename U, typename V>
void ECPCluster<T,U,V>::setDescriptorId(int i, uint32_t id) {
    this->descriptorIds[i] = id;
}

template <typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ECPCluster<T,U,V>::next()
{
    if (nextDescriptor == descriptors) 
        return NULL;
    nextDescriptor++;
    return new ExqDescriptor<T,U,V>(datafile);
};

template <typename T, typename U, typename V>
void ECPCluster<T,U,V>::PrintCluster(string indent)
{
    cout << indent << "Offset: " << offset << endl;
    cout << indent << "Pages:  " << pages << endl;
    cout << indent << "Descrs: " << descriptors << endl;
    //for (int i = 0; i < descriptors; i++) {
    //    PrintDescriptor(descriptorList[i]);
    //}
}

template class exq::ECPCluster<uint64_t,uint64_t,uint64_t>;

