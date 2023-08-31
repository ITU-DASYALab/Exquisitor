#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstdio>
#include <sys/types.h>
#include <cstdint>

using std::string;
using std::cout;
using std::endl;

class ECPConfig
{

    private:
        void SetParameter(string param, string value);

        uint32_t numDims;  // dimensionality of descriptors
        uint32_t numClst;  // clusters in index
        uint32_t numLvls;  // levels of tree index
        uint32_t numOfst;  // cluster offset

        uint32_t pgeSize;  // Size of each page

        string pathIdx;  // path to index file
        string pathDat;  // data file path

        //uint32_t k;        // neighbors to retrieve
        //uint32_t b;        // clusters to search in

    public:
        ECPConfig(string filename);
        ~ECPConfig();

        uint32_t getNumDims()  { return numDims; };
        uint32_t getNumClst()  { return numClst; };
        uint32_t getNumLvls()  { return numLvls; };
        uint32_t getNumOfst()  { return numOfst; };

        uint32_t getPageSize()  { return pgeSize; };

        string getPathIdx()  { return pathIdx; };
        string getPathDat()  { return pathDat; };

        // exit with failure if config doesn't make sense
        void SanityCheck();
        void PrintConfig();
};
