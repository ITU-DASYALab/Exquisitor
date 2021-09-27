#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstdio>
#include <sys/types.h>

using std::string;
using std::cout;
using std::endl;

class ECPConfig
{

    private:
        void SetParameter(string param, string value);

        uint64_t numDims;  // dimensionality of descriptors
        uint64_t numClst;  // clusters in index
        uint64_t numLvls;  // levels of tree index

        uint64_t pgeSize;  // Size of each page

        string pathIdx;  // path to index file
        string pathDat;  // data file path

        uint64_t k;        // neighbors to retrieve
        uint64_t b;        // clusters to search in

    public:
        ECPConfig(string filename);
        ~ECPConfig();

        uint64_t getNumDims()  { return numDims; };
        uint64_t getNumClst()  { return numClst; };
        uint64_t getNumLvls()  { return numLvls; };

        uint64_t getPgeSize()  { return pgeSize; };

        string getPathIdx()  { return pathIdx; };
        string getPathDat()  { return pathDat; };

        // exit with failure if config doesn't make sense
        void SanityCheck();
        void PrintConfig();
};
