#include "ECPConfig.h"

ECPConfig::ECPConfig(string filename) {
    // Open the file
    FILE *file = fopen(filename.c_str(), "rb");
    if (file == 0) {
        cout << "ConfigMapper (" << filename << ")" << endl;
        cout << "Error: open config file (ConfigMapper.cpp)" << endl;
        exit(0);
    }

    // Get the lines from the file one by one
    char param[1000];
    char value[1000];

    // number of dimensions
    int res;
    while (!feof(file)) {
        res = fscanf(file, "%s %s\n", (char*)&param, (char*)&value);
        if (res == 0) cout << "Error reading config" << endl;
        SetParameter(param, value);
    }

    fclose(file);
}

void ECPConfig::SetParameter(string param, string value) {
    char* ptr;
    if (param == "dims:")
        numDims = strtol(value.c_str(), &ptr, 10);
    if (param == "clst:")
        numClst = strtol(value.c_str(), &ptr, 10);
    if (param == "lvls:")
        numLvls = strtol(value.c_str(), &ptr, 10);
    if (param == "page:")
        pgeSize = strtol(value.c_str(), &ptr, 10);
    if (param == "indx:")
        pathIdx = value;
    if (param == "data:")
        pathDat = value;
}


ECPConfig::~ECPConfig() {
}

#define CONFIG_WARNING "CONFIG WARNING: "
#define CONFIG_ERROR "CONFIG ERROR: "

void ECPConfig::SanityCheck() {
    if ((numDims % 16) != 0) {
        cout << CONFIG_ERROR << "Dimensionality must be a multiple of 16: numDims = " << numDims << endl;
        exit(EXIT_FAILURE);
    }

    if (pgeSize % 1024 != 0) {
        cout << CONFIG_ERROR << "Page size must be in whole KB (" << pgeSize << ")" << endl;
        exit(EXIT_FAILURE);
    }

    if (pgeSize % 4096 != 0) {
        cout << CONFIG_WARNING << "Page size is typically a multiple of 4KB (" << pgeSize << ")" << endl;
    }
}

void ECPConfig::PrintConfig()
{
    cout << "numDims: " << numDims << endl;
    cout << "numClst: " << numClst << endl;
    cout << "numLvls: " << numLvls << endl;
    cout << "pgeSize: " << pgeSize << endl;
    cout << "pathIdx: " << pathIdx << endl;
    cout << "pathDat: " << pathDat << endl;
}
