//
// Created by Omar Shahbaz Khan on 07/07/2020.
//

#include "gtest/gtest.h"
#include "ExqWorker.h"
#include "ExqFunctionsR64.h"
#include "ExqDataHandlerH5.h"

class SingleModalityWorker: public ::testing::Test {
public:
    ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>* dataHandler;
    ExqFunctionsR64<uint64_t,uint64_t,uint64_t>* functionsR64;

    //TODO: Initialize all mock object and values to test suggest
    SingleModalityDataHandlerFixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compFiles, 1);
        this->dataHandler->loadData(activeModalities.size(), 1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityWorker() {
        delete dataHandler;
        delete functionsR64;
    }
};

TEST_F(SingleModalityWorker, getStuff) {

}
